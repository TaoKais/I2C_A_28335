#include "C:\Users\Kdt_T\workspace_ccstheia\Pro_t_II\LAB12_I2C_28335_CCSV5\Include_2833x\DSP2833x_Device.h"
#include "C:\Users\Kdt_T\workspace_ccstheia\Pro_t_II\LAB12_I2C_28335_CCSV5\Include_2833x\DSP2833x_I2c.h"

// Define LCD I2C Address
#define LCD_I2C_ADDRESS 0x27  // Common address for I2C LCD backpacks

// Function Prototypes
void Gpio_select(void);
void I2CA_Init(void);
interrupt void cpu_timer0_isr(void);

// LCD Function Prototypes
void LCD_Init(void);
void I2C_SendCommand(unsigned char address, unsigned char cmd);
void I2C_SendData(unsigned char address, unsigned char data);
void LCD_Print(char *str);

// Define a simple delay function
void DELAY_US(Uint16 usec);

//###########################################################################
//                       main code
//###########################################################################
void main(void)
{
    InitSysCtrl();  // Basic Core Init from DSP2833x_SysCtrl.c

    EALLOW;
    SysCtrlRegs.WDCR = 0x00AF;  // Re-enable the watchdog
    EDIS;      // 0x00AF  to NOT disable the Watchdog, Prescaler = 64

    DINT;        // Disable all interrupts

    Gpio_select();  // GPIO9, GPIO11, GPIO34 and GPIO49 as output
                    // to 4 LEDs at Peripheral Explorer

    //  Initialize I2C
    I2CA_Init();

    InitPieCtrl();    // basic setup of PIE table; from DSP2833x_PieCtrl.c
    InitPieVectTable(); // default ISR's in PIE

    EALLOW;
    PieVectTable.TINT0 = &cpu_timer0_isr;
    EDIS;

    InitCpuTimers();  // basic setup CPU Timer0, 1 and 2
    ConfigCpuTimer(&CpuTimer0, 150, 100000);

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;    // cpu-timer 0

    IER |= 0x01;

    EINT;
    ERTM;

    CpuTimer0Regs.TCR.bit.TSS = 0;  // start timer0

    // Initialize LCD
    LCD_Init();

    // Print the message
    I2C_SendCommand(LCD_I2C_ADDRESS, 0x80);  // Set cursor to line 1, position 0
    LCD_Print("objetivo Logrado");

    while (1)
    {
        while (CpuTimer0.InterruptCount == 0);
        CpuTimer0.InterruptCount = 0;

        EALLOW;
        SysCtrlRegs.WDKEY = 0x55;  // service WD #1
        EDIS;

        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;    // toggle red LED LD3 @ 28335CC
    }
}

// LCD Function Definitions
void LCD_Init(void)
{
    // Initialize I2C module if not already done

    // Send initialization commands to the LCD
    I2C_SendCommand(LCD_I2C_ADDRESS, 0x33);  // 8-bit mode
    DELAY_US(100);
    I2C_SendCommand(LCD_I2C_ADDRESS, 0x32);  // 4-bit mode
    DELAY_US(100);
    I2C_SendCommand(LCD_I2C_ADDRESS, 0x28);  // 4-bit mode, 2 lines, 5x8 dots
    DELAY_US(100);
    I2C_SendCommand(LCD_I2C_ADDRESS, 0x0C);  // Display on, cursor off, blink off
    DELAY_US(100);
    I2C_SendCommand(LCD_I2C_ADDRESS, 0x01);  // Clear display
    DELAY_US(2000);  // Longer delay after clear display
}

void I2C_SendCommand(unsigned char address, unsigned char cmd)
{
    I2caRegs.I2CSAR = address;  // Set slave address

    I2caRegs.I2CCNT = 1;        // Send one byte
    I2caRegs.I2CDXR = cmd;      // Command byte
    I2caRegs.I2CMDR.all = 0x6220;  // Master transmitter mode, no STOP

    while (I2caRegs.I2CSTR.bit.SCD == 0); // Wait for STOP condition

    I2caRegs.I2CSTR.bit.SCD = 1;  // Clear STOP condition flag
}

void I2C_SendData(unsigned char address, unsigned char data)
{
    I2caRegs.I2CSAR = address;  // Set slave address

    I2caRegs.I2CCNT = 1;        // Send one byte
    I2caRegs.I2CDXR = data;      // Data byte
    I2caRegs.I2CMDR.all = 0x6220;  // Master transmitter mode, no STOP

    while (I2caRegs.I2CSTR.bit.SCD == 0); // Wait for STOP condition

    I2caRegs.I2CSTR.bit.SCD = 1;  // Clear STOP condition flag
}

void LCD_Print(char *str)
{
    while (*str)
    {
        I2C_SendData(LCD_I2C_ADDRESS, *str);
        str++;
    }
}

void Gpio_select(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.all = 0;      // GPIO15 ... GPIO0 = General Puropse I/O
    GpioCtrlRegs.GPAMUX2.all = 0;      // GPIO31 ... GPIO16 = General Purpose I/O

    GpioCtrlRegs.GPBMUX1.all = 0;      // GPIO47 ... GPIO32 = General Purpose I/O
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;  // GPIO32 = I2C - SDA
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;  // GPIO33 = I2C - SCL

    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;   // Enable pull-up for GPIO33 (SCLA)

    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

    GpioCtrlRegs.GPBMUX2.all = 0;      // GPIO63 ... GPIO48 = General Purpose I/O

    GpioCtrlRegs.GPCMUX1.all = 0;      // GPIO79 ... GPIO64 = General Purpose I/O
    GpioCtrlRegs.GPCMUX2.all = 0;      // GPIO87 ... GPIO80 = General Purpose I/O


    GpioCtrlRegs.GPADIR.all = 0;      // GPIO0 to 31 as inputs
    GpioCtrlRegs.GPADIR.bit.GPIO9 = 1;    // GPIO9 = LED LD1
    GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;   // GpIO11 = LED LD2

    GpioCtrlRegs.GPBDIR.all = 0;      // GPIO63-32 as inputs
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;    // peripheral explorer: LED LD3 at GPIO34
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;   // peripheral explorer: LED LD4 at GPIO49

    GpioCtrlRegs.GPCDIR.all = 0;      // GPIO87-64 as inputs
    EDIS;
}

void I2CA_Init(void)
{

    I2caRegs.I2CMDR.bit.IRS = 0;  // Reset the I2C module
    // I2C Prescale Register
    I2caRegs.I2CPSC.all = 14;    // Internal I2C module clock = SYSCLOCK/(PSC +1)
                                    // = 10 MHz

    I2caRegs.I2CCLKL = 95;      // Tmaster = (PSC +1)[ICCL + 5 + ICCH + 5] / 150MHz
    I2caRegs.I2CCLKH = 95;      // Tmaster =  10 [ICCL + ICCH + 10] / 150 MHz
                                    // d = 5  for IPSC >1

                                    // for I2C 50 kHz:
                                    // Tmaster = 20 �s *150 MHz / 10 = 200 = (ICCL + ICCH +10)
                                    // ICCL + ICCH = 190
                                    // ICCL = ICH = 190/2 = 95

    I2caRegs.I2CMDR.bit.IRS = 1;  // Take I2C out of reset
}

interrupt void cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;
    EALLOW;
    SysCtrlRegs.WDKEY = 0xAA;  // service WD #2
    EDIS;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// Define a simple delay function
void DELAY_US(Uint16 usec)
{
    Uint32 i;
    for (i = 0; i < (150 * usec); i++) {}  // Adjust the loop count based on your clock frequency
}
