/**
  ******************************************************************************
  * @file    sirc_encode.c
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    29-May-2012
  * @brief   This file provides all the sony sirc encode firmware functions
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

/** @addtogroup SIRC_ENCODE
  * @brief SIRC_ENCODE driver modules
  * @{
  */

/** @defgroup SIRC_ENCODE_Private_TypesDefinitions
  * @{
  */

/**
  * @}
  */

/** @defgroup SIRC_ENCODE_Private_Defines
  * @{
  */
#define  SIRC12_HIGH_STATE_NB_SYMBOL     ((uint8_t )3)       /* Nb high state symbol definition*/
#define  SIRC12_LOW_STATE_NB_SYMBOL      ((uint8_t )2)       /* Nb low state symbol definition*/
#define  SIRC12_ADDRESS_BITS             ((uint8_t )5)       /* Nb of data bits definition*/
#define  SIRC12_INSTRUCTION_BITS         ((uint8_t )7)       /* Nb of data bits definition*/
#define  SIRC12_HIGH_STATE_CODE          ((uint8_t )0x03)    /* SIRC12 high level definition*/
#define  SIRC12_LOW_STATE_CODE           ((uint8_t )0x01)    /* SIRC12 low level definition*/
#define  SIRC12_MAIN_FRAME_LENGTH        ((uint8_t )12)      /* Main frame length*/
#define  SIRC12_BIT_FORMAT_MASK          ((uint16_t)1)       /* Bit mask definition*/
#define  SIRC12_COUNTER_LIMIT            ((uint16_t)75)      /* main frame + idle time */
#define  SIRC12_IS_RUNNING               ((uint8_t)4)        /* SIRC12 Protocol number */
#define  SIRC12_HEADERS                  ((uint16_t)0x0F)    /* SIRC12 Start pulse */
#define  SIRC12_HEADERS_LENGTH           ((uint8_t)5)        /* Length of the headers */
#define  SIRC12_CODED_FRAME_TABLE_LENGTH ((uint8_t)2)        /* Coded frame table number of uint32_t word  */

#define MESSAGE1  "LEFT | RIGHT| DOWN  | SEL  "
#define MESSAGE2  "PREV | NEXT |SWITCH | SEND "
/**
  * @}  
  */


/** @defgroup SIRC_ENCODE_Private_Macros
  * @{
  */
/**
  * @}  
  */

/** @defgroup SIRC_ENCODE_Private_Variables
  * @{
  */
extern uint8_t AddressIndex;
extern uint8_t InstructionIndex;
extern char* SIRC_devices[];
extern char* SIRC_Commands[];
extern __IO uint8_t RFDemoStatus; 
uint32_t SIRC12_FramePulseWidthFormat[SIRC12_CODED_FRAME_TABLE_LENGTH];
/* Internal Flags   ----------------------------------------------------------*/   
uint8_t SIRC12_Send_Operation_Ready_f = RESET;
uint8_t SIRC12_Send_Operation_Completed_f = SET;  

uint16_t SIRC12_Framebinaryformat = 0;
uint8_t  SIRC12_BitsSent_Counter = 0; 
uint8_t  SIRC12_CodedFrameLastWordLength = 0;  
uint8_t  SIRC12_CodedInstructionLength = 0; 
uint8_t  SIRC12_CodedAddressLength = 0; 
uint8_t  SIRC12_Nb_Word = 0;
uint8_t PrescalerValue = 0; 
/**
  * @}
  */

/** @defgroup SIRC_ENCODE_Private_FunctionPrototypes
  * @{
  */
static void SIRC12_PulseWidthModulationConvert(uint32_t  bindata, uint8_t bindatalength);
static void SIRC12_Shift_Table(uint32_t Table[]);
static void SIRC12_AddHeaders(uint8_t headers);
static void SIRC12_AddStateFragment(uint8_t STATE, uint8_t freespace);
static void SIRC12_AddHeadersFragment(uint8_t headers, uint8_t freespace);
static uint32_t SIRC12_MSBToLSB_Data(uint32_t Data, uint8_t DataNbBits);
static uint16_t SIRC12_BinFrameGeneration(uint8_t SIRC12_Address, uint8_t SIRC12_Instruction);
/**
  * @}
  */

