#include "init.h"

void InitUART1() {

    //must set to digital
    AD1PCFGLbits.PCFG4 = 1;
    AD1PCFGLbits.PCFG5 = 1;

    //Page 181 of dsPIC33FJ datasheet. This ties RP2 to UART1 RX
    RPINR18bits.U1RXR = 2;
    TRISBbits.TRISB2 = 1;

    // Page 189 of dsPIC33FJ datasheet. This ties RP3 to UART1 TX
    //Table 11-2 lists the decoded values for this register pg 167
    RPOR1bits.RP3R = 3;

    U1MODEbits.STSEL = 0; //1 stop bit
    U1MODEbits.PDSEL = 0; //8 bit data, no parity
    U1MODEbits.ABAUD = 0; //auto-baud disabled
    U1MODEbits.BRGH = 0; //standard speed mode

    //check ref manual uart section for calculation
    if (BAUDRATE == 115200)
        U1BRG = 20;
    else if (BAUDRATE == 9600)
        U1BRG = 256;
    else
        U1BRG = 0xFF;

    //not using interrupts for transmit, polling instead
    //U1STAbits.UTXISEL0 = 0;
    //U1STAbits.UTXISEL1 = 0;
    //IEC0bits.U1TXIE = 1;

    //interrupt after one character is rcvd
    U1STAbits.URXISEL = 0;
    
    //clear flag then enable interrupts
    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;

    U1MODEbits.UARTEN = 1; //enable uart
    U1STAbits.UTXEN = 1; //transmitter enabled

    //IFS0bits.U1TXIF = 0;

}

void InitClock() {
    /* Configure Oscillator to operate the device at 40Mhz
       Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
       Fosc= 7.37*(43)/(2*2)=80Mhz for Fosc, Fcy = 40Mhz */
    PLLFBD = 41; // M = 43
    CLKDIVbits.PLLPOST = 0; // N1 = 2
    CLKDIVbits.PLLPRE = 0; // N2 = 2
    OSCTUN = 0;
    RCONbits.SWDTEN = 0;

    // Clock switch to incorporate PLL
    __builtin_write_OSCCONH(0x01); // Initiate Clock Switch to
    // FRC with PLL (NOSC=0b001)
    __builtin_write_OSCCONL(0x01); // Start clock switching
    while (OSCCONbits.COSC != 0b001); // Wait for Clock switch to occur

    while (OSCCONbits.LOCK != 1) {
    };
}

void InitTimer1()
{
	// Clear Timer value (i.e. current tiemr value) to 0
	TMR1 = 0;
        
	T1CONbits.TCS = 0; //source is Fcy
	T1CONbits.TCKPS = 2; //1:64; period = 1.6us
        //Set PR1 to 10ms
	PR1 = 37500; //6250;

	// Clear Timer 1 interrupt flag. This allows us to detect the
	// first interupt.
	IFS0bits.T1IF = 0;

	// Enable the interrupt for Timer 1
	IEC0bits.T1IE = 1;
}

void InitADC1() {

    AD1CON1bits.AD12B = 0; //10 bit operation
    AD1CON2bits.VCFG = 0; //using Vdd & Vss as reference
    AD1CON3bits.ADCS = 0x0F; //setting Tad. must be at least 75ns
    //Tad = (ADCS + 1) * Tcy = 16 * (1/40Mhz) = 400ns
    AD1CON3bits.SAMC = 0x01; //auto sample time set to 1 Tad
    //sample + conversion time = (SAMC)*Tad + 12*Tad = 13*Tad = 5.2us
    AD1PCFGL = 0xFFFF; //setting all ports to digital
    AD1PCFGLbits.PCFG0 = 0; //sets AN0 to analog
    AD1CHS0bits.CH0NA = 0; //channel 0 negative input is Vss
    AD1CHS0bits.CH0SA = 0; //setting positive input to AN0
    AD1CON2bits.CHPS = 0; //convert channle 0 only
    AD1CON1bits.ASAM = 0; //manual sampling
    AD1CON1bits.SSRC = 0b111; //manual sample & auto-conversion
    AD1CON1bits.FORM = 0b00; // integer output, top 6 bits 0
    AD1CON2bits.SMPI = 0; //not really using DMA
    IFS0bits.AD1IF = 0; //adc interrupt flag reset
    IEC0bits.AD1IE = 0; //interrupt disabled
    AD1CON1bits.ADON = 1; //turn the key
}