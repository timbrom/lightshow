
#define TOP_OF_STACK 0x20000800

static void nmi_handler(void);
static void hf_handler(void);
int main(void);

// Set up the vector table
unsigned int *vec_table[4] __attribute__ ((section("vectors"))) = {
    (unsigned int*) TOP_OF_STACK,       // The stack pointer
    (unsigned int*) main,               // Entry point
    (unsigned int*) nmi_handler,        // NMI Handler
    (unsigned int*) hf_handler,         // The hard fault handler
};

static void set_freq(void)
{
    rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_168MHZ]);
    
    rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN);
}

static void gpio_setup(void)
{
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,
                   GPIO12 | GPIO13 | GPIO14 | GPIO15);
}

int main(void)
{
    int i;

    set_freq();
    gpio_setup();

    gpio_set(GPIOD, GPIO12  | GPIO14);

    while(1)
    {
        gpio_toggle(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
        for (i = 0; i < 6000000; i++)
            __asm__("nop");
    }

    return 0;
}

void nmi_handler(void)
{
    for(;;);
}

void hf_handler(void)
{
    for(;;);
}

