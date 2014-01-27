
MEMORY
{
    ram (rwx)  : ORIGIN = 0x20000000, LENGTH = 192K
    rom (rx)   : ORIGIN = 0x00000000, LENGTH = 1024K
}

SECTIONS
{
    . = 0x0;
    .text :
    {
        *(vectors)
        *(.text)
        *(.rodata)
    } >rom

    . = 0x20000000;
    .data :
    {
        *(.data)
    } >ram AT > rom

    .bss :
    {
        *(.bss)
    } >ram AT > rom
}

