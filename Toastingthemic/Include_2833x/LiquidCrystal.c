#include "LiquidCrystal.h"
#include "DSP2833x_Device.h"

void delayMicroseconds(unsigned int us);

// Constructor for 8-bit mode
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    init(0, rs, 255, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

// Constructor for 8-bit mode with RW pin
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                             uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    init(0, rs, rw, enable, d0, d1, d2, d3, d4, d5, d6, d7);
}

// Constructor for 4-bit mode
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    init(1, rs, 255, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

// Constructor for 4-bit mode with RW pin
LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t rw, uint8_t enable,
                             uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    init(1, rs, rw, enable, d0, d1, d2, d3, 0, 0, 0, 0);
}

void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t rw, uint8_t enable,
                         uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                         uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
    _rs_pin = rs;
    _rw_pin = rw;
    _enable_pin = enable;

    _data_pins[0] = d0;
    _data_pins[1] = d1;
    _data_pins[2] = d2;
    _data_pins[3] = d3;
    _data_pins[4] = d4;
    _data_pins[5] = d5;
    _data_pins[6] = d6;
    _data_pins[7] = d7;

    if (fourbitmode)
        _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
    else
        _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;

    begin(16, 1);
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    if (lines > 1) {
        _displayfunction |= LCD_2LINE;
    }
    _numlines = lines;

    setRowOffsets(0x00, 0x40, 0x00 + cols, 0x40 + cols);

    if ((dotsize != LCD_5x8DOTS) && (lines == 1)) {
        _displayfunction |= LCD_5x10DOTS;
    }

    // Initialize GPIO pins
    EALLOW;
    GpioCtrlRegs.GPADIR.all |= (1 << _rs_pin) | (1 << _enable_pin);
    for (int i = 0; i < 8; ++i) {
        GpioCtrlRegs.GPADIR.all |= (1 << _data_pins[i]);
    }
    if (_rw_pin != 255) {
        GpioCtrlRegs.GPADIR.all |= (1 << _rw_pin);
    }
    EDIS;

    // Initial delay
    delayMicroseconds(50000);

    // Initialize LCD
    digitalWrite(_rs_pin, LOW);
    digitalWrite(_enable_pin, LOW);
    if (_rw_pin != 255) {
        digitalWrite(_rw_pin, LOW);
    }

    if (!(_displayfunction & LCD_8BITMODE)) {
        write4bits(0x03);
        delayMicroseconds(4500);
        write4bits(0x03);
        delayMicroseconds(4500);
        write4bits(0x03);
        delayMicroseconds(150);
        write4bits(0x02);
    } else {
        command(LCD_FUNCTIONSET | _displayfunction);
        delayMicroseconds(4500);
        command(LCD_FUNCTIONSET | _displayfunction);
        delayMicroseconds(150);
        command(LCD_FUNCTIONSET | _displayfunction);
    }

    command(LCD_FUNCTIONSET | _displayfunction);
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();
    clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::setRowOffsets(int row0, int row1, int row2, int row3) {
    _row_offsets[0] = row0;
    _row_offsets[1] = row1;
    _row_offsets[2] = row2;
    _row_offsets[3] = row3;
}

void LiquidCrystal::clear() {
    command(LCD_CLEARDISPLAY);
    delayMicroseconds(2000);
}

void LiquidCrystal::home() {
    command(LCD_RETURNHOME);
    delayMicroseconds(2000);
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row) {
    const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
    if (row >= max_lines) {
        row = max_lines - 1;
    }
    if (row >= _numlines) {
        row = _numlines - 1;
    }
    command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

void LiquidCrystal::noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::scrollDisplayLeft(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidCrystal::scrollDisplayRight(void) {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LiquidCrystal::leftToRight(void) {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::rightToLeft(void) {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::autoscroll(void) {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::noAutoscroll(void) {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x7;
    command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; i++) {
        write(charmap[i]);
    }
}

inline void LiquidCrystal::command(uint8_t value) {
    send(value, LOW);
}

inline size_t LiquidCrystal::write(uint8_t value) {
    send(value, HIGH);
    return 1;
}

void LiquidCrystal::send(uint8_t value, uint8_t mode) {
    digitalWrite(_rs_pin, mode);
    if (_rw_pin != 255) {
        digitalWrite(_rw_pin, LOW);
    }
    if (_displayfunction & LCD_8BITMODE) {
        write8bits(value);
    } else {
        write4bits(value >> 4);
        write4bits(value);
    }
}

void LiquidCrystal::pulseEnable(void) {
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(_enable_pin, HIGH);
    delayMicroseconds(1);
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(100);
}

void LiquidCrystal::write4bits(uint8_t value) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    pulseEnable();
}

void LiquidCrystal::write8bits(uint8_t value) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(_data_pins[i], (value >> i) & 0x01);
    }
    pulseEnable();
}

// Implement delayMicroseconds function
void delayMicroseconds(unsigned int us) {
    unsigned int cycles = us * 150;
    while (cycles > 0) {
        cycles--;
    }
}

void digitalWrite(uint8_t pin, uint8_t value) {
    if (value) {
        GpioDataRegs.GPASET.bit.GPIO0 + pin = 1;
    } else {
        GpioDataRegs.GPACLEAR.bit.GPIO0 + pin = 1;
    }
}