//
//      Lab12_4: TMS320F28335
//      (C) Frank Bormann
//
//###########################################################################
//
// FILE:	Lab12_4.c
//
// TITLE:	DSP28335 I2C test
//          Temperature Sensor TMP100 connected to GPIO33 (SCL) and GPIO32(SDA)
//			LED GPIO34 as life indicator 100 ms toggle
//			12 bit mode of TMP100, 1/16 degree celcius resolution
//			use watch window for variable "temperature" (type int; qvalue:8)
//			new: I2C - in FIFO - mode
//			I2C - Interrupts in use
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2.0 | 24 Aug 2009 | F.B. | Lab12_4  F28335; Header-files Version 1.20 
//  3.1 | 15 Nov 2009 | F.B  | Lab12_1 for F28335 @30MHz and PE revision 5
//###########################################################################
#include "C:\Users\Kdt_T\workspace_ccstheia\Pro_t_II\LAB12_I2C_28335_CCSV5\Include_2833x\DSP2833x_Device.h"

// TMP100 commands
#define TMP100_SLAVE 			0x48	// slave address TMP101 (ADDR0=ADDR1=0)
#define POINTER_TEMPERATURE 	0	
#define POINTER_CONFIGURATION 	1
#define POINTER_T_LOW 			2
#define POINTER_T_HIGH 			3

// external function prototypes
extern void InitSysCtrl(void);
extern void InitPieCtrl(void);
extern void InitPieVectTable(void);
extern void InitCpuTimers(void);
extern void ConfigCpuTimer(struct CPUTIMER_VARS *, float, float);


// Prototype statements for functions found within this file.
void Gpio_select(void);
void I2CA_Init(void);
interrupt void cpu_timer0_isr(void);
interrupt void i2c_fifo_isr(void);
interrupt void i2c_basic_isr(void);

// global variables
int temperature;	// temperature = 2' Komplement of temparature (-128 ... +127 Celsius)
					// is an I8Q8 - Value 

