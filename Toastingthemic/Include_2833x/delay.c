#include "delay.h"

void DELAY_US(unsigned long microseconds)
{
    // Assuming a 150MHz clock, 1 cycle is ~6.67ns
    // 1 microsecond is 150 cycles
    unsigned long cycles = microseconds * 150;

    while (cycles > 0)
    {
        cycles--;
    }
}