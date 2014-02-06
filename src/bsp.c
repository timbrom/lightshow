/* Functions needed for newlib */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "serial.h"

extern unsigned int _heap_bottom;
extern unsigned int _heap_top;

void* _sbrk(int incr)
{
    static void *heap_end;
    void *prev_heap_end;

    if (heap_end == 0) 
    {
        heap_end = &_heap_bottom;
    }

    prev_heap_end = heap_end;
    if (heap_end + incr > (void*)&_heap_top)
    {
        return 0;
    }

    heap_end += incr;
    return (void*)prev_heap_end;
}

int _write(int file, char *ptr, int len)
{
    while(len > 0)
    {
        __io_putchar(*ptr++);
        len--;
    }
}

int _close(int file)
{
    return -1;
}

int _fstat(int file, struct stat *st)
{
    return 0;
}

int _isatty(int file)
{
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    return 0;
}

int _read(int file, char *ptr, int len)
{
    return 0;
}
