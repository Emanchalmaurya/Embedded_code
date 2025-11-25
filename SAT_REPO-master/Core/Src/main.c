/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MPU6050_ADDR        (0x68 << 1)
#define PWR_MGMT_1          0x6B
#define GYRO_XOUT_H         0x43


/* ICM-20948 I²C Definitions */
#define ICM20948_ADDR        (0x68 << 1)    // AD0=GND (default)
#define ICM20948_WHO_AM_I    0x00
#define ICM20948_PWR_MGMT_1  0x06
#define ICM20948_PWR_MGMT_2  0x07
#define ICM20948_ACCEL_XOUT_H 0x2D
#define ICM20948_GYRO_XOUT_H  0x33
#define ICM20948_INT_PIN_CFG  0x0F


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t soldier_id = 0x7;
uint8_t firing_mode = 0x2;
uint8_t weapon_id = 0x5;
uint32_t trigger_count = 0;
uint32_t last_sent_count = 0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void USART2_Print(char* msg);
void XBee_Send(char* msg);
void delay_us(uint16_t us);
void ir_on(void);
void ir_off(void);
void send_burst(uint16_t on_time, uint16_t off_time);
void send_bit(uint8_t bit);
void send_nec(uint8_t addr, uint8_t data);
void MPU6050_Init(void);
void MPU6050_Read_Gyro(int16_t* gx, int16_t* gy, int16_t* gz);
void Error_Handler(void);


//void MPU9250_Init(void);
//void MPU9250_ReadRaw(int16_t* accel, int16_t* gyro, int16_t* mag, int16_t* temp);
void Calculate_RPY(int16_t* accel, int16_t* mag, float* roll, float* pitch, float* yaw);
void print_uart(const char* msg);
void I2C_Scan(void) ;
void ICM20948_Init(void);
void ICM20948_ReadRaw(int16_t *accel, int16_t *gyro);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


  USART2_Print("System Initialized.\r\n");

    //check_addresses();
 // I2C_Scan();
  ICM20948_Init();
    USART2_Print("MPU9250 Initialized.\r\n");

    uint8_t command = (firing_mode << 4) | (weapon_id & 0x0F);
    uint16_t accel[3], gyro[3], mag[3];
    float roll, pitch, yaw;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //uint16_t gx, gy, gz;
	     char xbee_msg[128];
	      char debug_msg[128];

	      static uint32_t last_trigger_count = 0;
	     ICM20948_ReadRaw(accel, gyro);
	      Calculate_RPY(accel, mag, &roll, &pitch, &yaw);


	      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET)
	      {
	        trigger_count++;
	        last_trigger_count = trigger_count;

	        snprintf(debug_msg, sizeof(debug_msg), "Trigger pressed! New count: %lu\r\n", trigger_count);
	        USART2_Print(debug_msg);
	        //XBee_Send(debug_msg);
	        for (int i = 0; i < 4; i++) {
	          send_nec(soldier_id, command);
	          HAL_Delay(120);
	        }
	        USART2_Print("IR signal sent.\r\n");

	        HAL_Delay(300);
	      }

	     snprintf(xbee_msg,sizeof(xbee_msg),"Roll: %.2f  Pitch: %.2f  Yaw: %.2f Trigger_count :%lu\r\n", roll, pitch, yaw,last_trigger_count);
	      //USART2_Print(buf);
	      //HAL_Delay(100);  // 10Hz update rate


	     // snprintf(xbee_msg, sizeof(xbee_msg), "%d,%d,%d,%lu\r\n",
	       //          last_gx, last_gy, last_gz, last_trigger_count);
	     XBee_Send(xbee_msg);
	      snprintf(debug_msg, sizeof(debug_msg), "Sent: %s", xbee_msg);
	      USART2_Print(debug_msg);

	      HAL_Delay(100);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10D19CE4;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2105;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 701;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 79;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SMPS_EN_Pin|SMPS_SW_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : Trigger_Pin */
  GPIO_InitStruct.Pin = Trigger_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Trigger_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SMPS_EN_Pin SMPS_SW_Pin */
  GPIO_InitStruct.Pin = SMPS_EN_Pin|SMPS_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 SMPS_PG_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_5|SMPS_PG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD4_Pin */
  GPIO_InitStruct.Pin = LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD4_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void USART2_Print(char* msg)
{
  HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void XBee_Send(char* msg)
{
  if(HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY)==HAL_OK){
	  HAL_UART_Transmit(&huart2, "xbee sent", 10, HAL_MAX_DELAY);
  }
}

void delay_us(uint16_t us)
{
  __HAL_TIM_SET_COUNTER(&htim6, 0);
  HAL_TIM_Base_Start(&htim6);
  while (__HAL_TIM_GET_COUNTER(&htim6) < us);
  HAL_TIM_Base_Stop(&htim6);
}

