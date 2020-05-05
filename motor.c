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
#include "math.c"
#include "init.c"
#include "uart.c"
#include "pwm.c"
#include "encoder.c"
#include "fmt.c"

// Config:
static __code uint16_t __at (_CONFIG) configWord = _INTOSCIO & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOR_OFF & _IESO_ON & _FCMEN_OFF;

const intmax_t Target = 0x2ff;

uintmax_t ticks = 0;

struct flags_s {
    unsigned TICK   : 1;
    unsigned        : 7;
};

volatile struct flags_s flags = {0, 0};

// int8_t power = 0;
// int8_t step = 1;

void tick() {
    // if ((step > 0 && (int8_t)(power + step) < power) || (step < 0 && (int8_t)(power + step) > power)) {
    //     step *= -1;
    // }
    // power += step;

    velocity = pos - prevPos;
    prevPos = pos;

    if (pos < Target) {
        setPower(127);
    } else {
        setPower(0);
    }

    ticks++;
}

void main(void) {
    oscInit(F_8M);
    pinsInit();
    uartInit();
    //TXIE = 1;
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
            tick();
        }
    }
}

void interrupt(void) __interrupt 0 {
    if (RABIF) {
        encoderISR();
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