//###########################################################################
//						main code									
//###########################################################################
void main(void)
{
	InitSysCtrl();	// Basic Core Init from DSP2833x_SysCtrl.c

	EALLOW;
   	SysCtrlRegs.WDCR= 0x00AF;	// Re-enable the watchdog 
   	EDIS;			// 0x00AF  to NOT disable the Watchdog, Prescaler = 64

	DINT;				// Disable all interrupts
	
	Gpio_select();	// GPIO9, GPIO11, GPIO34 and GPIO49 as output
					// to 4 LEDs at Peripheral Explorer	

	//	Initialize I2C
	I2CA_Init();

	// Send START, set pointer to Configuration register and set resolution to 12 bit
	I2caRegs.I2CCNT = 2;	
	I2caRegs.I2CDXR = POINTER_CONFIGURATION;
	I2caRegs.I2CDXR = 0x60;						// TMP101 in 12 bit mode (R1=R0=1)
	I2caRegs.I2CMDR.all = 0x6E20;
	/*	Bit15 = 0;	no NACK in receiver mode
		Bit14 = 1;  FREE on emulation halt
		Bit13 = 1;  STT  generate START 
		Bit12 = 0;	reserved
		Bit11 = 1;  STP  generate STOP
		Bit10 = 1;  MST  master mode
		Bit9  = 1;  TRX  master - transmitter mode
		Bit8  = 0;	XA   7 bit address mode
		Bit7  = 0;  RM   nonrepeat mode, I2CCNT determines # of bytes
		Bit6  = 1;  DLB  no loopback mode
		Bit5  = 1;  IRS  I2C module enabled
		Bit4  = 0;  STB  no start byte mode
		Bit3  = 0;  FDF  no free data format
		Bit2-0: 0;  BC   8 bit per data byte	*/

	while(I2caRegs.I2CSTR.bit.SCD == 0);			// wait for STOP condition
	I2caRegs.I2CSTR.bit.SCD = 1;					// clear SCD flag


	InitPieCtrl();		// basic setup of PIE table; from DSP2833x_PieCtrl.c
	InitPieVectTable();	// default ISR's in PIE

	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	PieVectTable.I2CINT2A = &i2c_fifo_isr;
	PieVectTable.I2CINT1A = &i2c_basic_isr;
	EDIS;

	InitCpuTimers();	// basic setup CPU Timer0, 1 and 2
	ConfigCpuTimer(&CpuTimer0,150,100000);

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		// cpu-timer 0
	PieCtrlRegs.PIEIER8.bit.INTx1 = 1;		// i2c - basic 
	PieCtrlRegs.PIEIER8.bit.INTx2 = 1;		// i2c - FIFO 

	IER |=0x81;

	EINT;
	ERTM;

	CpuTimer0Regs.TCR.bit.TSS = 0;	// start timer0

	while(1)
	{    
  		while(CpuTimer0.InterruptCount == 0);
		CpuTimer0.InterruptCount = 0;

		EALLOW;
		SysCtrlRegs.WDKEY = 0x55;	// service WD #1
		EDIS;

		//	Send START and set pointer to temperature - register
		I2caRegs.I2CCNT 	= 1;				// pointer to temperature register
		I2caRegs.I2CDXR		= POINTER_TEMPERATURE;
		// Send start as master transmitter
 		I2caRegs.I2CMDR.all = 0x6620;
		/*	Bit15 = 0;	no NACK in receiver mode
			Bit14 = 1;  FREE on emulation halt
			Bit13 = 1;  STT  generate START 
			Bit12 = 0;	reserved
			Bit11 = 0;  STP  not generate STOP
			Bit10 = 1;  MST  master mode
			Bit9  = 1;  TRX  master - transmitter mode
			Bit8  = 0;	XA   7 bit address mode
			Bit7  = 0;  RM   nonrepeat mode, I2CCNT determines # of bytes
			Bit6  = 1;  DLB  no loopback mode
			Bit5  = 1;  IRS  I2C module enabled
			Bit4  = 0;  STB  no start byte mode
			Bit3  = 0;  FDF  no free data format
			Bit2-0: 0;  BC   8 bit per data byte	*/
	
		// master - receiver mode is initialized in "i2c_basic_isr" after ARDY
		// read of temperature is done by ISR "i2c_fifo_isr"

	  	GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;		// toggle red LED LD3 @ 28335CC
	}
} 

void Gpio_select(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0;			// GPIO15 ... GPIO0 = General Puropse I/O
	GpioCtrlRegs.GPAMUX2.all = 0;			// GPIO31 ... GPIO16 = General Purpose I/O
	
	GpioCtrlRegs.GPBMUX1.all = 0;			// GPIO47 ... GPIO32 = General Purpose I/O
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;	// GPIO32 = I2C - SDA
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;	// GPIO33 = I2C - SCL

	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)

	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

	GpioCtrlRegs.GPBMUX2.all = 0;			// GPIO63 ... GPIO48 = General Purpose I/O

	GpioCtrlRegs.GPCMUX1.all = 0;			// GPIO79 ... GPIO64 = General Purpose I/O
	GpioCtrlRegs.GPCMUX2.all = 0;			// GPIO87 ... GPIO80 = General Purpose I/O
	 

	GpioCtrlRegs.GPADIR.all = 0;			// GPIO0 to 31 as inputs
	GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;		// GPIO9 = LED LD1
	GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;		// GpIO11 = LED LD2

	GpioCtrlRegs.GPBDIR.all = 0;			// GPIO63-32 as inputs
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;		// peripheral explorer: LED LD3 at GPIO34
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; 	// peripheral explorer: LED LD4 at GPIO49

	GpioCtrlRegs.GPCDIR.all = 0;			// GPIO87-64 as inputs
	EDIS;
}  

