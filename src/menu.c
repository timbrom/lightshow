#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "serial.h"
#include "menu.h"
#include "timer.h"

char* Menu_Commands_Text[MENU_NUM_ITEMS] = {
    "Turn all off",
    "Turn on (mask)",
    "Random (0 - steady, 1 - change on beat)",
    "Equalizer",
    "Turn all outputs on",
    "Walking Ones",
    "Walking Zeros",
};

char command[RXBUFFERSIZE + 1];
uint8_t command_len = 0;
extern void (*sample_collected_fp)(int16_t max, int16_t min, int32_t last_average);
static void handle_command();
static void equalizer(int16_t max, int16_t min, int32_t last_average);
static void random(int16_t max, int16_t min, int32_t last_average);
static void turnOnNum(uint8_t num);
static void turnOnMask(uint16_t mask);
static void walking_ones(void);
static void walking_zeros(void);

void display_menu()
{
    for(int i = 0; i < MENU_NUM_ITEMS; i++)
    {
        printf("%d - %s\r\n", i, Menu_Commands_Text[i]);
    }
    printf("\r\n");
}

void process_menu()
{
    uint8_t read_len = read(0, &command[command_len], RXBUFFERSIZE - command_len);
    command_len += read_len;

    /* Wait to receive entire command */
    if((command_len > 0) && (command[command_len-1] == '\r'))
    {
        command[command_len] = '\0';
        printf("Command: %s\r\n", command);
        display_menu();

        handle_command();

        command_len = 0;
    }

    /* Prevent overflow */
    if(command_len >= RXBUFFERSIZE)
        command_len = 0;
}

static void handle_command()
{
    char *endptr;
    long int command_num = strtol(command, &endptr, 0);
    sample_collected_fp = NULL; /* Stop collecting samples */
    timer_callback_handler = NULL;
    timer_stopInterval();

    switch(command_num)
    {
        case MENU_OFF:
            turnOnNum(0);
            break;
        case MENU_ON:
            /* Get the turn on mask */
            {
                long int mask = strtol(endptr, NULL, 0);
                turnOnMask(mask);
                break;
            }
        case MENU_RANDOM:
            {
                long int submenu = strtol(endptr, NULL, 0);

                if(submenu == 0)
                {
                    uint32_t randnum = RNG_GetRandomNumber();
                    turnOnMask((uint16_t)(randnum & 0xFFFF));
                }
                else if(submenu == 1)
                {
                    sample_collected_fp = random;
                }
                else
                {
                    printf("Invalid submenu");
                }
                break;
            }
        case MENU_EQ:
            sample_collected_fp = equalizer;
            break;
        case MENU_ALL_ON:
            turnOnMask(0xFFFF);
            break;
        case MENU_WALKING_ONES:
            /* Get the interval */
            {
                long int mask = strtol(endptr, NULL, 0);
                if(mask == 0) /* Default to 1 second */
                    mask = 1000;
                timer_callback_handler = walking_ones;
                timer_startInterval(mask * 2);
            }
            break;
        case MENU_WALKING_ZEROS:
            /* Get the interval */
            {
                long int mask = strtol(endptr, NULL, 0);
                if(mask == 0) /* Default to 1 second */
                    mask = 1000;
                timer_callback_handler = walking_zeros;
                timer_startInterval(mask * 2);
            }
            break;
        default:
            printf("No such command\r\n");
            break;
        
    }
}

static void equalizer(int16_t max, int16_t min, int32_t last_average)
{
    int16_t step = (max - min) / 8;
    int8_t num_on = (last_average - min) / step;

    turnOnNum(num_on);
}

static void random(int16_t max, int16_t min, int32_t last_average)
{
    static bool armed = true;
    (void)min;

    printf("Max: %d Min: %d Last Average: %d\r\n", max, min, last_average);

    /* If I'm armed, when the last average is loud (start of a beat) then 
     * change the outputs and disarm. */
    if(armed)
    {
        if(last_average > ((3 * max) / 4))
        {
            uint32_t randnum = RNG_GetRandomNumber();
            turnOnMask((uint16_t)(randnum & 0xFFFF));
            armed = false;
        }
    }
    /* If unarmed, re-arm when things are quiet */
    else
    {
        if(last_average < (max / 2))
        {
            armed = true;
        }
    }
}

/**
 * Turns on first N outlets
 *
 * @param[in] num Number to turn on
 */
static void turnOnNum(uint8_t num)
{

    uint16_t reg_val = 0x0000;
    switch(num)
    {
    case 8:
        reg_val |= GPIO_Pin_7;
    case 7:
        reg_val |= GPIO_Pin_6;
    case 6:
        reg_val |= GPIO_Pin_5;
    case 5:
        reg_val |= GPIO_Pin_4;
    case 4:
        reg_val |= GPIO_Pin_3;
    case 3:
        reg_val |= GPIO_Pin_2;
    case 2:
        reg_val |= GPIO_Pin_1;
    case 1:
        reg_val |= GPIO_Pin_0;
        break;
    case 0:
        reg_val = 0;
        break;
    default:
        reg_val = 0xFFFF;
    }

    GPIOE->BSRRL = reg_val;
    GPIOE->BSRRH = ~reg_val;
}

/**
 * Turns on outlets according to mask
 *
 * @param[in] num Number to turn on
 */
static void turnOnMask(uint16_t mask)
{
    GPIOE->BSRRL = mask;
    GPIOE->BSRRH = ~mask;
}

/**
 * Walks a 1 across the lines
 */
static void walking_ones()
{
    static uint16_t mask = 1;

    turnOnMask(mask);

    mask <<= 1;
    if(mask > 0xF0)
    {
        mask = 1;
    }
}

/**
 * Walks a 0 across the lines
 */
static void walking_zeros()
{
    static uint16_t mask = 0xFE;

    turnOnMask(mask);

    mask <<= 1;
    mask |= 1;
    if((mask & 0xFF) == 0xFF)
    {
        mask = 0xFE;
    }
}