/** @defgroup SIRC_ENCODE_Private_Functions
  * @{
  */

/**
  * @brief  RCR receiver demo exec.
  * @param  None
  * @retval None
  */
void Menu_SIRC12_Encode_Func(void)
{
  uint8_t pressedkey = 0, index = 0;
  
  /* Disable the JoyStick interrupt */
  Demo_IntExtOnOffCmd(DISABLE);
  
  while (Menu_ReadKey() != NOKEY)
  {}
  /* Clear the LCD */ 
  LCD_Clear(LCD_COLOR_WHITE);   
  
  /* Display Image */
  LCD_SetDisplayWindow(120, 192, 64, 64);
  Storage_OpenReadFile(120, 192, "STFILES/Icon11.BMP");  
  LCD_WindowModeDisable();
  
  LCD_SetFont(&Font12x12);
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_CYAN);
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_BLACK); 
  LCD_DisplayStringLine(LINE(18), MESSAGE1);
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_BLUE);
  LCD_SetTextColor(LCD_COLOR_WHITE);
  LCD_DisplayStringLine(LINE(19), MESSAGE2);
  LCD_SetFont(&Font16x24);
  
  SIRC12_Encode_Init();
 
  AddressIndex = 0;
  InstructionIndex = 0;
  RFDemoStatus = SIRCDEMO;

  pressedkey = Menu_ReadKey();

  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_RED);
  /* Display the device address message */
  LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[InstructionIndex]);
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);
  /* Display the device address message */
  LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[AddressIndex]);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_RED);
  while(pressedkey != UP)
  { 
    pressedkey = Menu_ReadKey();
    /* To switch between device address and command */
    if (pressedkey == DOWN)
    {
      /* Set the LCD Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);

      if (index == 0)
      { 
        index = 1;
        /* Display the device address message */
        LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[InstructionIndex]);
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        /* Display the device address message */
        LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[AddressIndex]);
      }
      else
      {
        index = 0;
        /* Display the device address message */
        LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[AddressIndex]);
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
          /* Display the device address message */
        LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[InstructionIndex]);
      }
    }
    if (index == 0)
    {
      /* Commands index decrement */
      if (pressedkey == LEFT)
      {
        if (InstructionIndex == 0)
        { 
          InstructionIndex = 127;
        }
        else
        {
          InstructionIndex--;
        }
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        /* Display the device address message */
        LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[InstructionIndex]);
      }
      /* Commands index increment */
      if (pressedkey == RIGHT)
      {
        if (InstructionIndex == 127)
        { 
          InstructionIndex = 0;
        }
        else
        {
          InstructionIndex++;
        }
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[InstructionIndex]);
      }
    }
    else
    {
      /* Decrement the address device index */
      if (pressedkey == LEFT)
      {
        if (AddressIndex == 0)
        { 
          AddressIndex = 31;
        }
        else
        {
          AddressIndex--;
        }
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        /* Display the device address message */
        LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[AddressIndex]);
      }
      /* Increment the address device index increment */
      if (pressedkey == RIGHT)
      {
        if (AddressIndex == 31)
        { 
          AddressIndex = 0;
        }
        else
        {
          AddressIndex++;
        }
        /* Set the LCD Text Color */
        LCD_SetTextColor(LCD_COLOR_RED);
        LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[AddressIndex]);
      }
    }
    if (pressedkey == SEL)
    {
      /* Set the LCD Text Color */
      LCD_SetTextColor(LCD_COLOR_WHITE);
      
      LCD_DisplayStringLine(LCD_LINE_6, SIRC_Commands[InstructionIndex]);
      LCD_DisplayStringLine(LCD_LINE_7, SIRC_devices[AddressIndex]);
      /* Button is pressed */
      SIRC12_Encode_SendFrame(AddressIndex, InstructionIndex);
    }
  }
  LCD_Clear(LCD_COLOR_WHITE);
  /* Enable the JoyStick interrupt */
  Demo_IntExtOnOffCmd(ENABLE);
  
  /* Display menu */
  Menu_DisplayMenu();
}

/**
  * @brief  Init Hardware (IPs used) for SIRC generation
  * @param  None
  * @retval None
  */