void I2CA_Init(void)
{
	
	I2caRegs.I2CMDR.bit.IRS = 0;	// Reset the I2C module
	// I2C slave address register
	I2caRegs.I2CSAR = TMP100_SLAVE;	
	// I2C Prescale Register
	I2caRegs.I2CPSC.all = 14;		// Internal I2C module clock = SYSCLOCK/(PSC +1)
								    // = 10 MHz
								    
	I2caRegs.I2CCLKL = 95;			// Tmaster = (PSC +1)[ICCL + 5 + ICCH + 5] / 150MHz
	I2caRegs.I2CCLKH = 95;			// Tmaster =  10 [ICCL + ICCH + 10] / 150 MHz 
									// d = 5  for IPSC >1
									
									// for I2C 50 kHz:
									// Tmaster = 20 �s *150 MHz / 10 = 200 = (ICCL + ICCH +10)  
									// ICCL + ICCH = 190
									// ICCL = ICH = 190/2 = 95	

//	I2caRegs.I2CCLKL = 45;			
//	I2caRegs.I2CCLKH = 45;			// for I2C 100 kHz:
									// Tmaster = 10 �s *150 MHz / 10 = 100 = (ICCL + ICCH + 10)  
									// ICCL + ICCH = 90
									// ICCL = ICH = 90/2 = 45
									 
	I2caRegs.I2CIER.bit.ARDY = 1;		// enable access ready interrupt

	I2caRegs.I2CFFTX.all = 0;			// reset TXFIFO
	I2caRegs.I2CFFTX.bit.TXFFIL  = 0;	// interrupt level = 0 (TXFIFO empty)
	I2caRegs.I2CFFTX.bit.I2CFFEN = 1;	// enable FIFOs	
	I2caRegs.I2CFFTX.bit.TXFFRST = 1;	// enable TX - operation


	I2caRegs.I2CFFRX.all = 0;			// reset RXFIFO
	I2caRegs.I2CFFRX.bit.RXFFIL = 2;	// set INT after 2 bytes
	I2caRegs.I2CFFRX.bit.RXFFRST = 1;	// enable RXFIFO 
	I2caRegs.I2CFFRX.bit.RXFFIENA = 1;	// enable RXFIFO - interrupt


	I2caRegs.I2CMDR.bit.IRS = 1;	// Take I2C out of reset
}

interrupt void cpu_timer0_isr(void)
{
	CpuTimer0.InterruptCount++;
	EALLOW;
	SysCtrlRegs.WDKEY = 0xAA;	// service WD #2
	EDIS;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void i2c_fifo_isr(void)
{
	int i;
	
	if(I2caRegs.I2CFFRX.bit.RXFFINT == 1)	// RX-FIFO - interrupt
	{
		i = I2caRegs.I2CDRR << 8;			// read upper 8 Bit (integers)
		i += I2caRegs.I2CDRR;				// read lower 8 Bit (fractions)
		temperature = i;

		I2caRegs.I2CFFRX.bit.RXFFINTCLR = 1;		// clear i2c - RXFIFOINT
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

interrupt void i2c_basic_isr(void)
{
	unsigned int IntSource;

   // Read interrupt source
   IntSource = I2caRegs.I2CISRC.all;

   if(IntSource == 3)			// ARDY
   {
		// continue with Master - Receiver - part
		I2caRegs.I2CCNT		= 2;	// read 2 byte temperature 
		I2caRegs.I2CMDR.all = 0x6C20;
		/*	Bit15 = 0;	no NACK in receiver mode
			Bit14 = 1;  FREE on emulation halt
			Bit13 = 1;  STT  generate START 
			Bit12 = 0;	reserved
			Bit11 = 1;  STP  generate STOP
			Bit10 = 1;  MST  master mode
			Bit9  = 0;  TRX  master - receiver mode
			Bit8  = 0;	XA   7 bit address mode
			Bit7  = 0;  RM   nonrepeat mode, I2CCNT determines # of bytes
			Bit6  = 0;  DLB  no loopback mode
			Bit5  = 1;  IRS  I2C module enabled
			Bit4  = 0;  STB  no start byte mode
			Bit3  = 0;  FDF  no free data format
			Bit2-0: 0;  BC   8 bit per data byte	*/
	}

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

//===========================================================================
// End of SourceCode.
//===========================================================================
