/**
 * lightshow entry point
 */

#include "main.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_conf.h"
#include "microphone.h"
#include "serial.h"
#include <stdio.h>

GPIO_InitTypeDef  GPIO_InitStructure;
extern volatile uint32_t Num_Ticks;
extern __IO uint32_t Audio_Available;

int main(void)
{
    int i = 0;
    uint16_t audio[16];
    int16_t audio_avg[10]; // Average of each of the last ten frames
    int16_t audio_avg_num = 0; // Number of averages in the audio avg buffer
    uint8_t bytes_sent = 0;

    /* Set up system tick */
    RCC_ClocksTypeDef RCC_Clocks;
  
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);

    /* GPIOA Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /* Configure PA7 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    WaveRecorderInit(32000,16, 1);
    WaveRecorderStart(audio, 128);
    serial_init();

    while(0)
    {
        /* Set PA7 */
        GPIOA->BSRRL = GPIO_Pin_7;

        i = Num_Ticks;
        while((Num_Ticks - i) < 100);
        
        /* Reset PA7 */
        GPIOA->BSRRH = GPIO_Pin_7;

        i = Num_Ticks;
        while((Num_Ticks - i) < 100);
    }

    while(1)
    {
        int32_t sum = 0;
        while(Audio_Available == 0);

        for(i = 0; i < 16; i++)
        {
            sum += (int16_t)audio[i];
        }

        audio_avg[audio_avg_num++] = sum /= 10;
        Audio_Available = 0;

        if(audio_avg_num >= 10)
        {
            int32_t last_average = 0;

            audio_avg_num = 0;

            for(i = 0; i < 10; i++)
            {
                last_average += (int32_t)audio_avg[i];
            }

            if(last_average > 50000)
            {
                GPIOA->BSRRL = GPIO_Pin_7;
                i = Num_Ticks;
                while((Num_Ticks - i) < 100);
            }
            else
            {
                GPIOA->BSRRH = GPIO_Pin_7;
            }
            bytes_sent++;

            if(bytes_sent > 20)
            {
                printf("hello\r\n");
                bytes_sent = 0;
            }
        }
    }
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