void SIRC12_Encode_Init(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* TIM16 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);
  
  /* TIM17 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM17, ENABLE);
  
  /* Pin configuration: input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_0);

  /* DeInit TIM17 */
  TIM_DeInit(TIM17);
  
  /* Elementary period 888us */
  /* Time base configuration for timer 2 */
  TIM_TimeBaseStructure.TIM_Period = 1200;
  TIM_TimeBaseStructure.TIM_Prescaler = 0x00;
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM17, &TIM_TimeBaseStructure);
  
  /* Prescaler configuration */
  TIM_PrescalerConfig(TIM17, 0, TIM_PSCReloadMode_Immediate);
  
  /* Output Compare Timing Mode configuration: Channel 1N */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 600; /* Set duty cycle to 50% to be compatible with RC5 specification */
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM17, &TIM_OCInitStructure);
  
  /* Timer17 preload enable */
  TIM_OC1PreloadConfig(TIM17, TIM_OCPreload_Enable);
  
  /* Timer 17 Enable */
  TIM_Cmd(TIM17, ENABLE);
  
  /* Enable the TIM16 channel1 output to be connected internly to the IRTIM */
  TIM_CtrlPWMOutputs(TIM17, ENABLE);
  
  /* DeInit TIM16 */
  TIM_DeInit(TIM16);

  /* Time Base = 36Khz */
  /* Time Base configuration for timer 16 */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = 28799;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);
  
  /* Duty Cycle = 25% */
  /* Channel 1 Configuration in Timing mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 28799;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
  TIM_OC1Init(TIM16, &TIM_OCInitStructure);
   
  /* Enable the TIM16 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* TIM16 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM16, ENABLE);
  
  /* TIM IT Disable */
  TIM_ITConfig(TIM16, TIM_IT_Update, DISABLE);
  
  /* TIM Disable */
  TIM_Cmd(TIM16, DISABLE);
   
  /* Set the LCD Back Color */
  LCD_SetBackColor(LCD_COLOR_RED);
  
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_GREEN);    
  LCD_DisplayStringLine(LCD_LINE_0, "   STM320518-EVAL   ");
  LCD_DisplayStringLine(LCD_LINE_1, " SIRC InfraRed Demo ");
  LCD_SetBackColor(LCD_COLOR_BLUE);
 
  /* Set the LCD Text Color */
  LCD_SetTextColor(LCD_COLOR_WHITE);  
}

/**
  * @brief  Generate and Send the SIRC12 frame.
  * @param  SIRC12_Address : the SIRC12 Device destination 
  * @param  SIRC12_Instruction : the SIRC12 command instruction 
  * @retval None
  */
void SIRC12_Encode_SendFrame(uint8_t SIRC12_Address, uint8_t SIRC12_Instruction)
{
  /* Check the parameters */
  assert_param(IS_SIRC12_ADDRESS_IN_RANGE(SIRC12_Address));
  assert_param(IS_SIRC12_INSTRUCTION_IN_RANGE(SIRC12_Instruction));
  
  /* Generate a binary format of the message */
  SIRC12_Framebinaryformat = SIRC12_BinFrameGeneration(SIRC12_Address,SIRC12_Instruction);
  
  /* Transform address and data from MSB first to LSB first */
  SIRC12_Framebinaryformat = SIRC12_MSBToLSB_Data(SIRC12_Framebinaryformat, SIRC12_MAIN_FRAME_LENGTH);
  
  /* Convert the frame binary format to a PulseWidthModulation format of the message */
  SIRC12_PulseWidthModulationConvert(SIRC12_Framebinaryformat, SIRC12_MAIN_FRAME_LENGTH);
  
  /* Add the headers to SIRC12_FramePulseWidthFormat Table */
  SIRC12_AddHeaders(SIRC12_HEADERS);
  
  /* Set the Send operation Ready flag to indicate that the frame is ready to be sent */
  SIRC12_Send_Operation_Ready_f = SET;
  
  /* TIM IT Enable */
  TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);
  /* Enable all Interrupt */
  TIM_Cmd(TIM16, ENABLE);
}
        
/**
  * @brief  Send by hardware PulseWidthModulation Format SIRC12 Frame.
  * @param  SIRC12_framemanchestarformat: the SIRC12 frame in PulseWidthModulation format.
  * @retval None
  */
