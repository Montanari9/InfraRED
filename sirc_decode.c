/**
  ******************************************************************************
  * @file    sirc_decode.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    29-May-2012
  * @brief   This file provides all the sony sirc decode firmware functions.
  *    
  * 1. How to use this driver
  * -------------------------         
  *      - TIM2 Capture Compare and Update interrupts are used to decode the IR
  *        frame, if a frame is received correctly a global variable IRFrameReceived 
  *        will be set to inform the application.
  *          
  *      - Then the application should call the function SIRC_Decode() to retrieve 
  *        the received IR frame. 
  *        
  *      - You can easily tailor this driver to any other InfraRed protocol, 
  *        by just adapting the defines from sirc_decode.h to the InfraRed 
  *        protocol specification(Bit Duration, Header Duration, Marge Tolerance, 
  *        Number of bits...).         
  *                   
  * 2. Important to know
  * --------------------  
  *      - TIM2_IRQHandler ISRs are coded within this driver.
  *        If you are using one or both Interrupts in your application you have 
  *        to proceed carefully:   
  *           - either add your application code in these ISRs
  *           - or copy the contents of these ISRs in you application code
  *                              
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM320518_EVAL_Demo
  * @{
  */

/** @addtogroup SIRC_DECODE
  * @brief SIRC_DECODE driver modules
  * @{
  */

/** @defgroup SIRC_DECODE_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup SIRC_DECODE_Private_Defines
  * @{
  */

/**
  * @}  
  */


/** @defgroup SIRC_DECODE_Private_Macros
  * @{
  */
/* Table of SIRC address */
const char  * SIRC_devices[32] = {
         "        SAT         ",  /* 0 */
         "         TV         ",  /* 1 */
         "        VTR1        ",  /* 2 */
         "      Teletext      ",  /* 3 */
         "     Widescreen     ",  /* 4 */
         "      Reserved      ",  /* 5 */
         "     laser Disk     ",  /* 6 */
         "        VTR2        ",  /* 7 */
         "      Reserved      ",  /* 8 */
         "      Reserved      ",  /* 9 */
         "      Reserved      ",  /* 10 */
         "        VTR3        ",  /* 11 */
         "Surround Sound Proc ",  /* 12 */
         "      Reserved      ",  /* 13 */
         "      Reserved      ",  /* 14 */
         "      Reserved      ",  /* 15 */
         "   Audio/Cassette   ",  /* 16 */
         "         CD         ",  /* 17 */
         "     Equalizer      ",  /* 18 */
         "      Reserved      ",  /* 19 */
         "      Reserved      ",  /* 20 */
         "      Reserved      ",  /* 21 */
         "      Reserved      ",  /* 22 */
         "      Reserved      ",  /* 23 */
         "      Reserved      ",  /* 24 */
         "      Reserved      ",  /* 25 */
         "     DVD Player     ",  /* 26 */
         "      Reserved      ",  /* 27 */
         "      Reserved      ",  /* 28 */
         "      Reserved      ",  /* 29 */
         "      Reserved      ",  /* 30 */
         "      Reserved      ",  /* 31 */
};

