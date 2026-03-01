/**
  ******************************************************************************
  * @file    BSP/Src/main.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the STM32H750B_DISCOVERY BSP Drivers
  *          This is the main program.   
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32H7xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void CPU_CACHE_Enable(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

#define GAME_DIMENSION 2

uint32_t xLength, yLength;
int segmentGameX, segmentGameY;

void ErrOutput() {
    UTIL_LCD_SetFont(&Font24);
    
    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKRED);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
    UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *) "Failed", CENTER_MODE);
}

void DrawGameSegmentBackground(int x, int y, uint32_t color) {
    int locationX = segmentGameX * x;
    int locationY = segmentGameY * y;

    BSP_LCD_FillRect(0, locationX, locationY, segmentGameX, segmentGameY, color);
}

void DrawGameSegmentForeground(int x, int y, int offsetX, int offsetY, uint32_t frontColor, uint32_t backColor, uint8_t* spring) {
    
    int locationX = segmentGameX * x;
    int locationY = segmentGameY * y;
  
    UTIL_LCD_SetFont(&Font16);
    UTIL_LCD_SetTextColor(frontColor);
    UTIL_LCD_SetBackColor(backColor);
    UTIL_LCD_DisplayStringAt(offsetX + locationX, offsetY + locationY, spring, LEFT_MODE);
}

void DrawPluralGames() {
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
    for (int i = 0; i < GAME_DIMENSION; i++) {
      for (int j = 0; j < GAME_DIMENSION; j++) {
        if ((i+j) % 2 == 0) {
          DrawGameSegmentBackground(i, j, UTIL_LCD_COLOR_LIGHTBLUE);
        } else {
          DrawGameSegmentBackground(i, j, UTIL_LCD_COLOR_LIGHTRED);
        }
      }
    }
    DrawGameSegmentForeground(0, 0, segmentGameX / 2 - 10, segmentGameY / 2 - 10, UTIL_LCD_COLOR_GREEN, UTIL_LCD_COLOR_LIGHTBLUE, (uint8_t*) "Snake");
    DrawGameSegmentForeground(1, 0, segmentGameX / 2 - 10, segmentGameY / 2 - 10, UTIL_LCD_COLOR_GREEN, UTIL_LCD_COLOR_LIGHTRED, (uint8_t*) "Minesweeper");
    DrawGameSegmentForeground(0, 1, segmentGameX / 2 - 10, segmentGameY / 2 - 10, UTIL_LCD_COLOR_GREEN, UTIL_LCD_COLOR_LIGHTRED, (uint8_t*) "Tetris");
    DrawGameSegmentForeground(1, 1, segmentGameX / 2 - 10, segmentGameY / 2 - 10, UTIL_LCD_COLOR_GREEN, UTIL_LCD_COLOR_LIGHTBLUE, (uint8_t*) "Hero Guitar");



    UTIL_LCD_SetFont(&Font24);
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_GREEN);
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *) "Game select", CENTER_MODE);
}

int pressChoice(int x, int y) {
    int selection = 0;

    int currentX = 0;
    int currentY = segmentGameY;
    for (int i = 0; i < GAME_DIMENSION; i++) {
      currentX = segmentGameX;
      for (int j = 0; j < GAME_DIMENSION; j++) {
        if (currentX > x && currentY > y) {
          return selection;
        }
        selection++;
        currentX += segmentGameX;
      }
      currentY += segmentGameY;
    }

    return -1;
}

void gameChoice(int* gameTracker) {
    TS_State_t state;

    BSP_TS_GetState(0, &state);
    if (!state.TouchDetected) {
      return;
    }

    int decision = pressChoice(state.TouchX, state.TouchY);
    if (decision == -1) {
      ErrOutput();
      return;
    }

    if (decision == 0) {
      Picasso(gameTracker);
    }
    if (decision == 1) {
      Bombica(gameTracker);
    }

}

int main(void)
{
  /* System Init, System clock, voltage scaling and L1-Cache configuration are done by CPU1 (Cortex-M7)
     in the meantime Domain D2 is put in STOP mode(Cortex-M4 in deep-sleep)
  */

  /* Enable the CPU Cache */
  CPU_CACHE_Enable();

  /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  /* Configure the system clock to 400 MHz */
  SystemClock_Config();


  BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);

  BSP_LCD_GetXSize(0, &xLength);
  BSP_LCD_GetYSize(0, &yLength);

  segmentGameX = xLength / GAME_DIMENSION;
  segmentGameY = yLength / GAME_DIMENSION;

  TS_Init_t touch_init;
  touch_init.Width = xLength;
  touch_init.Height = yLength;
  touch_init.Orientation = TS_SWAP_XY;
  touch_init.Accuracy = 5;

  uint32_t status = BSP_ERROR_NONE;
  status = BSP_TS_Init(0, &touch_init);
  if (status != BSP_ERROR_NONE) {
  	ErrOutput();
      return NULL;
  }

  //Touchscreen_demo();
  /* Wait For User inputs */
  DrawPluralGames();

  int gameTracker= 0;
  while (1)
  {
    gameChoice(&gameTracker);
    if (gameTracker) {
      gameTracker = 0;
      DrawPluralGames();
    }
  }
}
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
  *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /*!< Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

/* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if(ret != HAL_OK)
  {
    Error_Handler();
  }

 /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
 */

  /*activate CSI clock mondatory for I/O Compensation Cell*/
  __HAL_RCC_CSI_ENABLE() ;

  /* Enable SYSCFG clock mondatory for I/O Compensation Cell */
  __HAL_RCC_SYSCFG_CLK_ENABLE() ;

  /* Enables the I/O Compensation Cell */
  HAL_EnableCompensationCell();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Turn LED REDon */
  BSP_LED_On(LED_RED);
  while(1)
  {
  }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @brief  CPU L1-Cache enable.
  * @param  None
  * @retval None
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

/**
  * @}
  */

/**
  * @}
  */

