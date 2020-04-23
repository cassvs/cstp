// PWM setup routines

void pwmInit() {
    PR2 = 0xff;

    // PWM mode, all outputs active high
    CCP1CONbits.CCP1M = 0b1100;

    // Initial output value
    CCPR1L = 0;

    // Output LSBs
    CCP1CONbits.DC1B = 0b00;

    // Timer
    TMR2IF = 0;
    T2CONbits.T2CKPS = 0;
    TMR2ON = 1;

    STRSYNC = 1;

    while(!TMR2IF){};
}