/* Table of SIRC commands */
const char  * SIRC_Commands[128] = {
    "        Num1        ",   /* 0 */
    "        Num2        ",   /* 1 */
    "        Num3        ",   /* 2 */
    "        Num4        ",   /* 3 */
    "        Num5        ",   /* 4 */
    "        Num6        ",   /* 5 */
    "        Num7        ",   /* 6 */
    "        Num8        ",   /* 7 */
    "        Num9        ",   /* 8 */
    "        Num0        ",   /* 9 */
    "       Reserved     ",   /* 10 */
    "        Enter       ",   /* 11 */
    "      Reserved      ",   /* 12 */
    "      Reserved      ",   /* 13 */
    "      Reserved      ",   /* 14 */
    "      Reserved      ",   /* 15 */
    "     Channel Up     ",   /* 16 */
    "     Channel Down   ",   /* 17 */
    "       Volume Up    ",   /* 18 */
    "      Volume Down   ",   /* 19 */
    "         Mute       ",   /* 20 */
    "        Power       ",   /* 21 */
    "        Reset       ",   /* 22 */
    "      Audio Mode    ",   /* 23 */
    "      Picture Up    ",   /* 24 */
    "     Picture Down   ",   /* 25 */
    "     Colour Up      ",   /* 26 */
    "     Colour Down    ",   /* 27 */
    "      Reserved      ",   /* 28 */
    "      Reserved      ",   /* 29 */
    "    Brightness Up   ",   /* 30 */
    "   Brightness Down  ",   /* 31 */
    "       Hue Up       ",   /* 32 */
    "      Hue Down      ",   /* 33 */
    "    Sharpness Up    ",   /* 34 */
    "   Sharpness Down   ",   /* 35 */
    "   Select TV Tuner  ",   /* 36 */
    "   Balance Left     ",   /* 37 */
    "   Balance Right    ",   /* 38 */
    "      Reserved      ",   /* 39 */
    "      Reserved      ",   /* 40 */
    "    Surround on/off ",   /* 41 */
    "      Reserved      ",   /* 42 */
    "      Reserved      ",   /* 43 */
    "      Reserved      ",   /* 44 */
    "      Reserved      ",   /* 45 */
    "      Reserved      ",   /* 46 */
    "     Power off      ",   /* 47 */
    "     Time Display   ",   /* 48 */
    "      Reserved      ",   /* 49 */
    "      Reserved      ",   /* 50 */
    "      Reserved      ",   /* 51 */
    "      Reserved      ",   /* 52 */
    "      Reserved      ",   /* 53 */
    "     Sleep Timer    ",   /* 54 */
    "      Reserved      ",   /* 55 */
    "      Reserved      ",   /* 56 */
    "      Reserved      ",   /* 57 */
    "   Channel Display  ",   /* 58 */
    "    Channel Jump    ",   /* 59 */
    "      Reserved      ",   /* 60 */
    "      Reserved      ",   /* 61 */
    "      Reserved      ",   /* 62 */
    "      Reserved      ",   /* 63 */
    " Select Input video1",   /* 64 */
    " Select Input video2",   /* 65 */
    " Select Input video3",   /* 66 */
    "      Reserved      ",   /* 67 */
    "      Reserved      ",   /* 68 */
    "      Reserved      ",   /* 69 */
    "      Reserved      ",   /* 70 */
    "      Reserved      ",   /* 71 */
    "      Reserved      ",   /* 72 */
    "      Reserved      ",   /* 73 */
    "noiseReductionon/off",   /* 74 */
    "      Reserved      ",   /* 75 */
    "      Reserved      ",   /* 76 */
    "      Reserved      ",   /* 77 */
    "  Cable/Broadcast   ",   /* 78 */
    " Notch Filter on/off",   /* 79 */
    "      Reserved      ",   /* 80 */
    "      Reserved      ",   /* 81 */
    "      Reserved      ",   /* 82 */
    "      Reserved      ",   /* 83 */
    "      Reserved      ",   /* 84 */
    "      Reserved      ",   /* 85 */
    "      Reserved      ",   /* 86 */
    "      Reserved      ",   /* 87 */
    "   PIP Channel Up   ",   /* 88 */
    "   PIP Channel Down ",   /* 89 */
    "      Reserved      ",   /* 90 */
    "      PIP on        ",   /* 91 */
    "    Freeze Screen   ",   /* 92 */
    "      Reserved      ",   /* 93 */
    "     PIP Position   ",   /* 94 */
    "      PIP Swap      ",   /* 95 */
    "        Guide       ",   /* 96 */
    "     Video Setup    ",   /* 97 */
    "     Audio Setup    ",   /* 98 */
    "     Exit Setup     ",   /* 99 */
    "      Reserved      ",   /* 100 */
    "      Reserved      ",   /* 101 */
    "      Reserved      ",   /* 102 */
    "      Reserved      ",   /* 103 */
    "      Reserved      ",   /* 104 */
    "      Reserved      ",   /* 105 */
    "      Reserved      ",   /* 106 */
    "    Auto Program    ",   /* 107 */
    "      Reserved      ",   /* 108 */
    "      Reserved      ",   /* 109 */
    "      Reserved      ",   /* 110 */
    "       PIP off      ",   /* 111 */
    "       Treble Up    ",   /* 112 */
    "      Treble Down   ",   /* 113 */
    "       Bass Up      ",   /* 114 */
    "      Bass Down     ",   /* 115 */
    "       + Key        ",   /* 116 */
    "       - Key        ",   /* 117 */
    "      Reserved      ",   /* 118 */
    "    Slipt Screen    ",   /* 119 */
    "    Add Channel     ",   /* 120 */
    "   Delete Channel   ",   /* 121 */
    "      Reserved      ",   /* 122 */
    "      Reserved      ",   /* 123 */
    "      Reserved      ",   /* 124 */
    "  Trinitone on/off  ",   /* 125 */
    "      Reserved      ",   /* 126 */
    "Display a red Rtests"    /* 127 */
};
/**
  * @}
  */

