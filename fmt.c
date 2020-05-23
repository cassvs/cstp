// Formatting stuff

uint8_t toHex(uint8_t x) {
    if (x <= 9) {
        return x + '0';  // 0-9
    } else if (x >= 10 && x <= 15) {
        return x - 10 + 'a';   // a-f
    } else {
        return '_'; // _
    }
}

static inline uint8_t regConvert(char c) {
    return c - 'g';
}

uint8_t unHex(char c) {
    if ('0' <= c && c <= '9') {
        return c - '0';
    } else if ('a' <= c && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return 0;
    }
}
