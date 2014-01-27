/* Entry Point */
ENTRY(Reset_Handler)

_estack = 0x20030000;

MEMORY
{
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 192K
    rom (rx)    : ORIGIN = 0x08000000, LENGTH = 1024K
}

SECTIONS
{
    . = 0x0;

    .text :
    {
        *(.isr_vector)
        *(.text)
    } >rom

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)
        . = ALIGN(4);
    } >rom

    . = 0x20000000;
    _sidata = LOADADDR(.data);
    .data :
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    } >ram AT > rom

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        __bss_start__ = .;
        *(.bss)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = .;
    } >ram
}

