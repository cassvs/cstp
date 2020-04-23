// UART setup and utility routines

void uartInit() {
    // 9600 baud
    SPBRG = 12;
    SPBRGH = 0;

    BAUDCTL = 0x40;
    RCSTA = 0x00;
    TXSTA = 0x22;

    SPEN = 1;
    CREN = 1;
}

void putchar(uint8_t c) {
    while (!TXIF)
        continue;
    TXREG = c;
}

uint8_t getchar() {
    while (!RCIF)
        continue;
    if (OERR) {
        CREN = 0;
        CREN = 1;
    }
    return RCREG;
}
