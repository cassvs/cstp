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
