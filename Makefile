CC		:= sdcc
CFLAGS	:= --use-non-free
PORT 	:= pic14
DEVICE	:= 16f690

FLASHUTIL	:= pk2cmd
FLASHFLAGS	:= -M

TARGET	:= motor
DEPS	:= init.c math.c fmt.c encoder.c pwm.c uart.c

.PHONY: all flash

all: $(TARGET)

flash: $(TARGET).hex
	$(FLASHUTIL) $(FLASHFLAGS) -PPIC$(DEVICE) -F$(TARGET).hex

$(TARGET): %: %.c $(DEPS)
	$(CC) $(CFLAGS) -m$(PORT) -p$(DEVICE) $<
