// Math functions

uint8_t abs(int8_t x) {
    if (x >= 0) {
        return x;
    } else {
        return (uint8_t)(x * -1);
    }
}

int8_t sign(int8_t x) {
    if (x >= 0) {
        return 1;
    } else {
        return -1;
    }
}

int8_t bound(int8_t l, int8_t u, int8_t x) {
    if (x < l) {
        return l;
    } else if (x > u) {
        return u;
    } else {
        return x;
    }
}

int8_t bound16(int8_t l, int8_t u, int16_t x) {
    if (x < l) {
        return l;
    } else if (x > u) {
        return u;
    } else {
        return x;
    }
}

int8_t average(int8_t* list, uint8_t len) {
    int16_t sum = 0;
    for (uint8_t i = 0; i < len; i++) {
        sum += list[i];
    }
    return sum / len;
}
