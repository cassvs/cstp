// Encoder routines and globals

volatile intmax_t pos = 0;
volatile intmax_t prevPos = 0;
volatile int8_t velocity = 0;
volatile uint8_t encState = 0;
volatile uint8_t prevEnc = 0;
volatile uint8_t encRaw = 0;
const int8_t encMap[16] = {0, 1, -1, 0,
                           -1, 0, 0, 1,
                           1, 0, 0, -1,
                           0, -1, 1, 0};

void encoderInit() {
    IOCA4 = 1;
    IOCA5 = 1;
    RABIF = 0;
    RABIE = 1;
}

inline void encoderISR() {
    encRaw = PORTA;
    RABIF = 0;
    prevEnc = encState;
    encState = (encRaw & 0b00110000) >> 4;
    pos += encMap[(encState << 2) + prevEnc];
    
    // if (encState == 0) {
    //     if (prevEnc == 2) {
    //         pos --;
    //     } else if (prevEnc == 1) {
    //         pos ++;
    //     }
    // } else if (encState == 3) {
    //     if (prevEnc == 2) {
    //         pos ++;
    //     } else if (prevEnc == 1) {
    //         pos --;
    //     }
    // }
}
