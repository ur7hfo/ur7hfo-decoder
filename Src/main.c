
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "banddecoder.h"
#include "eeprom24xx.h"
#include "menu.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_rx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

extern uint8_t trxCatBuffer[KENWOODCATLEN];
extern uint8_t loggerCatBuffer[KENWOODCATLEN];

uint8_t band = 0;
uint32_t freq = 0;

__IO uint32_t freqFromTrx = 0;
__IO uint32_t freqFromLogger = 0;

__IO ITStatus UartReady_RX_1 = RESET;
__IO ITStatus UartReady_TX_1 = RESET;
__IO ITStatus UartReady_RX_2 = RESET;
__IO ITStatus UartReady_TX_2 = RESET;
__IO ITStatus UartReady_RX_3 = RESET;
__IO ITStatus UartReady_TX_3 = RESET;

bool PttState = false;
bool logger = true;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  
  HAL_UART_Receive_DMA(&huart1, (uint8_t *) trxCatBuffer, sizeof(trxCatBuffer));
  HAL_UART_Receive_DMA(&huart2, (uint8_t *) loggerCatBuffer, sizeof(loggerCatBuffer));

#ifdef SSD1306
	ssd1306_Init();
	HAL_Delay(100);
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("UR7HFO", Font_7x10, White);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString("DECODER 0.01a", Font_7x10, White);
	ssd1306_UpdateScreen();
	HAL_Delay(1000);
	ssd1306_Fill(Black);
	ssd1306_UpdateScreen();
#endif

#ifdef LCD16X2
	LCD_ini();
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String("UR7HFO");
	LCD_SetPos(0,1);
	LCD_String("DECODER 0.01a");
	HAL_Delay(1000);
	LCD_Clear();
#endif

	/* Get tik from sys timer. */
	uint32_t time = HAL_GetTick();
	uint32_t time2 = HAL_GetTick();

	/* Check ext EEPROM connected or not. */
