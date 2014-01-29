/**
 * lightshow entry point
 */

#include "main.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_conf.h"
#include "microphone.h"

GPIO_InitTypeDef  GPIO_InitStructure;
extern volatile uint32_t Num_Ticks;

int main(void)
{
    int i = 0;
    uint16_t audio[128];
    float tmp = 15;

    tmp /= 3.413f;

    /* Set up system tick */
    RCC_ClocksTypeDef RCC_Clocks;
  
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1680);

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

    while (1)
    {
        /* Set PA7 */
        GPIOA->BSRRL = GPIO_Pin_7;

        i = Num_Ticks;
        while((Num_Ticks - i) < 1000);
        
        /* Reset PA7 */
        GPIOA->BSRRH = GPIO_Pin_7;

        i = Num_Ticks;
        while((Num_Ticks - i) < 1000);
    }

    while(1)
    {
      i++;
      tmp = i;
      tmp /= 3.01f;
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
