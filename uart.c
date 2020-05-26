// UART setup and utility routines

#define TXLEN 16
#define REGLEN 20

uint8_t txbuf[TXLEN];
uint8_t txread = 0;
uint8_t txwrite = 0;
uint8_t txlock = 0;

const char Address = 'M';

int8_t registers[REGLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

typedef enum RxState {
    LISTEN,
    GETREG,
    GETVAL
} RxState;

volatile RxState RecState = LISTEN;
volatile uint8_t reg = 0;
volatile int8_t val = 0;

typedef enum ByteType {
    UNKNOWN,
    ADDRESS,
    NUMERAL,
    COMMAND,
    REGISTER,
    NEWLINE
} ByteType;

ByteType getType(char c) {
    if ('A' <= c && c <= 'Z') {
        return ADDRESS;
    } else if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f')) {
        return NUMERAL;
    } else if ('g' <= c && c <= 'z') {
        return REGISTER;
    } else if ((' ' <= c && c <= '/') || (':' <= c && c <= '@') || ('[' <= c && c <= '`') || ('{' <= c && c <= '~')) {
        return COMMAND;
    } else if (c == '\n' || c == '\r') {
        return NEWLINE;
    } else {
        return UNKNOWN;
    }
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

void hexOut(uint8_t x) {
    putchar_buf(toHex((x & 0b11110000) >> 4));
    putchar_buf(toHex(x & 0b00001111));
}

void regCommand(char c) {
    switch(c) {
        case '?':
            hexOut(registers[reg]);
            puts("\r\n");
            break;
    }
}

void devCommand(char c) {
    switch (c) {
        case '?':
            putchar_buf(Address);
            puts("\r\n");
            break;
        case '@':
            putchar_buf(reg + 'g');
            puts("\r\n");
            break;
        case '#':
            hexOut(val);
            puts("\r\n");
            break;
        case '.':
            runState = IDLE;
            break;
        case '<':
            runState = RESET;
            break;
        case '>':
            runState = RUNNING;
            break;
        case '~':
            pos = prevPos = 0;
            break;
        // case ',':
        // // TODO: Make this work. Writes to Tx ring buffer waaay too fast
        //     for (uint8_t i = 0; i < REGLEN; i++) {
        //         putchar_buf(i + 'g');
        //         puts(": ");
        //         hexOut(registers[i]);
        //         puts("\r\n");
        //     }
        //     break;
        case ';':
            hexOut(runState);
            puts("\r\n");
            break;
    }
}

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

// uint8_t getchar() {
//     while (!RCIF)
//         continue;
//     if (OERR) {
//         CREN = 0;
//         CREN = 1;
//     }
//     return RCREG;
// }

inline void txISR() {
    if (!txlock && txwrite != txread) {
        TXREG = *(txbuf + txread);
        txread = (txread + 1) % TXLEN;
    }
    if (txwrite == txread) {
        TXIE = 0;
    }
}

inline void rxISR() {
    char c = RCREG;
    if (OERR) {
        CREN = 0;
        CREN = 1;
    }
    ByteType t = getType(c);
    if (t == ADDRESS) {
        if (c == Address) {
            RecState = GETREG;
        } else {
            RecState = LISTEN;
        }
    } else {
        switch(RecState) {
            case LISTEN:
                break;

            case GETREG:
                switch(t) {
                    case REGISTER:
                        reg = regConvert(c);
                        RecState = GETVAL;
                        break;
                    case COMMAND:
                        devCommand(c);
                        RecState = LISTEN;
                        break;
                }
                break;

            case GETVAL:
                switch(t) {
                    case NUMERAL:
                        val = val << 4;
                        val = val | unHex(c);
                        break;
                    case NEWLINE:
                        registers[reg] = val;
                        reg = 0;
                        val = 0;
                        RecState = LISTEN;
                        break;
                    case COMMAND:
                        regCommand(c);
                        RecState = LISTEN;
                        break;
                }
                break;
        }
    }
    //putchar_buf(c);
}