void ir_on(void)  { HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); }
void ir_off(void) { HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1); }
void send_burst(uint16_t on_time, uint16_t off_time)
{
  ir_on(); delay_us(on_time);
  ir_off(); delay_us(off_time);
}

void send_bit(uint8_t bit)
{
  if (bit) send_burst(560, 1690);
  else     send_burst(560, 560);
}

void send_nec(uint8_t addr, uint8_t data)
{
  send_burst(9000, 4500);
  for (int i = 0; i < 8; i++) send_bit((addr >> i) & 1);
  for (int i = 0; i < 8; i++) send_bit((~addr >> i) & 1);
  for (int i = 0; i < 8; i++) send_bit((data >> i) & 1);
  for (int i = 0; i < 8; i++) send_bit((~data >> i) & 1);
  send_burst(560, 0);
}

void ICM20948_Init(void)
{
    uint8_t who_am_i = 0;
    uint8_t data;
    char msg[64];

    HAL_Delay(100);

    // 1. Read WHO_AM_I register (0x00) from BANK 0
    HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR, ICM20948_WHO_AM_I,
                     I2C_MEMADD_SIZE_8BIT, &who_am_i, 1, HAL_MAX_DELAY);

    sprintf(msg, "WHO_AM_I = 0x%02X\r\n", who_am_i);
    USART2_Print(msg);

    if (who_am_i == 0xEA)  // ICM-20948 detected
    {
        USART2_Print("ICM20948 detected\r\n");

        // 2. Reset device
        data = 0x01;  // set DEVICE_RESET
        HAL_I2C_Mem_Write(&hi2c1, ICM20948_ADDR, ICM20948_PWR_MGMT_1,
                          I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
        HAL_Delay(100);

        // 3. Wake up device, select auto clock
        data = 0x01;
        HAL_I2C_Mem_Write(&hi2c1, ICM20948_ADDR, ICM20948_PWR_MGMT_1,
                          I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

        // 4. Enable accel and gyro
        data = 0x00;
        HAL_I2C_Mem_Write(&hi2c1, ICM20948_ADDR, ICM20948_PWR_MGMT_2,
                          I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);

        HAL_Delay(10);
    }
    else
    {
        sprintf(msg, "Unknown device: 0x%02X\r\n", who_am_i);
        USART2_Print(msg);
        Error_Handler();
    }
}

void ICM20948_ReadRaw(int16_t *accel, int16_t *gyro)
{
    uint8_t buf[12];

    // Read Accel (6 bytes) + Gyro (6 bytes) from BANK 0
    HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR, ICM20948_ACCEL_XOUT_H,
                     I2C_MEMADD_SIZE_8BIT, buf, 12, HAL_MAX_DELAY);

    accel[0] = (int16_t)(buf[0] << 8 | buf[1]);
    accel[1] = (int16_t)(buf[2] << 8 | buf[3]);
    accel[2] = (int16_t)(buf[4] << 8 | buf[5]);
    gyro[0]  = (int16_t)(buf[6] << 8 | buf[7]);
    gyro[1]  = (int16_t)(buf[8] << 8 | buf[9]);
    gyro[2]  = (int16_t)(buf[10] << 8 | buf[11]);
}

/* Calculate Roll, Pitch, Yaw */
void Calculate_RPY(int16_t* accel, int16_t* mag, float* roll, float* pitch, float* yaw) {
    float ax = accel[0], ay = accel[1], az = accel[2];
    float mx = mag[0], my = mag[1], mz = mag[2];

    *roll = atan2f(ay, az) * 180 / M_PI;
    *pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180 / M_PI;

    float roll_rad = *roll * M_PI / 180.0f;
    float pitch_rad = *pitch * M_PI / 180.0f;

    float mx2 = mx * cosf(pitch_rad) + mz * sinf(pitch_rad);
    float my2 = mx * sinf(roll_rad) * sinf(pitch_rad) + my * cosf(roll_rad) - mz * sinf(roll_rad) * cosf(pitch_rad);

    *yaw = atan2f(-my2, mx2) * 180 / M_PI;
    if (*yaw < 0) *yaw += 360;
}

/////test case for debugging////////////
void I2C_Scan(void)
{
    char msg[32];
    HAL_StatusTypeDef res;
    for (uint8_t i = 1; i < 128; i++)
    {
        res = HAL_I2C_IsDeviceReady(&hi2c1, (i << 1), 2, 10);
        if (res == HAL_OK)
        {
            sprintf(msg, "Found at 0x%02X\r\n", i << 1);
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        }
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)"Scan done\r\n", 11, HAL_MAX_DELAY);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