/** @defgroup SIRC_DECODE_Private_Variables
  * @{
  */

__IO StatusYesOrNo IRFrameReceived = NO;   /*!< IR frame status */
  
/* IR bit definitions*/
uint16_t IROnTimeMin = 0; 
uint16_t IROnTimeMax = 0; 
uint16_t IRValueStep = 0;
uint16_t IRValueMargin = 0;
uint16_t IRValue00 = 0;

/* Header time definitions*/
uint16_t IRHeaderLowMin = 0;
uint16_t IRHeaderLowMax = 0;
uint16_t IRHeaderWholeMin = 0;
uint16_t IRHeaderWholeMax = 0;
tSIRC_packet IRTmpPacket; /*!< IR packet*/

uint16_t IRTimeOut = 0;
static uint32_t TIMCLKValueKHz = 0;/*!< Timer clock */

SIRC_Frame_TypeDef SIRC_FRAME;
extern __IO uint8_t RFDemoStatus;
/**
  * @}
  */

/** @defgroup SIRC_DECODE_Private_FunctionPrototypes
  * @{
  */
 
static uint8_t SIRC_DecodeHeader(uint32_t lowPulseLength, uint32_t wholePulseLength);
static uint8_t SIRC_DecodeBit(uint32_t lowPulseLength, uint32_t wholePulseLength);
static uint32_t TIM_GetCounterCLKValue(void);
static uint32_t reversebit(uint32_t data);
 
/**
  * @}
  */

/** @defgroup SIRC_DECODE_Private_Functions
  * @{
  */
/**
  * @brief  SIRC receiver demo exec.
  * @param  None
  * @retval None
  */
void Menu_SIRCDecode_Func(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  
  TIM_Cmd(TIM15,DISABLE);
  
  /* Disable the JoyStick interrupt */
  Demo_IntExtOnOffCmd(DISABLE);
  
  while (Menu_ReadKey() != NOKEY)
  {}
  /* Clear the LCD */ 
  LCD_Clear(LCD_COLOR_WHITE);
  
  /* Display Image */
  LCD_SetDisplayWindow(120, 192, 64, 64);
  Storage_OpenReadFile(120, 192, "STFILES/TV.BMP");  
  LCD_WindowModeDisable();
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LCD_LINE_9, "  To exit press UP  ");
  
  /* Initialize the InfraRed application: SIRC */
  RFDemoStatus = SIRCDEMO;
  SIRC_Init();
  
  while(Menu_ReadKey() != UP)
  { 
    /* SIRC frame decode */
    SIRC_Decode(&SIRC_FRAME);
  }
  
  LCD_Clear(LCD_COLOR_WHITE);
  
  TIM_DeInit(TIM2);
  
  /* Time Base configuration 100ms */
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  TIM_TimeBaseStructure.TIM_Prescaler = 1000;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 0x0C80;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
  
  /* Channel 1, 2, 3 and 4 Configuration in Timing mode */
  TIM_OCStructInit(&TIM_OCInitStructure);
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0x0;
  
  TIM_OC1Init(TIM2, &TIM_OCInitStructure);
  
  Demo_LedShow(ENABLE);
  
  /* Exit the SIRC demo */
  RFDemoStatus = 0;
  
  /* Enable the JoyStick interrupt */
  Demo_IntExtOnOffCmd(ENABLE);
  
  /* Display menu */
  Menu_DisplayMenu();
}

