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

/* Register map:
g:
h:
i:
j:
k:
l:
m:
n:
o:
p: motor power (signed)
q: deadband (signed (for some reason))
r:
s:
t:
u:
v:
w: target byte 0 (LSB)
x: target byte 1
y: target byte 2
z: target byte 3
*/

/* UART command syntax:
    Capital letter: Address
    lowercase letter: Register (g-z), hex numeral (a-f)
    Printable symbol: Command
    Newline (\r, \n): End of numeric input delimiter
    Other non-printables: Ignored

    Set device M's register 'g' to 255:  "Mgff\n"
    Test for the presence of a device A on the bus: "A?" (Should receive "A\r\n" if A exists)
    Get value stored in device B's register 'z': "Bz?" (Should receive "__\r\n", where _'s are hex numerals)
    Run: "A>"   Stop: "A."  Reset (drive until encoder is at zero): "A<"
    See uart.c for other commands.
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

uintmax_t ticks = 0;

struct flags_s {
    unsigned TICK   : 1;
    unsigned        : 7;
};

volatile struct flags_s flags = {0, 0};

int8_t power = 0;

void tick() {

    velocity = pos - prevPos;
    prevPos = pos;

    // Stupid drive-to-position
    if (pos < Reg_t('w', intmax_t) - Reg('q')) {
        power = Reg('p');
    } else if (pos > Reg_t('w', intmax_t) + Reg('q')) {
        power = -Reg('p');
    } else {
        power = 0;
    }

    setPower(power);

    ticks++;
}

void main(void) {
    oscInit(F_8M);
    pinsInit();
    uartInit();
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
    }
}
