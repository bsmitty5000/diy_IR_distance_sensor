#include "helpers.h"

unsigned short sample_adc() {

    AD1CON1bits.SAMP = 1;
    while(AD1CON1bits.DONE == 0);
    AD1CON1bits.DONE = 0;
    return ADC1BUF0;
}
