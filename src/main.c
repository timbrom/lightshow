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

extern volatile uint32_t Num_Ticks;

void initGPIO(void);

int main(void)
{
    uint16_t audio[16];

    /* Set up system tick */
    RCC_ClocksTypeDef RCC_Clocks;
  
    /* SysTick end of count event each 1ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);

    /* Initialize peripherals */
    initGPIO();
    WaveRecorderInit();
    WaveRecorderStart(audio);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    RNG_Cmd(ENABLE);
    serial_init();
    timer_init();
    display_menu();
    timer_callback_handler = NULL;

    /* Begin superloop */
    while(1)
    {
        process_menu();
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
