// Initialization routines and constants

typedef enum OscFrequency {
    F_31K,
    F_125K,
    F_250K,
    F_500K,
    F_1M,
    F_2M,
    F_4M,
    F_8M
} OscFrequency;

void oscInit(OscFrequency f) {
    switch(f) {
        case F_31K:
            OSCCONbits.IRCF = 0b000;
            break;
        case F_125K:
            OSCCONbits.IRCF = 0b001;
            break;
        case F_250K:
            OSCCONbits.IRCF = 0b010;
            break;
        case F_500K:
            OSCCONbits.IRCF = 0b011;
            break;
        case F_1M:
            OSCCONbits.IRCF = 0b100;
            break;
        case F_2M:
            OSCCONbits.IRCF = 0b101;
            break;
        case F_4M:
            OSCCONbits.IRCF = 0b110;
            break;
        case F_8M:
            OSCCONbits.IRCF = 0b111;
            break;
    }
}

void pinsInit() {
    // All pins high-z
    TRISA = 0xff;
    TRISB = 0xff;
    TRISC = 0xff;

    // All IO latches clear
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;

    // No analog channels
    ANSEL = 0x00;
    ANSELH = 0x00;
}

void tickInit() {
    T0CS = 0;   // Clock source Fosc/4
    PSA = 0;    // Take prescaler away from WDT
    OPTION_REGbits.PS = 0b111; // Prescaler 1:256
    TMR0 = 0;   // Clear counter and prescaler
    T0IF = 0;   // Clear interrupt flag
    T0IE = 1;   // Enable interrupt
}