void SIRC12_Encode_SignalGenerate(uint32_t formaat[])
{
  uint8_t bitmsg = 0; 
  uint8_t tablecounter =0;
  
  if((SIRC12_Send_Operation_Ready_f != RESET) && (SIRC12_BitsSent_Counter <= ( SIRC12_COUNTER_LIMIT )))
  {
    /*Reset send operation complete flag*/
    SIRC12_Send_Operation_Completed_f = RESET;
    
    /*Read message bits*/
    if (SIRC12_BitsSent_Counter < SIRC12_CodedFrameLastWordLength) 
    {
      /*Read coded frame bits from the last word*/
      bitmsg = (uint8_t)((formaat[0] >> SIRC12_BitsSent_Counter)& 1);
    }
    else
    {
      /*Read coded frame bits from the table*/
        bitmsg = (uint8_t)((formaat[((SIRC12_BitsSent_Counter-SIRC12_CodedFrameLastWordLength)/32)+1] >> (SIRC12_BitsSent_Counter-SIRC12_CodedFrameLastWordLength))& 1);
    }
    /* Generate signal */
    if (bitmsg == SET)
    {
      TIM_ForcedOC1Config(TIM16, TIM_ForcedAction_Active);
    }
    
    else if (bitmsg == RESET)
    { 
      TIM_ForcedOC1Config(TIM16, TIM_ForcedAction_InActive);
    }
    
    else if (SIRC12_BitsSent_Counter <= ( SIRC12_Nb_Word*32+ SIRC12_CodedFrameLastWordLength ))
    { 
      TIM_ForcedOC1Config(TIM16, TIM_ForcedAction_InActive);
    }
    
    SIRC12_BitsSent_Counter++;
  }   
  else
  {
    /* Reset flags   */
    SIRC12_Send_Operation_Completed_f = SET;
    SIRC12_Send_Operation_Ready_f = RESET;
    /* TIM IT Disable */
    TIM_ITConfig(TIM16, TIM_IT_Update, DISABLE);
    TIM_Cmd(TIM16, DISABLE);
    /*Reset counters  */
    SIRC12_Nb_Word = 0; 
    SIRC12_BitsSent_Counter = 0;
    /*Reset counters  */
    SIRC12_CodedFrameLastWordLength = 0;
    SIRC12_Framebinaryformat = 0; 
    
    /*Reset frame temporary variables*/
    for (tablecounter = 0; tablecounter<SIRC12_Nb_Word; tablecounter++)
    {
      formaat[tablecounter] = 0x0;
    }
    TIM_ForcedOC1Config(TIM16, TIM_ForcedAction_InActive);
  }
}
/**
  * @brief  Transform the frame binary form from MSB to LSB.
  * @param  Data: Frame binary format to inverse
  * @param  DataNbBits: number if bits to be inverted
  * @retval Symmetric binary frame form
  */
static uint32_t SIRC12_MSBToLSB_Data(uint32_t Data ,uint8_t DataNbBits)
{
  uint32_t temp = 0;       /* Temporary variable to memorize the converted message */
  uint8_t datacount = 0; /* Counter of bits converted */
  
  /* Shift the temporary variable to the left and add one bit from the Binary frame  */
  for (datacount = 0; datacount < (DataNbBits); datacount++)
  {
    temp=temp << 1;
    temp |= ((Data>>datacount)&1);
  }
  return temp;
}

/**
  * @brief  Generate the binary format of the SIRC12 frame.
  * @param  SIRC12_Address: Select the device address 
  * @param  SIRC12_Instruction: Select the device instruction 
  * @retval Binary format of the SIRC12 Frame.
  */
static uint16_t SIRC12_BinFrameGeneration(uint8_t SIRC12_Address, uint8_t SIRC12_Instruction)
{
  /* wait until the ongoing Frame sending operation finishes */
  while(SIRC12_Send_Operation_Completed_f == RESET) 
  { 
  }
  
  /* Reset SIRC12_Send_Operation_Ready_f flag to mention that a send operation can be treated */
  SIRC12_Send_Operation_Ready_f = RESET;  
  
  /* Concat Binary Frame Format */
  SIRC12_Framebinaryformat = (SIRC12_Address << SIRC12_INSTRUCTION_BITS );
  SIRC12_Framebinaryformat = SIRC12_Framebinaryformat | (SIRC12_Instruction);
  
  return (SIRC12_Framebinaryformat);
}

