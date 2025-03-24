//
//      Lab12_4: TMS320F28335
//      (C) Frank Bormann
//
//###########################################################################
//
// FILE:	Lab12_4.c
//
// TITLE:	DSP28335 I2C test
//          Variable Voltage on GPIO32 (SDA) and LCD 16x2 display
//			LED GPIO34 as life indicator 100 ms toggle
//			use watch window for variable "voltage" (type int; qvalue:8)
//			new: I2C - in FIFO - mode
//			I2C - Interrupts in use
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  2.0 | 24 Aug 2009 | F.B. | Lab12_4  F28335; Header-files Version 1.20 
//  3.1 | 15 Nov 2009 | F.B  | Lab12_1 for F28335 @30MHz and PE revision 5
//  4.0 | 24 Mar 2025 | T.K. | Modified to use variable voltage on GPIO32, added LCD display, GPIO blinking
//###########################################################################
#include "C:\Users\Kdt_T\workspace_ccstheia\Pro_t_II\LAB12_I2C_28335_CCSV5\Include_2833x\DSP2833x_Device.h"
#include "lcd.h"

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
int voltage; // voltage value

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

	//	Initialize I2C
	I2CA_Init();

	// Initialize LCD
	LCD_Init();
    LCD_Clear();
    LCD_Home();
    LCD_Print("Voltage:");

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

		// Set SDA line based on voltage value
		if (voltage > 128) {
			GpioDataRegs.GPBDAT.bit.GPIO32 = 1; // Set SDA line high
		} else {
			GpioDataRegs.GPBDAT.bit.GPIO32 = 0; // Set SDA line low
		}

        // Update LCD with voltage value
        char buffer[16];
        sprintf(buffer, "Voltage: %d", voltage);
        LCD_SetCursor(1, 0);
        LCD_Print(buffer);

		// Toggle GPIOs 09, 11, 34, and 49
		GpioDataRegs.GPATOGGLE.bit.GPIO9 = 1;   // Toggle GPIO9
		GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1;  // Toggle GPIO11
		GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;  // Toggle GPIO34
		GpioDataRegs.GPBTOGGLE.bit.GPIO49 = 1;  // Toggle GPIO49
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
	I2caRegs.I2CSAR = 0x48; // Dummy slave address
	// I2C Prescale Register
	I2caRegs.I2CPSC.all = 14;		// Internal I2C module clock = SYSCLOCK/(PSC +1)
								    // = 10 MHz
								    
	I2caRegs.I2CCLKL = 95;			// Tmaster = (PSC +1)[ICCL + 5 + ICCH + 5] / 150MHz
	I2caRegs.I2CCLKH = 95;			// Tmaster =  10 [ICCL + ICCH + 10] / 150 MHz 
									// d = 5  for IPSC >1
									
									// for I2C 50 kHz:
									// Tmaster = 20 μs *150 MHz / 10 = 200 = (ICCL + ICCH +10)  
									// ICCL + ICCH = 190
									// ICCL = ICH = 190/2 = 95	

//	I2caRegs.I2CCLKL = 45;			
//	I2caRegs.I2CCLKH = 45;			// for I2C 100 kHz:
									// Tmaster = 10 μs *150 MHz / 10 = 100 = (ICCL + ICCH + 10)  
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
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

interrupt void i2c_basic_isr(void)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

//===========================================================================
// End of SourceCode.
//===========================================================================