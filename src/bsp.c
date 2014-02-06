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
    (void)&file;
    int num_written = 0;
    while(len > 0)
    {
        __io_putchar(*ptr++);
        len--;
        num_written++;
    }

    return num_written;
}

int _close(int file)
{
    (void)&file;
    return -1;
}

int _fstat(int file, struct stat *st)
{
    (void)&file;
    (void)st;
    return 0;
}

int _isatty(int file)
{
    (void)&file;
    return 1;
}

int _lseek(int file, int ptr, int dir)
{
    (void)&file;
    (void)&ptr;
    (void)&dir;
    return 0;
}

int _read(int file, char *ptr, int len)
{
    (void)&file;
    (void)&ptr;
    (void)&len;
    return 0;
}
