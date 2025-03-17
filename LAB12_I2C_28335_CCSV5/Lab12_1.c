//
//      Lab12_1: TMS320F28335
//      (C) Frank Bormann
//
//###########################################################################
//
// FILE:	Lab12_1.c
//
// TITLE:	DSP28335 I2C test
//          Temperature Sensor TMP100 connected to GPIO33 (SCL) and GPIO32(SDA)
//			LED LD1 as life indicator 100 ms toggle
//			Basic use of TMP100 (9 bit mode, 0.5 celcius resolution)
//			use watch window for variable "temperature" (type int; qvalue:8)
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  3.0 | 24 Jul 2009 | F.B. | Lab12_1  F28335; Header-files Version 1.20 
//  3.1 | 15 Nov 2009 | F.B  | Lab12_1 for F28335 @30MHz and PE revision 5
//###########################################################################
#include "C:\Users\Kdt_T\workspace_ccstheia\Pro_t_II\LAB12_I2C_28335_CCSV5\Include_2833x\DSP2833x_Device.h"

// TMP100 commands
#define TMP100_SLAVE 			0x48	// slave address TMP100 (ADDR0=ADDR1=0)
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

//###########################################################################
//						main code									
//###########################################################################
void main(void)
{
	int temperature;	// temperature = 2' Komplement of temparature (-128 ... +127 Celsius)
						// is an I8Q8 - Value 

	InitSysCtrl();	// Basic Core Init from DSP2833x_SysCtrl.c

	EALLOW;
   	SysCtrlRegs.WDCR= 0x00AF;	// Re-enable the watchdog 
   	EDIS;			// 0x00AF  to NOT disable the Watchdog, Prescaler = 64

	DINT;				// Disable all interrupts
	
	Gpio_select();	// GPIO9, GPIO11, GPIO34 and GPIO49 as output
					// to 4 LEDs at Peripheral Explorer	

	//	Initialize I2C
	I2CA_Init();

	InitPieCtrl();		// basic setup of PIE table; from DSP2833x_PieCtrl.c
	InitPieVectTable();	// default ISR's in PIE

	EALLOW;
	PieVectTable.TINT0 = &cpu_timer0_isr;
	EDIS;

	InitCpuTimers();	// basic setup CPU Timer0, 1 and 2
	ConfigCpuTimer(&CpuTimer0,150,100000);

	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

	IER |=1;

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

			while(I2caRegs.I2CSTR.bit.RRDY == 0);	// wait for 1st byte
			temperature = I2caRegs.I2CDRR << 8;			// read upper 8 Bit (integers)
			// RRDY is automatically cleared by read of I2CDRR 
			while(I2caRegs.I2CSTR.bit.RRDY == 0);	// wait for 2nd byte
			temperature += I2caRegs.I2CDRR;				// read lower 8 Bit (fractions)

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
	I2caRegs.I2CPSC.all = 14;		// Internal I2C module clock = SYSCLK/(PSC +1)
								    // = 10 MHz
								    
	I2caRegs.I2CCLKL = 95;			// Tmaster = (PSC +1)[ICCL + 5 + ICCH + 5] / 150MHz
	I2caRegs.I2CCLKH = 95;			// Tmaster =  15 [ICCL + ICCH + 10] / 150 MHz 
									// d = 5  for IPSC >1
									
									// for I2C 50 kHz:
									// Tmaster = 20 �s * 150 MHz / 15 = 200 = (ICCL + ICCH +10)  
									// ICCL + ICCH = 190
									// ICCL = ICH = 190/2 = 95	

//	I2caRegs.I2CCLKL = 45;			
//	I2caRegs.I2CCLKH = 45;			// for I2C 100 kHz:
									// Tmaster = 10 �s *150 MHz / 15 = 100 = (ICCL + ICCH + 10)  
									// ICCL + ICCH = 90
									// ICCL = ICH = 90/2 = 45 	

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
//===========================================================================
// End of SourceCode.
//===========================================================================
