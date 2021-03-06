/**
  ******************************************************************************
  * @file    Audio_playback_and_record/inc/waverecorder.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    28-October-2011
  * @brief   Header for waverecorder.c module
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2S_AUDIO_H
#define __I2S_AUDIO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


/* Exported types ------------------------------------------------------------*/
/* Exported Defines ----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void AUDIO_REC_SPI_IRQHANDLER(void);
uint32_t WaveRecorderInit(void);
uint8_t WaveRecorderStart(uint16_t* pbuf);
uint32_t WaveRecorderStop(void);
void Delay(__IO uint32_t nTime);
void WaveRecorderUpdate(void);
extern void (*sample_collected_fp)(int16_t * p_data);


#endif /* __WAVE_RECORDER_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

