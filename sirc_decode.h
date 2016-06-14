/**
  ******************************************************************************
  * @file    sirc_decode.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    29-May-2012
  * @brief   This file contains all the functions prototypes for the SIRC 
  *          firmware library.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SIRC_DECODE_H
#define __SIRC_DECODE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/** @addtogroup STM320518_EVAL_Demo
  * @{
  */

/** @addtogroup SIRC_DECODE
  * @{
  */

/** @defgroup SIRC_DECODE_Exported_Types
  * @{
  */

/** 
  * @brief  SIRC frame structure  
  */
typedef struct
{  
  __IO uint8_t Command;   /*!< Command field */
  __IO uint8_t Address ;  /*!< Address field */
} SIRC_Frame_TypeDef;

/** 
  * @brief  SIRC packet structure  
  */
typedef struct
{
  uint8_t count;  /*!< Bit count */
  uint8_t status; /*!< Status */
  uint32_t data;  /*!< Data */
} tSIRC_packet;

/**
  * @}
  */

/** @defgroup SIRC_DECODE_Exported_Constants
  * @{
  */


#define SIRC_MESSAGE                    " SIRC InfraRed Demo  "

#define SIRC_TIME_OUT_US                4050 

#define SIRC_STATUS_HEADER              1 << 1
#define SIRC_STATUS_RX                  1 << 0
#define INITIAL_STATUS                SIRC_STATUS_HEADER 

#define SIRC_BIT_ERROR                  0xFF
#define SIRC_HEADER_ERROR               0xFF
#define SIRC_HEADER_OK                  0x00

#define SIRC_BITS_COUNT                 11
#define SIRC_TOTAL_BITS_COUNT           11

#define SIRC_ONTIME_MIN_US              (600 - 60)
#define SIRC_ONTIME_MAX_US              (1200 + 60)

#define SIRC_HEADER_LOW_MIN_US          (2400 - 150)
#define SIRC_HEADER_LOW_MAX_US          (2400 + 150)
#define SIRC_HEADER_WHOLE_MIN_US        (2400 + 600 - 60)
#define SIRC_HEADER_WHOLE_MAX_US        (2400 + 600 + 60)

#define SIRC_VALUE_STEP_US              600
#define SIRC_VALUE_MARGIN_US            100
#define SIRC_VALUE_00_US                1200

/**
  * @}
  */
  
/** @defgroup SIRC_DECODE_Exported_Macros
  * @{
  */
/**
  * @}
  */
/** @defgroup SIRC_DECODE_Exported_Functions
  * @{
  */
void Menu_SIRCDecode_Func(void);
void SIRC_DeInit(void);
void SIRC_Init(void);
void SIRC_Decode(SIRC_Frame_TypeDef *ir_frame);
void SIRC_ResetPacket(void);
void SIRC_DataSampling(uint32_t lowPulseLength, uint32_t wholePulseLength);

#ifdef __cplusplus
}
#endif

#endif /* __SIRC_DECODE_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
