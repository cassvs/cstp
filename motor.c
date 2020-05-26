// P1 Motor Controller

// $ sdcc --use-non-free -mpic14 -p16f690 motor.c
// $ pk2cmd -M -PPIC16f690 -Fmotor.hex

/*
                                  _________
                             Vdd [|1  U 20|] Vss
                       Encoder A [|       |] ICSP Data
                       Encoder B [|       |] ICSP Clock
                           !MCLR [| PIC   |]
                         PWM Fwd [|  16F  |]
                         PWM Rev [|   690 |]
                                 [|       |]
                                 [|       |]
                                 [|       |] UART Rx
                         UART Tx [|10___11|]
*/

#include <stdint.h>
#include <pic14regs.h>

typedef enum RunState {
    IDLE,
    RUNNING,
    RESET
} RunState;

volatile RunState runState = IDLE;

#include "math.c"
#include "init.c"
#include "fmt.c"
#include "encoder.c"
#include "uart.c"
#include "pwm.c"

#define Reg(x) registers[regConvert(x)]
#define Reg_t(x, t) *(t *)(registers + regConvert(x))

// Config:
static __code uint16_t __at (_CONFIG) configWord = _INTOSCIO & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOR_OFF & _IESO_ON & _FCMEN_OFF;

// const int8_t TargetVelocity = 4;
//
// #define AvgLen 8
// int8_t avgbuf[AvgLen];
// int8_t averageVelocity;
// uint8_t avgIndex = 0;

uintmax_t ticks = 0;

struct flags_s {
    unsigned TICK   : 1;
    unsigned        : 7;
};

volatile struct flags_s flags = {0, 0};

int8_t power = 0;
// int8_t step = 1;

void tick() {
    // if ((step > 0 && (int8_t)(power + step) < power) || (step < 0 && (int8_t)(power + step) > power)) {
    //     step *= -1;
    // }
    // power += step;

    velocity = pos - prevPos;
    prevPos = pos;

    // avgbuf[avgIndex] = velocity;
    // avgIndex = (avgIndex + 1) % AvgLen;
    //
    // averageVelocity = average(avgbuf, AvgLen);
    //
    // if (averageVelocity < TargetVelocity) {
    //     power = bound16(-127, 127, (int16_t)power + 1);
    // } else if (averageVelocity > TargetVelocity) {
    //     power = bound16(-127, 127, (int16_t)power - 1);
    // }

    // Stupid drive-to-position
    if (pos < Reg_t('w', intmax_t) - Reg('q')) {
        power = Reg('p');
    } else if (pos > Reg_t('w', intmax_t) + Reg('q')) {
        power = -Reg('p');
    } else {
        power = 0;
    }

    setPower(power);
    //
    // putchar_buf(toHex((velocity & 0b11110000) >> 4));
    // putchar_buf(toHex(velocity & 0b00001111));
    // puts("\r\n");

    ticks++;
}

void main(void) {
    oscInit(F_8M);
    pinsInit();
    uartInit();
    //TXIE = 1;
    RCIE = 1;
    pwmInit();
    TRISC5 = 0;
    TRISC4 = 0;
    encoderInit();
    tickInit();
    PEIE = 1;
    GIE = 1;

    puts("Init done\r\n");

    while (1) {
        if (flags.TICK) {
            flags.TICK = 0;
            switch (runState) {
                case RUNNING:
                    tick();
                    break;
                case RESET:
                    if ((pos <= 0 && prevPos >= 0) || (pos >= 0 && prevPos <= 0)) {
                        runState = IDLE;
                    } else {
                        setPower((pos > 0) ? -Reg('p') : Reg('p'));
                    }
                    break;
                case IDLE:
                    setPower(0);
                    break;
            }
        }
    }
}

void interrupt(void) __interrupt 0 {
    if (RABIF) {
        encoderISR();
    }
    if (RCIF) {
        rxISR();
    }
    if (TXIF) {
        txISR();
    }
    if (T0IF) {
        T0IF = 0;
        flags.TICK = 1;
        //tick();
    }
}
