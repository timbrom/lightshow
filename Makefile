CC=arm-none-eabi-gcc
CFLAGS=-Wall -Werror -Wextra -std=c99 -fno-common -ffreestanding -O0 -g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_STDPERIPH_DRIVER
CFLAGS+=-Isrc -Isrc/CMSIS/Include -Isrc/STM32F4xx -Isrc/STM32F4xx_StdPeriph_Driver/inc
LD=arm-none-eabi-ld
LDFLAGS=-Tlightshow.ld -nostartfiles -L/usr/arm-none-eabi/lib
AS=arm-none-eabi-as

ODIR=build
LDIR=lib
SRCS=src

build/STM32F4xx/startup_stm32f40_41xxx.o: 
	$(AS) src/STM32F4xx/startup_stm32f40_41xxx.s -o $@

$(ODIR)/%.o: $(SRCS)%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/STM32F4xx/*.o $(ODIR)/STM32F4xx_StdPeriph_Driver/*.o *~ core $(INCDIR)/*~ 

