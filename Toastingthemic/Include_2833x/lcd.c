#include "lcd.h"

// Define GPIO pins for LCD control
#define LCD_RS GpioDataRegs.GPBDAT.bit.GPIO49
#define LCD_EN GpioDataRegs.GPBDAT.bit.GPIO48
#define LCD_D4 GpioDataRegs.GPBDAT.bit.GPIO47
#define LCD_D5 GpioDataRegs.GPBDAT.bit.GPIO46
#define LCD_D6 GpioDataRegs.GPBDAT.bit.GPIO45
#define LCD_D7 GpioDataRegs.GPBDAT.bit.GPIO44

static void LCD_Enable(void);
static void LCD_Write4Bits(unsigned char data);

void LCD_Init(void)
{
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1; // RS
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1; // EN
    GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1; // D4
    GpioCtrlRegs.GPBDIR.bit.GPIO46 = 1; // D5
    GpioCtrlRegs.GPBDIR.bit.GPIO45 = 1; // D6
    GpioCtrlRegs.GPBDIR.bit.GPIO44 = 1; // D7
    EDIS;

    LCD_Command(0x03);
    LCD_Command(0x03);
    LCD_Command(0x03);
    LCD_Command(0x02);

    LCD_Command(LCD_CMD_FUNCTION_SET | LCD_4BIT_MODE | LCD_2_LINE | LCD_5x8DOTS);
    LCD_Command(LCD_CMD_DISPLAY_CONTROL | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF);
    LCD_Command(LCD_CMD_CLEAR_DISPLAY);
    LCD_Command(LCD_CMD_ENTRY_MODE_SET | LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT);
}

void LCD_Clear(void)
{
    LCD_Command(LCD_CMD_CLEAR_DISPLAY);
}

void LCD_Home(void)
{
    LCD_Command(LCD_CMD_RETURN_HOME);
}

void LCD_SetCursor(int row, int col)
{
    int row_offsets[] = {0x00, 0x40};
    if (row > 1) row = 1;
    LCD_Command(LCD_CMD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void LCD_Print(const char *str)
{
    while (*str)
    {
        LCD_Write(*str++);
    }
}

void LCD_Command(unsigned char cmd)
{
    LCD_RS = 0;
    LCD_Write4Bits(cmd >> 4);
    LCD_Write4Bits(cmd);
}

void LCD_Write(unsigned char data)
{
    LCD_RS = 1;
    LCD_Write4Bits(data >> 4);
    LCD_Write4Bits(data);
}

static void LCD_Enable(void)
{
    LCD_EN = 1;
    DELAY_US(1);
    LCD_EN = 0;
    DELAY_US(100);
}

static void LCD_Write4Bits(unsigned char data)
{
    LCD_D4 = (data >> 0) & 0x01;
    LCD_D5 = (data >> 1) & 0x01;
    LCD_D6 = (data >> 2) & 0x01;
    LCD_D7 = (data >> 3) & 0x01;
    LCD_Enable();
}