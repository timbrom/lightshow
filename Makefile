CC=arm-none-eabi-gcc
CFLAGS="-Wall -Werror -Wextra -std=c99 -fno-common -O0 -g -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16"
LD=arm-none-eabi-ld
LDFLAGS="-Tlightshow.ld -nostartfiles -L/usr/arm-none-eabi/lib"

ODIR=build
LDIR=lib
SRCS=src

$(ODIR)/%.o: $(SRCS)%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