/**
  * @brief  Shift the coded frame table by one box.
  * @param  Table: coded data table
  * @retval None
  */
static void SIRC12_Shift_Table(uint32_t Table[])
{
  uint8_t i = 0;
  /* Increment the coded frame table  words number  */
 
  SIRC12_Nb_Word++;
  /* Shift the coded frame table to the left by one box */
  
  for (i = 0; i < SIRC12_Nb_Word; i++)
  {    
    Table[SIRC12_Nb_Word-i] = Table[SIRC12_Nb_Word - i - 1];
  }
  /* Clear the first the coded frame table box */
  Table[0] = 0;
  
  /* Reset the last word length counter  */
  SIRC12_CodedFrameLastWordLength = 0;
}

/**
  * @brief  Splits state codes in to two word of the table .
  * @param  State:the coded state to add to the coded frame table
  * @param  freespace: the last coded frame table word free space 
  * @retval None
  */
static void SIRC12_AddStateFragment(uint8_t State, uint8_t freespace)
{
  /* Shift the table to the left by one box */
  SIRC12_Shift_Table(SIRC12_FramePulseWidthFormat );
  
  /*Test if the message to add is a high state code */
  if(State == SIRC12_HIGH_STATE_CODE)
  {
    /* The message is a high state code  */
    /* Add the first frame fragment to the First word of the table*/
    SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] = SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] << freespace ;
    SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] |= SIRC12_HIGH_STATE_CODE >> (SIRC12_HIGH_STATE_NB_SYMBOL-freespace); 
    
    /* Add the Second frame fragment to the Second word of the table*/
    SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0] << (SIRC12_HIGH_STATE_NB_SYMBOL-freespace) ;
    SIRC12_FramePulseWidthFormat[0] |= ((SIRC12_HIGH_STATE_CODE << (8 - SIRC12_HIGH_STATE_NB_SYMBOL+freespace))&0xff)>>(8-SIRC12_HIGH_STATE_NB_SYMBOL+freespace);
    
    /* Increment the Last word of the coded frame counter*/
    SIRC12_CodedFrameLastWordLength = SIRC12_HIGH_STATE_NB_SYMBOL-freespace ; 
  }
  else 
  {
    /* The message is a low state code  */
    /* Add the first frame fragment to the First word of the table*/
    SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] = SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] << freespace ;
    SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] |= SIRC12_LOW_STATE_CODE >> (SIRC12_LOW_STATE_NB_SYMBOL-freespace); 
    
    /* Add the Second frame fragment to the Second word of the table*/
    SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0] << (SIRC12_LOW_STATE_NB_SYMBOL-freespace) ;
    SIRC12_FramePulseWidthFormat[0] |= ((SIRC12_LOW_STATE_CODE << (8-SIRC12_LOW_STATE_NB_SYMBOL+freespace))&0xff)>>(8-SIRC12_LOW_STATE_NB_SYMBOL+freespace);
    
    /* Increment the Last word of the coded frame counter*/
    SIRC12_CodedFrameLastWordLength = SIRC12_LOW_STATE_NB_SYMBOL-freespace ; 
  }
}

/**
  * @brief  Converts the SIRC12 message from binary to Pulse width modulation Format.
  * @param  SIRC12_BinaryMessageFormat:the SIRC12 message in binary format.
  * @retval None
  */
