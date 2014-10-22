/**
 * lightshow entry point
 */

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_rng.h"
#include "microphone.h"
#include "serial.h"
#include "menu.h"
#include "timer.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_SAMPLES 30

extern volatile uint32_t Num_Ticks;
extern __IO uint32_t Audio_Available;
void (*sample_collected_fp)(int16_t max, int16_t min, int32_t last_average);

void initGPIO(void);

int main(void)
{
    int i = 0;
    uint16_t audio[16];
    int16_t audio_avg[NUM_SAMPLES]; // Average of each of the last NUM_SAMPLES frames
    int16_t audio_avg_num = 0; // Number of averages in the audio avg buffer
    int16_t max = 0xFFFE;
    uint32_t max_time = 0;
    int16_t min = 0x7FFE;
    uint32_t min_time = 0;

    /* Set up system tick */
    RCC_ClocksTypeDef RCC_Clocks;
  
    /* SysTick end of count event each 1ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    /* Initialize peripherals */
    initGPIO();
    WaveRecorderInit();
    WaveRecorderStart(audio, 16);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
    serial_init();
    timer_init();
    display_menu();
    sample_collected_fp = NULL;
    timer_callback_handler = NULL;

    /* Begin superloop */
    while(1)
    {
        int32_t sum = 0;

        /* Wait for audio to become available (16 samples) */
        while(Audio_Available == 0);

        /* Sum the 16 samples collected */
        for(i = 0; i < 16; i++)
        {
            sum += (int16_t)audio[i];
        }

        /* Collect the average of these 16 samples */
        audio_avg[audio_avg_num] = sum /= 16;
        Audio_Available = 0;

        audio_avg_num++;

        /* Once we have NUM_SAMPLES averages */
        if(audio_avg_num >= NUM_SAMPLES)
        {
            int32_t last_average = 0;

            /* Decay max and min if too much time has elapsed */
            if(Num_Ticks > max_time + 10000)
                max -= 50;
            if(Num_Ticks > min_time + 10000)
                min += 50;
            audio_avg_num = 0;

            /* Compute the average of the last NUM_SAMPLES averages (average of average) */
            for(i = 0; i < NUM_SAMPLES; i++)
            {
                last_average += (int32_t)audio_avg[i];
            }
            last_average /= NUM_SAMPLES;

            /* Track max and min. The first samples come back as MIN_S16, so handle that case */
            if(last_average > max)
            {
                max = last_average;
                max_time = Num_Ticks;
            }
            if(last_average < min || min < -32000)
            {
                min = last_average;
                min_time = Num_Ticks;
            }

            if(sample_collected_fp != NULL)
                sample_collected_fp(max, min, last_average);

            process_menu();
        }
    }
}

void initGPIO(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    /* GPIOA Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    /* Configure PA7 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
    (void)file;
    line++;
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
