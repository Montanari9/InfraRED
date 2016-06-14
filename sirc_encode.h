/**
  ******************************************************************************
  * @file    sirc_encode.h
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    29-May-2012
  * @brief   This file contains all the functions prototypes for the sirc encode 
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
#ifndef __SIRC_ENCODE_H
#define __SIRC_ENCODE_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
   
/** @addtogroup STM320518_EVAL_Demo
  * @{
  */

/** @addtogroup SIRC_ENCODE
  * @{
  */


/** @defgroup SIRC_ENCODE_Exported_Types
  * @{
  */

/* to avoid collusion, this value can be updated for thin adjust*/
#define  SIRC12_COLLUSION_AVOID_AJUST   ((uint8_t)0)

/**
  * @brief Definition of the SIRC12 devices adresses.
  */
   
typedef enum
   {
     SIRC12_Address_0          = ((uint8_t)0)
   }SIRC12_Address_TypeDef;
   
/**
  * @brief Definition of the SIRC12 devices instructions.
  */        
typedef enum
   {
     /* Device general Instruction Code*/
     SIRC12_Instruction_0      = ((uint8_t)0)
   }SIRC12_Instruction_TypeDef;
   
/**
  * @}
  */

/** @defgroup SIRC_ENCODE_Exported_Constants
  * @{
  */

/* Asserts Definition */

#define  SIRC12_ADDRESS_LIMIT        ((uint8_t ) 0x1F)   /* Address limit definition*/
#define  SIRC12_INSTRUCTION_LIMIT    ((uint8_t ) 0x7F)   /* Data limit definition*/

#define IS_SIRC12_ADDRESS_IN_RANGE(ADDRESS) ((ADDRESS) <= SIRC12_ADDRESS_LIMIT )
#define IS_SIRC12_INSTRUCTION_IN_RANGE(INSTRUCTION) ((INSTRUCTION) <= SIRC12_INSTRUCTION_LIMIT   )

/**
  * @}
  */

/** @defgroup SIRC_ENCODE_Exported_Macros
  * @{
  */

/**
  * @}
  */

   
   
   
/** @addtogroup SIRC_Exported_Functions
  * @{
  */
void Menu_SIRC12_Encode_Func(void);
void SIRC12_Encode_Init(void);
void SIRC12_Encode_SendFrame(uint8_t SIRC12_Address, uint8_t SIRC12_Instruction);
void SIRC12_Encode_SignalGenerate(uint32_t SIRC12_FramePulseWidthFormat[]);


#ifdef __cplusplus
}
#endif

#endif  /*__SIRC_ENCODE_H */
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
