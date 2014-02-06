/**
 * lightshow entry point
 */

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_conf.h"
#include "microphone.h"
#include "serial.h"
#include <stdlib.h>
#include <stdio.h>

#define NUM_SAMPLES 50

extern volatile uint32_t Num_Ticks;
extern __IO uint32_t Audio_Available;

void initGPIO(void);
void turnOnNum(uint8_t num);

int main(void)
{
    int i = 0;
    uint16_t audio[16];
    int16_t audio_avg[NUM_SAMPLES]; // Average of each of the last NUM_SAMPLES frames
    int16_t audio_avg_num = 0; // Number of averages in the audio avg buffer
    int16_t max = 0xFFFE;
    int16_t min = 0x7FFE;

    /* Set up system tick */
    RCC_ClocksTypeDef RCC_Clocks;
  
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

    /* Initialize peripherals */
    initGPIO();
    WaveRecorderInit(32000,16, 1);
    WaveRecorderStart(audio, 128);
    serial_init();

    /* Begin superloop */
    while(1)
    {
        int32_t sum = 0;
        while(Audio_Available == 0);

        for(i = 0; i < 16; i++)
        {
            sum += (int16_t)audio[i];
        }

        audio_avg[audio_avg_num] = sum /= 16;
        Audio_Available = 0;

        audio_avg_num++;

        if(audio_avg_num >= NUM_SAMPLES)
        {
            int32_t last_average = 0;

            max -= 10;
            min += 10;
            audio_avg_num = 0;

            for(i = 0; i < NUM_SAMPLES; i++)
            {
                last_average += (int32_t)audio_avg[i];
            }
            last_average /= NUM_SAMPLES;

            if(last_average > max)
            {
                max = last_average;
            }
            if(last_average < min || min < -32000)
            {
                min = last_average;
            }

            int16_t step = (max - min) / 8;
            int8_t num_on = (last_average - min) / step;

            printf("Max: %d Min: %d Step: %d Num: %d\r\n", max, min, step, num_on);
            turnOnNum(num_on);
        }
    }
}

/**
 * Turns on first N outlets
 *
 * @param[in] num Number to turn on
 */
void turnOnNum(uint8_t num)
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
