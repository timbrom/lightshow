#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "serial.h"
#include "menu.h"
#include "timer.h"
#include "microphone.h"

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
static void handle_command();
static void equalizer(int16_t *data);
static void random(int16_t *data);
static void turnOnNum(uint8_t num);
static void turnOnMask(uint16_t mask);
static void walking_ones(void);
static void walking_zeros(void);
extern volatile uint32_t Num_Ticks;

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
                timer_startInterval((mask * 2) - 1);
            }
            break;
        case MENU_WALKING_ZEROS:
            /* Get the interval */
            {
                long int mask = strtol(endptr, NULL, 0);
                if(mask == 0) /* Default to 1 second */
                    mask = 1000;
                timer_callback_handler = walking_zeros;
                timer_startInterval((mask * 2) - 1);
            }
            break;
        default:
            printf("No such command\r\n");
            break;
        
    }
}

static void equalizer(int16_t *data)
{
    const uint16_t num_instant_energy_samples = 1600;
    const uint16_t num_historic_energy_samples = 50;

    static double historic_energy_samples[50];
    static int8_t historic_energy_sample_idx = 0;
    static double instant_energy_avg = 0;
    static int16_t instant_energy_avg_cnt = 0;

    /* Keep a running sum of each sample. */
    for(int i = 0; i < 16; i++)
        instant_energy_avg += data[i] * data[i];
    instant_energy_avg_cnt += 16;

    if(instant_energy_avg_cnt == num_instant_energy_samples) // We have 800 samples (1/20th of a second)
    {
        /* Find current min and max */
        double min = historic_energy_samples[0];
        double max = historic_energy_samples[0];
        for(int i = 1; i < num_historic_energy_samples; i++)
        {
            if(historic_energy_samples[i] < min)
                min = historic_energy_samples[i];
            else if(historic_energy_samples[i] > max)
                max = historic_energy_samples[i];
        }
        historic_energy_samples[historic_energy_sample_idx++] = instant_energy_avg;
        if(historic_energy_sample_idx >= num_historic_energy_samples)
            historic_energy_sample_idx = 0;

        double step = (max - min) / 8;
        uint16_t turn_on = (uint16_t)((instant_energy_avg - min) / step) + 0.5;
        turnOnNum(turn_on);

        instant_energy_avg = 0;
        instant_energy_avg_cnt = 0;
    }
}

/* Algorithm comes from http://archive.gamedev.net/archive/reference/programming/features/beatdetection/index.html */
static void random(int16_t *data)
{
    const uint16_t num_instant_energy_samples = 800;
    const uint16_t num_historic_energy_samples = 20;

    static double historic_energy_samples[20];
    static int8_t historic_energy_sample_idx = 0;
    static double instant_energy_avg = 0;
    static int16_t instant_energy_avg_cnt = 0;

    /* Keep a running sum of each sample. */
    for(int i = 0; i < 16; i++)
        instant_energy_avg += data[i] * data[i];
    instant_energy_avg_cnt += 16;

    if(instant_energy_avg_cnt == num_instant_energy_samples) // We have 800 samples (1/20th of a second)
    {
        double avg = 0;
        double var = 0;
        double C = 0;

        /* Compute the average */
        for(int i = 0; i < num_historic_energy_samples; i++)
        {
            avg += historic_energy_samples[i];
        }
        avg /= num_historic_energy_samples;

        /* Compute the variance */
        for(int i = 0; i < num_historic_energy_samples; i++)
        {
            double tmp = (historic_energy_samples[i] - avg);
            var += tmp * tmp;
        }
        var /= num_historic_energy_samples;

        /* Compute the constant C */
        C = 1.3;

        /* Store off this local average */
        historic_energy_samples[historic_energy_sample_idx++] = instant_energy_avg;
        if(historic_energy_sample_idx >= num_historic_energy_samples)
        {
            historic_energy_sample_idx = 0;
        }

        /* See if we have a beat */
        if(instant_energy_avg > (C * avg))
        {
            uint32_t randnum = RNG_GetRandomNumber();
            turnOnMask((uint16_t)(randnum & 0xFFFF));
        }
//printf("Inst: %lf, Avg: %lf, Var: %lf, C: %lf, C*avg: %lf\r\n", instant_energy_avg, avg, var, C, C*avg);
        instant_energy_avg_cnt = 0;
        instant_energy_avg = 0;        
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

