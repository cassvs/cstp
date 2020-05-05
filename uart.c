// UART setup and utility routines

#define TXLEN 16

uint8_t txbuf[TXLEN];
uint8_t txread = 0;
uint8_t txwrite = 0;
uint8_t txlock = 0;

void uartInit() {
    // 9600 baud
    SPBRG = 12;
    SPBRGH = 1;

    BAUDCTL = 0x40;
    RCSTA = 0x00;
    TXSTA = 0x02;

    SPEN = 1;
    CREN = 1;
    TXEN = 1;
}

// void putchar(uint8_t c) {
//     while (!TXIF)
//         continue;
//     TXREG = c;
// }

uint8_t getchar() {
    while (!RCIF)
        continue;
    if (OERR) {
        CREN = 0;
        CREN = 1;
    }
    return RCREG;
}

void putchar_buf(uint8_t c) {
    while (txlock)
        continue;
    txlock = 1;
    *(txbuf + txwrite) = c;
    txwrite = (txwrite + 1) % TXLEN;
    TXIE = 1;
    txlock = 0;
}

void puts(uint8_t *str) {
    for (uint8_t i = 0; str[i] != 0; i++) {
        putchar_buf(str[i]);
    }
}

inline void txISR() {
    if (!txlock && txwrite != txread) {
        TXREG = *(txbuf + txread);
        txread = (txread + 1) % TXLEN;
    }
    if (txwrite == txread) {
        TXIE = 0;
    }
}