/**
  * @brief  De-initializes the peripherals (RCC,GPIO, TIM)       
  * @param  None
  * @retval None
  */
void SIRC_DeInit(void)
{ 
  TIM_DeInit(IR_TIM);
  GPIO_DeInit(IR_GPIO_PORT);
}

/**
  * @brief  Initialize the IR bit time range.
  * @param  None
  * @retval None
  */
void SIRC_Init(void)
{ 
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ICInitTypeDef TIM_ICInitStructure;
  
  /*  Clock Configuration for TIMER */
  RCC_APB1PeriphClockCmd(IR_TIM_CLK, ENABLE);

  /* Enable Button GPIO clock */
  RCC_AHBPeriphClockCmd(IR_GPIO_PORT_CLK, ENABLE);
  
  /* Pin configuration: input floating */
  GPIO_InitStructure.GPIO_Pin = IR_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(IR_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(IR_GPIO_PORT, IR_GPIO_SOURCE, GPIO_AF_2);
     
  /* Enable the TIM global Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = IR_TIM_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* TIMER frequency input */
  TIM_PrescalerConfig(IR_TIM, TIM_PRESCALER, TIM_PSCReloadMode_Immediate);

  /* TIM configuration */
  TIM_ICInitStructure.TIM_Channel = IR_TIM_Channel;
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
  TIM_ICInitStructure.TIM_ICFilter = 0x0;
  TIM_PWMIConfig(IR_TIM, &TIM_ICInitStructure); 

  /* Timer Clock */
  TIMCLKValueKHz = TIM_GetCounterCLKValue()/1000; 

  /* Select the TIM Input Trigger: TI2FP2 */
  TIM_SelectInputTrigger(IR_TIM, TIM_TS_TI2FP2);

  /* Select the slave Mode: Reset Mode */
  TIM_SelectSlaveMode(IR_TIM, TIM_SlaveMode_Reset);

  /* Enable the Master/Slave Mode */
  TIM_SelectMasterSlaveMode(IR_TIM, TIM_MasterSlaveMode_Enable);

  /* Configures the TIM Update Request Interrupt source: counter overflow */
  TIM_UpdateRequestConfig(IR_TIM,  TIM_UpdateSource_Regular);
   
  IRTimeOut = TIMCLKValueKHz * SIRC_TIME_OUT_US/1000;

  /* Set the TIM auto-reload register for each IR protocol */
  IR_TIM->ARR = IRTimeOut;
  
  /* Clear update flag */
  TIM_ClearFlag(IR_TIM, TIM_FLAG_Update);

  /* Enable TIM Update Event Interrupt Request */
  TIM_ITConfig(IR_TIM, TIM_IT_Update, ENABLE);

  /* Enable the CC2/CC1 Interrupt Request */
  TIM_ITConfig(IR_TIM, TIM_IT_CC2, ENABLE);
  TIM_ITConfig(IR_TIM, TIM_IT_CC1, ENABLE);

  /* Enable the timer */
  TIM_Cmd(IR_TIM, ENABLE);
  
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_RED);

  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_GREEN);
  LCD_DisplayStringLine(LCD_LINE_0, "   STM320518-EVAL   ");
  LCD_DisplayStringLine(LCD_LINE_1, SIRC_MESSAGE);
  
  LCD_SetBackColor(LCD_COLOR_BLUE);
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);

  /* Header */	
  IRHeaderLowMin = TIMCLKValueKHz * SIRC_HEADER_LOW_MIN_US/1000;
  IRHeaderLowMax = TIMCLKValueKHz * SIRC_HEADER_LOW_MAX_US/1000;
  IRHeaderWholeMin = TIMCLKValueKHz * SIRC_HEADER_WHOLE_MIN_US/1000;
  IRHeaderWholeMax = TIMCLKValueKHz * SIRC_HEADER_WHOLE_MAX_US/1000;

  /* Bit time range*/
  IROnTimeMax = TIMCLKValueKHz * SIRC_ONTIME_MAX_US /1000;
  IROnTimeMin = TIMCLKValueKHz * SIRC_ONTIME_MIN_US/1000; 
  IRValueStep = TIMCLKValueKHz * SIRC_VALUE_STEP_US/1000;
  IRValueMargin = TIMCLKValueKHz * SIRC_VALUE_MARGIN_US/1000;
  IRValue00 = TIMCLKValueKHz * SIRC_VALUE_00_US/1000;

  /* Default state */
  SIRC_ResetPacket();
}

