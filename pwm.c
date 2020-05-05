// PWM routines

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

void setPower(int8_t p) {
    if (sign(p) == 1) {
        STRA = 1;
        STRB = 0;
    } else {
        STRA = 0;
        STRB = 1;
    }
    CCPR1L = 2 * abs(bound(-127, 127, p));
}