#ifdef EEPROM_YES
	if( EEPROM24XX_IsConnected() == false )
	{
		LCD_String("EEPROM ERROR");
		HAL_Delay(2000);
		Error_Handler();
	}
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

		if (HAL_GPIO_ReadPin(PTT_INPUT_GPIO_Port, PTT_INPUT_Pin) == GPIO_PIN_SET) {
			PttState = false;
		} else {
			PttState = true;
		}

		if(UartReady_RX_2 == SET) {
			// freqFromLogger = readKenwoodCat(loggerCatBuffer);
			UartReady_RX_2 = RESET;
			HAL_UART_Receive_DMA(&huart2, (uint8_t *) loggerCatBuffer, sizeof(loggerCatBuffer));
		}

		if(UartReady_RX_1 == SET) {
			// freqFromTrx = readKenwoodCat(trxCatBuffer);
			UartReady_RX_1 = RESET;
			HAL_UART_Receive_DMA(&huart1, (uint8_t *) trxCatBuffer, sizeof(trxCatBuffer));
		}

		freqFromLogger = readKenwoodCat(loggerCatBuffer);
		freqFromTrx = readKenwoodCat(trxCatBuffer);

		/* If PTT not active state */
		if (PttState == false) {
			if (freqFromTrx > 1500000 && freqFromTrx < 450000000) {
				if (freqFromTrx != freq) {
					freq = freqFromTrx;
					band = GetBand(freq);
					WriteSplit(band, freq);
					WriteBand(band);
					bcdOut(band);
					IcomOutToPort(0x00, freq, 0x00);
					KenwoodOutToPort(freq);
					DisplayDraw(band, freq);
				}
			} else if (freqFromLogger > 1500000 && freqFromLogger < 450000000) {
				if (freqFromLogger != freq) {
					freq = freqFromLogger;
					band = GetBand(freq);
					WriteSplit(band, freq);
					WriteBand(band);
					bcdOut(band);
					IcomOutToPort(0x00, freq, 0x00);
					KenwoodOutToPort(freq);
					DisplayDraw(band, freq);
				}
			}
		}

		if (HAL_GetTick() - time > POLLING_INTERVAL) {
			if (logger != true) {
				KenwoodRequest();
			}
			time = HAL_GetTick();
		}

		if (HAL_GetTick() - time2 > CAT_TX_INTERVAL) {
			logger = CheckLoggerActive(loggerCatBuffer);
			freqFromLogger = readKenwoodCat(loggerCatBuffer);
			freqFromTrx = readKenwoodCat(trxCatBuffer);
			FlushCatBuffers(loggerCatBuffer, trxCatBuffer);

			if (freqFromLogger == 0 && freqFromTrx == 0) {
				DisplayText("NO DATA");
			} else {
				DisplayDraw(band, freq);
			}

			/* cmd => 0x00 address => 0x00 Send broadcast freq. */
			IcomOutToPort(0x00, freq, 0x00);
			/* IcomOutToPortMod(0x01, 0x00, 0x07); *//* cmd => 0x01 address => 0x00 Send broadcast mode. */
			KenwoodOutToPort(freq);
			time2 = HAL_GetTick();
		}

		/* IcomCivEmulator(freq); */
	}
  
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART2 init function */
static void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* USART3 init function */
static void MX_USART3_UART_Init(void)
{

  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 PA5 
                           PA6 PA7 PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5 
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB12 PB13 
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PTT_INPUT_Pin */
  GPIO_InitStruct.Pin = PTT_INPUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PTT_INPUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUT_RIGHT_Pin */
  GPIO_InitStruct.Pin = BUT_RIGHT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUT_RIGHT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUT_LEFT_Pin BUT_DOWN_Pin BUT_UP_Pin BUT_SELECT_Pin */
  GPIO_InitStruct.Pin = BUT_LEFT_Pin|BUT_DOWN_Pin|BUT_UP_Pin|BUT_SELECT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* ------------------------------------------------------------------------------------------------ */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *UartHandle)
{
	uint32_t error = UartHandle->ErrorCode;

	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_UpdateScreen();

	/* Parity error */
	if (error == HAL_UART_ERROR_PE) {
		if (UartHandle->Instance == USART1)
			ssd1306_WriteString("UART1 parity error", Font_7x10, White);
		if (UartHandle->Instance == USART2)
			ssd1306_WriteString("UART2 parity error", Font_7x10, White);
		if (UartHandle->Instance == USART3)
			ssd1306_WriteString("UART3 parity error", Font_7x10, White);
	}

	/* Noise error */
	if (error == HAL_UART_ERROR_NE) {
		if (UartHandle->Instance == USART1)
			ssd1306_WriteString("UART1 noise error", Font_7x10, White);
		if (UartHandle->Instance == USART2)
			ssd1306_WriteString("UART2 noise error", Font_7x10, White);
		if (UartHandle->Instance == USART3)
			ssd1306_WriteString("UART3 noise error", Font_7x10, White);
	}

	/* Frame error */
	if (error == HAL_UART_ERROR_FE) {
		if (UartHandle->Instance == USART1)
			ssd1306_WriteString("UART1 frame error", Font_7x10, White);
		if (UartHandle->Instance == USART2)
			ssd1306_WriteString("UART2 frame error", Font_7x10, White);
		if (UartHandle->Instance == USART3)
			ssd1306_WriteString("UART3 frame error", Font_7x10, White);
	}

	/* Overrrun error */
	if (error == HAL_UART_ERROR_ORE) {
		if (UartHandle->Instance == USART1)
			ssd1306_WriteString("UART1 overrun error", Font_7x10, White);
		if (UartHandle->Instance == USART2)
			ssd1306_WriteString("UART2 overrun error", Font_7x10, White);
		if (UartHandle->Instance == USART3)
			ssd1306_WriteString("UART3 overrun error", Font_7x10, White);
	}

	/* DMA error */
	if (error == HAL_UART_ERROR_DMA) {
		if (UartHandle->Instance == USART1)
			ssd1306_WriteString("UART1 dma error", Font_7x10, White);
		if (UartHandle->Instance == USART2)
			ssd1306_WriteString("UART2 dma error", Font_7x10, White);
		if (UartHandle->Instance == USART3)
			ssd1306_WriteString("UART3 dma error", Font_7x10, White);
	}

	ssd1306_UpdateScreen();

	/* Frame error flag */
	__HAL_UART_CLEAR_FEFLAG(&huart1);
	__HAL_UART_CLEAR_FEFLAG(&huart2);
	__HAL_UART_CLEAR_FEFLAG(&huart3);

	/* Noise error flag */
	__HAL_UART_CLEAR_NEFLAG(&huart1);
	__HAL_UART_CLEAR_NEFLAG(&huart2);
	__HAL_UART_CLEAR_NEFLAG(&huart3);

	/* Overrun error flag */
	__HAL_UART_CLEAR_OREFLAG(&huart1);
	__HAL_UART_CLEAR_OREFLAG(&huart2);
	__HAL_UART_CLEAR_OREFLAG(&huart3);

	/* PE error */
	__HAL_UART_CLEAR_PEFLAG(&huart1);
	__HAL_UART_CLEAR_PEFLAG(&huart2);
	__HAL_UART_CLEAR_PEFLAG(&huart3);

	HAL_UART_Receive_DMA(&huart1, (uint8_t *) trxCatBuffer, sizeof(trxCatBuffer));
	HAL_UART_Receive_DMA(&huart2, (uint8_t *) loggerCatBuffer, sizeof(loggerCatBuffer));


	/* Error_Handler(); */
}


/* ------------------------------------------------------------------------------------------------- */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
 {
	if (UartHandle->Instance == USART1) {
		UartReady_TX_1 = SET;
	}

	if (UartHandle->Instance == USART2) {
		UartReady_TX_2 = SET;
	}

	if (UartHandle->Instance == USART3) {
		UartReady_TX_3 = SET;
	}
}


/* ------------------------------------------------------------------------------------------------- */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
 {
	if (UartHandle->Instance == USART1) {
		UartReady_RX_1 = SET;
		/* hdma_usart1_rx.State = HAL_DMA_STATE_BUSY; */
	}

	if (UartHandle->Instance == USART2) {
		UartReady_RX_2 = SET;
		/* hdma_usart2_rx.State = HAL_DMA_STATE_BUSY; */
	}

	if (UartHandle->Instance == USART3) {
	}
}


/* void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {  } */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	char line_buf[16] = { 0 };

#ifdef LCD16X2
	LCD_Clear();
	LCD_SetPos(0,0);
	LCD_String(file);

	LCD_SetPos(0,1);
	LCD_String("STR: ");
	sprintf(line_buf, "%d", line);
	LCD_String(line_buf);
#endif

#ifdef SSD1306
	ssd1306_Fill(Black);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(file, Font_7x10, White);

	ssd1306_SetCursor(0, 16);
	sprintf(line_buf, "%d", line);
	ssd1306_WriteString(line_buf, Font_7x10, White);
	ssd1306_UpdateScreen();
#endif

	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
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
  /* USER CODE BEGIN 6 */
  
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