/**
  * @brief  Decode the IR frame (ADDRESS, COMMAND) when all the frame is 
  *         received, the IRFrameReceived will equal to YES.
  *         The IRFrameReceived is set to YES inside the  SIRC_DataSampling() 
  *         function when the whole IR frame is received without any error.
  *         If any received bit timing is out of the defined range, the IR packet
  *         is reset and the IRFrameReceived is set to NO.
  *         After the IR received display, the IRFrameReceived is set NO.
  *         User can check the IRFrameReceived variable status to verify if there
  *         is a new IR frame already received.          
  * @param  ir_frame: pointer to SIRC_Frame_TypeDef structure that contains the 
  *         the IR protocol fields (Address, Command,...).
  * @retval None
  */  
void SIRC_Decode(SIRC_Frame_TypeDef *ir_frame)
{
  if(IRFrameReceived != NO) 
  {
    IRTmpPacket.data = reversebit(IRTmpPacket.data);

    ir_frame->Command = (IRTmpPacket.data >> 20)& 0x7F;
    ir_frame->Address = (IRTmpPacket.data >> 27) & 0x1F;

    /* Display command and address */
    LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[ir_frame->Command]);
    LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[ir_frame->Address]);

    /* Default state */
    IRFrameReceived = NO; 
    SIRC_ResetPacket();

  }
}

/**
  * @brief  Put the packet to default state, ready to be filled in by incoming 
  *         data.
  * @param  None
  * @retval None
  */
void SIRC_ResetPacket(void)
{
  IRTmpPacket.count = 0;
  IRTmpPacket.status = INITIAL_STATUS;
  IRTmpPacket.data = 0;
}

/**
  * @brief  Identify the IR data.
  * @param  lowPulseLength: low pulse duration. 
  * @param  wholePulseLength: whole pulse duration.
  * @retval None
  */
void SIRC_DataSampling(uint32_t lowPulseLength, uint32_t wholePulseLength)
{
  uint8_t  tmpBit = 0;
  
  /* If the pulse timing is correct */
  if ((IRTmpPacket.status == SIRC_STATUS_RX)) 
  {
    /* Convert raw pulse timing to data value */
    tmpBit = SIRC_DecodeBit(lowPulseLength, wholePulseLength); 
    if (tmpBit != SIRC_BIT_ERROR) /* If the pulse timing is correct */
    {
      /* This operation fills in the incoming bit to the correct position in
      32 bit word*/
      IRTmpPacket.data |= tmpBit;
      IRTmpPacket.data <<= 1;
      /* Increment the bit count  */
      IRTmpPacket.count++;
    }
    /* If all bits identified */
    if (IRTmpPacket.count == SIRC_TOTAL_BITS_COUNT)
    {
      /* Frame received*/
      IRFrameReceived = YES;
    }
    /* Bit 15:the idle time between IR message and the inverted one */
    else if (IRTmpPacket.count == SIRC_BITS_COUNT)
    {
      IRTmpPacket.status = SIRC_STATUS_HEADER; 
    }
  }
  else if ((IRTmpPacket.status == SIRC_STATUS_HEADER))
  {
    /* Convert raw pulse timing to data value */
    tmpBit = SIRC_DecodeHeader(lowPulseLength, wholePulseLength);
    
    /* If the Header timing is correct */
    if ( tmpBit!= SIRC_HEADER_ERROR)
    { 
      /* Reception Status for the inverted message */
      IRTmpPacket.status = SIRC_STATUS_RX; 
    }
    else
    {
      /* Wrong header time */
      SIRC_ResetPacket();  
    }
  }
}

