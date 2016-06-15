//main.c
#include "init.h"
#include "uart.h"
#include "helpers.h"
#include <math.h>

// ******************************************************************************************* //
// Configuration bits for CONFIG1 settings. 
//
// Make sure "Configuration Bits set in code." option is checked in MPLAB.
// This option can be set by selecting "Configuration Bits..." under the Configure
// menu in MPLAB.

_FOSC(OSCIOFNC_ON & FCKSM_CSDCMD & POSCMD_NONE);	//Oscillator Configuration (clock switching: disabled;
							// failsafe clock monitor: disabled; OSC2 pin function: digital IO;
							// primary oscillator mode: disabled)
_FOSCSEL(FNOSC_FRCPLL);					//Oscillator Selection PLL
_FWDT(FWDTEN_OFF);					//Turn off WatchDog Timer
_FGS(GCP_OFF);						//Turn off code protect
_FPOR(FPWRT_PWR1);					//Turn off power up timer

//holds the char sent
volatile char uart_rcvd_char;
//flag indicating char has been rcvd
volatile char uart_rcvd;

unsigned char sample_time;
unsigned short raw_sample;

int main()
{

    //initialize everything
    uart_rcvd_char = 0;
    uart_rcvd = 0;
    sample_time = 0;

    //initializations
    InitClock();
    InitUART1();
    InitTimer1();
    InitADC1();

    //wait for signal from PC
    //commenting when doing the measuring
    //while(uart_rcvd == 0);

    //don't turn on when doing the measuring
    T1CONbits.TON = 0;

    while(1) {

        if (uart_rcvd == 1) {
            
            uart_rcvd = 0;
            raw_sample = sample_adc();
            sendShort(raw_sample);
        }
        
    }

    return 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{

	// Clear interrupt flag
	IFS0bits.U1RXIF = 0;
        //let the main loop know we received a char
        uart_rcvd = 1;
        //load the char
        uart_rcvd_char = U1RXREG;
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{

    // Clear Timer 1 interrupt flag to allow another Timer 1 interrupt to occur.
    IFS0bits.T1IF = 0;
    sample_time = 1;
}