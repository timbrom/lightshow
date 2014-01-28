CC=arm-none-eabi-gcc
CFLAGS=-Wall -Wextra -std=c99 -fno-common -O0 -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_STDPERIPH_DRIVER -g
CFLAGS+=-Isrc -Isrc/CMSIS/Include -Isrc/STM32F4xx -Isrc/STM32F4xx_StdPeriph_Driver/inc
LD=arm-none-eabi-ld
LDFLAGS=-Tlightshow.ld -nostartfiles -L/usr/arm-none-eabi/lib
AS=arm-none-eabi-as
ASFLAGS=-g

ODIR=build
LDIR=lib
SRCS=src

PERIPH_ODIR=STM32F4xx_StdPeriph_Driver

_OBJS=startup_stm32f40_41xxx.o main.o system_stm32f4xx.o microphone.o stm32f4xx_it.o
_PERIPH_OBJS=stm32f4xx_gpio.o stm32f4xx_rcc.o stm32f4xx_spi.o misc.o

PERIPH_OBJS = $(patsubst %,$(PERIPH_ODIR)/%,$(_PERIPH_OBJS))
_OBJS += $(PERIPH_OBJS)
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))


$(ODIR)/startup_stm32f40_41xxx.o: $(SRCS)/STM32F4xx/startup_stm32f40_41xxx.s
	$(AS) src/STM32F4xx/startup_stm32f40_41xxx.s -o $@ $(ASFLAGS)

$(ODIR)/%.o: $(SRCS)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/$(PERIPH_ODIR)/%.o: $(SRCS)/$(PERIPH_ODIR)/src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

lightshow: $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o build/flash.elf

.PHONY: clean

clean:
	rm -f $(ODIR)/flash.* $(ODIR)/*.o $(ODIR)/STM32F4xx/*.o $(ODIR)/STM32F4xx_StdPeriph_Driver/*.o *~ core $(INCDIR)/*~ 