static void SIRC12_PulseWidthModulationConvert(uint32_t  bindata, uint8_t bindatalength)
{ 
  uint8_t  dataframecount = 0;
  uint16_t bitformat = 0;
  
  for (dataframecount=0; dataframecount < bindatalength; dataframecount++)
  {
    uint8_t lastwordfreeespace=0;
    
    /*Calculate last coded frame word free space*/
    lastwordfreeespace = 32-SIRC12_CodedFrameLastWordLength;
    
    /* Select one bit from the binary frame*/
    bitformat = ((((uint16_t)(bindata)) >> dataframecount) & SIRC12_BIT_FORMAT_MASK) << dataframecount;
    
    /* Test the bit format state*/
    if(bitformat != 0)
    {   
      /* Test if the last word of the frame enough space */
      if ((lastwordfreeespace) > (SIRC12_HIGH_STATE_NB_SYMBOL-1))
      {
        /*Shift left the the last coded frame word by State number of bits */
        SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0]<< SIRC12_HIGH_STATE_NB_SYMBOL ;
        
        /* Add the state to the last word of the coded frame table */
        SIRC12_FramePulseWidthFormat[0] |= SIRC12_HIGH_STATE_CODE;
        
        /*Increment the last coded frame word counter */
        SIRC12_CodedFrameLastWordLength = SIRC12_CodedFrameLastWordLength + SIRC12_HIGH_STATE_NB_SYMBOL ;
      }
      else 
      {
        /* Split state code to two words */ 
        SIRC12_AddStateFragment(SIRC12_HIGH_STATE_CODE, lastwordfreeespace);
      }
    }
    else
    {
      /*  bit format == 0 */  
      /*Test if the last word of the frame enough space*/
      if ((lastwordfreeespace) > 1)
      {
        /* enough space found*/ 
        /*Shift left the the last coded frame word by State bits number */
        SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0] << SIRC12_LOW_STATE_NB_SYMBOL ;
        
        /*Shift left the the last coded frame word by State bits number */
        SIRC12_FramePulseWidthFormat[0] |= SIRC12_LOW_STATE_CODE;
        
        /* Increment the Last word of the coded frame counter*/
        SIRC12_CodedFrameLastWordLength = SIRC12_CodedFrameLastWordLength + SIRC12_LOW_STATE_NB_SYMBOL ;
      }
      else 
      {
        /*Split state code to two words*/ 
        SIRC12_AddStateFragment(SIRC12_LOW_STATE_CODE, lastwordfreeespace);
      }
    }  
  } 
}

/**
  * @brief  Splits coded headers in to two word of the table .
  * @param  State:the coded headers to add to the coded frame table
  * @param  freespace: the last coded frame table word free space 
  *         This parameter can be any of the @ref uint8_t.
  * @retval None
  */
static void SIRC12_AddHeadersFragment(uint8_t headers, uint8_t freespace)
{
  /* Shift the table to the left by one box */
  SIRC12_Shift_Table(SIRC12_FramePulseWidthFormat);
  
  /*Shift left the the second coded frame word by headers second fragment bits number */
  SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] = SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word]<< freespace ;
  
  /* Add the first headers fragment to the Second word of the table*/
  SIRC12_FramePulseWidthFormat[SIRC12_Nb_Word] |= SIRC12_HEADERS>>(SIRC12_HEADERS_LENGTH -freespace) ;   
  
  /*Shift left the the last coded frame word by headers bits number */
  SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0]<< (SIRC12_HEADERS_LENGTH -freespace) ;
  
  /* Add the Second Headers fragment to the Second word of the table*/
  SIRC12_FramePulseWidthFormat[0] |= (((( SIRC12_HEADERS)<<(freespace+3)))&0xFF)>>((freespace+3));
  
  /* Increment the Last word of the coded frame counter*/
  SIRC12_CodedFrameLastWordLength=SIRC12_HEADERS_LENGTH - freespace; 
}

/**
  * @brief  Adds coded headers to the coded frame table.
  * @param  headers 
  * @retval None
  */
static void SIRC12_AddHeaders(uint8_t  Headers)
{
  uint8_t lastwordfreespace = 0;
  
  /*Calculate last coded frame word free space*/
  lastwordfreespace = 32 - SIRC12_CodedFrameLastWordLength;
  
  if(lastwordfreespace > 4)
  {
    /*Shift left the the last coded frame word by headers bits number */
    SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0] << 5;
    
    /* Add the first headers fragment to the First word of the table*/
    SIRC12_FramePulseWidthFormat[0] = SIRC12_FramePulseWidthFormat[0]| Headers;
    
    /* Increment the Last word of the coded frame counter*/
    SIRC12_CodedFrameLastWordLength = SIRC12_CodedFrameLastWordLength+SIRC12_HEADERS_LENGTH;
  } 
  else 
  {
    SIRC12_AddHeadersFragment(Headers, lastwordfreespace);
  } 
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