/**
  * @brief  Check the header pulse if it has correct pulse time.
  * @param  lowPulseLength: low pulse header duration.
  * @param  wholePulseLength: whole pulse header duration.
  * @retval SIRC_HEADER_OK or SIRC_HEADER_ERROR
  */
static uint8_t SIRC_DecodeHeader(uint32_t lowPulseLength, uint32_t wholePulseLength)
{
  /* First check if low pulse time is according to the specs */
  if ((lowPulseLength >= IRHeaderLowMin) && (lowPulseLength <= IRHeaderLowMax)) 
  {
    /* Check if the high pulse is OK */
    if ((wholePulseLength >= IRHeaderWholeMin) && (wholePulseLength <= IRHeaderWholeMax))
    {
      return SIRC_HEADER_OK; /* Valid Header */
    }
  }	
  return SIRC_HEADER_ERROR;  /* Wrong Header */
}

/**
  * @brief  Convert raw time to data value.
  * @param  lowPulseLength: low pulse bit duration.
  * @param  wholePulseLength: whole pulse bit duration.
  * @retval BitValue( data0 or data1) or SIRC_BIT_ERROR
  */
static uint8_t SIRC_DecodeBit(uint32_t lowPulseLength , uint32_t wholePulseLength)
{
  uint8_t i = 0;
  
  /* First check if low pulse time is according to the specs */
  if ((lowPulseLength >= IROnTimeMin) && (lowPulseLength <= IROnTimeMax))
  {
    for (i = 0; i < 2; i++) /* There are data0 to data1 */
    {
      /* Check if the length is in given range */
        if ((wholePulseLength >= (uint32_t)(IRValue00 + (IRValueStep * i) - IRValueMargin)) 
            && (wholePulseLength <=  (uint32_t)(IRValue00 + (IRValueStep * i) + IRValueMargin)))
        return i; /* Return bit value */
    }
  }
  return SIRC_BIT_ERROR; /* No correct pulse length was found */
}

/**
  * @brief  Identify TIM clock
  * @param  None
  * @retval Timer clock
  */
static uint32_t TIM_GetCounterCLKValue(void)
{
  uint32_t apbprescaler = 0, apbfrequency = 0;
  uint32_t timprescaler = 0;
  __IO RCC_ClocksTypeDef RCC_ClockFreq;
  
  /* This function fills the RCC_ClockFreq structure with the current
  frequencies of different on chip clocks */
  RCC_GetClocksFreq((RCC_ClocksTypeDef*)&RCC_ClockFreq);
  
   /* Get the clock prescaler of APB1 */
    apbprescaler = ((RCC->CFGR >> 8) & 0x7);
    apbfrequency = RCC_ClockFreq.PCLK_Frequency; 
    timprescaler = TIM_PRESCALER;

  
  /* If APBx clock div >= 4 */
  if (apbprescaler >= 4)
  {
    return ((apbfrequency * 2)/(timprescaler + 1));
  }
  else
  {
    return (apbfrequency/(timprescaler+ 1));
  }
}
/**
  * @brief  Reverse bit       
  * @param  data: data to be inversed
  * @retval None
  */
static uint32_t reversebit(uint32_t data) 
{
  uint32_t i = 0;
  uint32_t j = 0;
  
  for(j = i = 0; i < 32; i++) 
  {
    j = (j << 1) + (data & 1); 
    data >>= 1; 
  }
  
  return j;
}
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
