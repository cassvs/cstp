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

// Config:
static __code uint16_t __at (_CONFIG) configWord = _INTOSCIO & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _CP_OFF & _CPD_OFF & _BOR_OFF & _IESO_ON & _FCMEN_OFF;

int8_t power = 0;
int8_t step = 1;

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

void tick() {
    if ((step > 0 && (int8_t)(power + step) < power) || (step < 0 && (int8_t)(power + step) > power)) {
        step *= -1;
        putchar((step < 0) ? 'F' : 'R');
    }
    power += step;
    if (power == 0)
        putchar('0');
    setPower(power);
}

void main(void) {
    oscInit(F_8M);
    pinsInit();
    uartInit();
    pwmInit();
    TRISC5 = 0;
    TRISC4 = 0;
    tickInit();
    PEIE = 1;
    GIE = 1;

    while(1){};
}

void interrupt(void) __interrupt 0 {
    if (T0IF) {
        T0IF = 0;
        tick();
    }
}
