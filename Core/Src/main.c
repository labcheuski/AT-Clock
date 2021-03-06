/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "p10.h"
//#include "st7789.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUTTON_RANGE 5
#define BUTTON_REPEAT_DELAY 100
#define BUTTON_REPEAT_PERIOD 10
#define MODE_BRIGHTNESS 0
#define MODE_HOUR 1
#define MODE_MINUTE 2
#define MODE_SECOND 3
#define MODE_CNT 4
#define BUZZER_ON 1
#define BUZZER_PRESCALER1 116
#define BUZZER_PRESCALER2 110
#define BUZZER_PRESCALER_BUTTON 2000
#define SW_READY 0
#define SW_STARTED 1
#define SW_STOPPED 2

#define DEV_ADDR 0xD0

#define MAX(A,B) ((A) > (B) ? (A) : (B))
#define MIN(A,B) ((A) < (B) ? (A) : (B))

#define PROGRAM_CLOCK 0
#define PROGRAM_STOPWATCH 1
#define PROGRAM_TIMER3 2
#define PROGRAM_CNT 3

#define TIMER3_START -3*60*100

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

#define BRIGHTNESS_STEPS 11
uint32_t brightness[] = {4, 9, 14, 19, 24, 29, 36, 49, 64, 79, 99};
int brightness_index = 4;

#define SIGNAL_STEPS 13
int signals[] = {5, 10, 15, 20, 30, 45, 0, 60, 90, 120, 180, 240, 300};
int signal_index = 6;

void Test_P10();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

int IntToBCD (int Value)
{
   return (((Value / 10) << 4) | (Value % 10));
}
int BCDToInt(int BCD)
{
   return (((BCD >> 4) * 10) + (BCD & 0xF));
}

uint8_t p10_test[P10_BUF_SIZE] = {
    0b00110001, 0b10111101, 0b00101100, 0b00110001, 0b11010010, 0b10000001, 0b10000000, 0b00000000,
    0b01010010, 0b10100001, 0b00100010, 0b00000010, 0b00010010, 0b00000010, 0b10000000, 0b00000000,
    0b10010100, 0b10111001, 0b00100001, 0b01001010, 0b00010100, 0b10000100, 0b10000000, 0b00000000,
    0b10010100, 0b10100100, 0b11100111, 0b01001010, 0b00011000, 0b10000100, 0b10000000, 0b00000000,
    0b10010111, 0b10100100, 0b00100001, 0b00111010, 0b00010100, 0b10000111, 0b10000000, 0b00000000,
    0b10010100, 0b10100100, 0b00100010, 0b00001010, 0b00010010, 0b10000100, 0b10000000, 0b00000000,
    0b10010100, 0b10111000, 0b00101100, 0b00110001, 0b11010010, 0b10000100, 0b10100000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00001100, 0b11110001, 0b00101111,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000110, 0b00010010, 0b00010001, 0b00101000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001010, 0b01000010, 0b01100101, 0b00101110,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00000100, 0b00010001, 0b11100001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b01001000, 0b00010100, 0b00100001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00010000, 0b00010000, 0b00100001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b00011110, 0b11100000, 0b00101110
};

uint8_t p10_start[P10_BUF_SIZE] = {
    0b00111010, 0b00011100, 0b00111011, 0b11101110, 0b00110001, 0b10000000, 0b00000000, 0b00000000,
    0b01001010, 0b00010010, 0b01001000, 0b10001001, 0b01001010, 0b00000000, 0b00000000, 0b00000000,
    0b10001010, 0b00010010, 0b10001000, 0b10001001, 0b01001010, 0b00000000, 0b00000000, 0b00000000,
    0b10001010, 0b00011100, 0b10001000, 0b10001110, 0b01001001, 0b10000000, 0b00000000, 0b00000000,
    0b11111010, 0b00010010, 0b11111000, 0b10001001, 0b01001000, 0b01000000, 0b00000000, 0b00000000,
    0b10001010, 0b00010010, 0b10001000, 0b10001001, 0b01001000, 0b01000000, 0b00000000, 0b00000000,
    0b10001011, 0b11011100, 0b10001000, 0b10001001, 0b00110001, 0b10000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000111, 0b11010100, 0b01010100, 0b01001110,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00010110, 0b11010110, 0b01010000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00010101, 0b01010101, 0b01010000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00010100, 0b01010100, 0b11010000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00010100, 0b01010100, 0b01010011,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00010100, 0b01010100, 0b01010001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000001, 0b00010100, 0b01010100, 0b01001110,
};

uint8_t p10_brightness[P10_BUF_SIZE] = {
    0b01110111, 0b00100100, 0b11000111, 0b01111101, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b10100101, 0b00101000, 0b00010001, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b10101001, 0b00101000, 0b00010001, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b01110111, 0b00110001, 0b00101000, 0b00010001, 0b11000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101001, 0b00101000, 0b00010001, 0b00100000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00100101, 0b00101000, 0b00010001, 0b00100000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00100100, 0b11000111, 0b00010001, 0b11000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
};

uint8_t p10_program_clock[P10_BUF_SIZE] = {
    0b11100111, 0b10101010, 0b10001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101010, 0b10001011, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101010, 0b10001010, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b11100111, 0b10011100, 0b10011010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b00101010, 0b10101010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b00101010, 0b11001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000111, 0b10101010, 0b10001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010010, 0b00110011, 0b10100001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010010, 0b01010100, 0b00100001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00010010, 0b10010100, 0b00100001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00001110, 0b10010100, 0b00111001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b11110100, 0b00100101,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b10010100, 0b00100101,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000010, 0b10010011, 0b10111001,
};

uint8_t p10_program_stopwatch[P10_BUF_SIZE] = {
    0b11100111, 0b10101010, 0b10001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101010, 0b10001011, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101010, 0b10001010, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b11100111, 0b10011100, 0b10011010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b00101010, 0b10101010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b00101010, 0b11001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000111, 0b10101010, 0b10001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000011, 0b10111101, 0b00101001, 0b01001000, 0b11000110, 0b01000101, 0b11101110,
    0b00000000, 0b00000100, 0b00100001, 0b00101001, 0b01001001, 0b01001001, 0b01101101, 0b00001001,
    0b00000000, 0b00000100, 0b00100001, 0b01001001, 0b01001001, 0b01001001, 0b01010101, 0b00001001,
    0b00000000, 0b00000100, 0b00111101, 0b10000111, 0b01111001, 0b01001001, 0b01000101, 0b11101110,
    0b00000000, 0b00000100, 0b00100001, 0b01000001, 0b01001001, 0b01001001, 0b01000101, 0b00001000,
    0b00000000, 0b00000100, 0b00100001, 0b00100001, 0b01001011, 0b11101001, 0b01000101, 0b00001000,
    0b00000000, 0b00000011, 0b10111101, 0b00100110, 0b01001010, 0b00100110, 0b01000101, 0b11101000,
};

uint8_t p10_program_timer3[P10_BUF_SIZE] = {
    0b11100111, 0b10101010, 0b10001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101010, 0b10001011, 0b01100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10010100, 0b00101010, 0b10001010, 0b10100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b11100111, 0b10011100, 0b10011010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b00101010, 0b10101010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b00101010, 0b11001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000111, 0b10101010, 0b10001010, 0b00100000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b11111000, 0b11001110, 0b01000101, 0b11101110,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100001, 0b01000000, 0b01101101, 0b00001001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100010, 0b01010001, 0b01010101, 0b00001001,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100010, 0b01010011, 0b01000101, 0b11101110,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100011, 0b11010101, 0b01000101, 0b00001000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100010, 0b01011001, 0b01000101, 0b00001000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00100010, 0b01010001, 0b01000101, 0b11101000,
};

uint8_t p10_signal[P10_BUF_SIZE] = {
    0b01110100, 0b10111101, 0b00100011, 0b00011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b10100001, 0b00100101, 0b00101000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b10100001, 0b00101001, 0b01001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000100, 0b10100001, 0b11101001, 0b01001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000101, 0b10100001, 0b00101111, 0b01001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b10000110, 0b10100001, 0b00101001, 0b01001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b01110100, 0b10100001, 0b00101001, 0b01001000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,

    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001011, 0b11011111,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001010, 0b00000100,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001010, 0b00000100,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01111011, 0b11100100,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001010, 0b00000100,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001010, 0b00000100,
    0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b01001011, 0b11100100,
};

void p10_test_text()
{
  p10_putscreen(p10_test);
  HAL_Delay(10000);
  p10_putrect(0, 0, 64, 16, 0);
}

void main_loop_clock()
{
  uint8_t Buf[8];
  uint8_t last = 0;

  int button_mode = -BUTTON_RANGE;
  int button_up = -BUTTON_RANGE;
  int button_down = -BUTTON_RANGE;
  int button_mode_pressed = 0;
  int button_up_pressed = 0;
  int button_down_pressed = 0;

  int mode = 0;

  p10_putrect(0, 0, 64, 16, 0);

  int loop_cnt = 0;
  int loop_half = 0;
  while (1) {
    //Get Time
    Buf[0] = 0;
    HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 1, 1000);
    HAL_Delay(10); //?????? ???? ????????????
    HAL_I2C_Master_Receive(&hi2c1, DEV_ADDR, (uint8_t*) Buf, 3, (uint32_t)1000);

    loop_cnt++;
    if (last != Buf[0]) {
      last = Buf[0];

      loop_half = loop_cnt / 2;
      loop_cnt = 0;

      p10_putnumber(0,  Buf[2] >> 4, 0);
      p10_putnumber(10, Buf[2] & 0x0F, 0);
      p10_putnumber(23, Buf[1] >> 4, 0);
      p10_putnumber(33, Buf[1] & 0x0F, 0);
      p10_putnumber(46, Buf[0] >> 4 & 0x0F, 0);
      p10_putnumber(56, Buf[0] & 0x0F, 0);

      p10_putrect(19, 4, 3, 2, 1);
      p10_putrect(19, 10, 3, 2, 1);
      p10_putrect(42, 4, 3, 2, 1);
      p10_putrect(42, 10, 3, 2, 1);

#if (BUZZER_ON)
        //Buzzer
  //      if (Buf[0] & 0x0F > 0x55) {
        if ((Buf[0] & 0x0F) > 0x05) {
          __HAL_TIM_SET_PRESCALER(&htim2, 79);
          HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        }
  //      if (Buf[0] == 0) {
        if ((Buf[0] & 0x0F) == 0) {
          __HAL_TIM_SET_PRESCALER(&htim2, 75);
          HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
        }
#endif
    }

#if (BUZZER_ON)
      if (loop_cnt == loop_half >> 1) {
        //      if (Buf[0] != 0) {
        if ((Buf[0] & 0x0F) != 0) {
          HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
          HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
          HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
        }
      }
#endif

    if (loop_cnt == loop_half) {

      //???????????????? - ?????????????????? ????????
      switch (mode) {
      case MODE_HOUR:
        p10_putrect(0, 0, 18, 16, 0);
        break;
      case MODE_MINUTE:
        p10_putrect(23, 0, 18, 16, 0);
        break;
      case MODE_SECOND:
        p10_putrect(46, 0, 18, 16, 0);
        break;
      }
    }

    HAL_Delay(10);
    //HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, HAL_GPIO_ReadPin(BUT_MODE_GPIO_Port, BUT_MODE_Pin));

    //???????????? ????????????
    if (!HAL_GPIO_ReadPin(BUT_MODE_GPIO_Port, BUT_MODE_Pin)) {
      if (button_mode < BUTTON_RANGE) button_mode++;
      if (button_mode == 0) {
        button_mode_pressed = 1;
        button_mode = BUTTON_RANGE;
      }
    } else {
      if (button_mode > -BUTTON_RANGE) button_mode--;
      if (button_mode == 0) button_mode = -BUTTON_RANGE;
    }
    if (!HAL_GPIO_ReadPin(BUT_UP_GPIO_Port, BUT_UP_Pin)) {
      if (button_up < BUTTON_RANGE) button_up++;
      if (button_up == 0) {
        button_up_pressed = 1;
        button_up = BUTTON_RANGE;
      }
    } else {
      if (button_up > -BUTTON_RANGE) button_up--;
      if (button_up == 0) button_up = -BUTTON_RANGE;
    }
    if (!HAL_GPIO_ReadPin(BUT_DOWN_GPIO_Port, BUT_DOWN_Pin)) {
      if (button_down < BUTTON_RANGE) button_down++;
      if (button_down == 0) {
        button_down_pressed = 1;
        button_down = BUTTON_RANGE;
      }
    } else {
      if (button_down > -BUTTON_RANGE) button_down--;
      if (button_down == 0) button_down = -BUTTON_RANGE;
    }

    //?????????????????? ?????????????? ????????????
    if (button_mode_pressed) {
//      p10_test_text();
      button_mode_pressed = 0;
      mode = (mode + 1) % MODE_CNT;
    }

    if (button_up_pressed) {
      button_up_pressed = 0;

      switch (mode) {
      case MODE_BRIGHTNESS:
        brightness_index = MIN(brightness_index + 1, BRIGHTNESS_STEPS-1);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness[brightness_index]);
        p10_putrect(0, 0, 64, 16, 0);
        p10_putnumber(46, brightness[brightness_index] / 10 % 10, 0);
        p10_putnumber(56, brightness[brightness_index] % 10, 0);
        HAL_Delay(1000);
        break;
      case MODE_HOUR:
        Buf[0] = 2;
        Buf[1] = IntToBCD((BCDToInt(Buf[2]) + 1) % 24);
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 2, 1000);
        break;
      case MODE_MINUTE:
        Buf[0] = 1;
        Buf[1] = IntToBCD((BCDToInt(Buf[1]) + 1) % 60);
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 2, 1000);
        break;
      case MODE_SECOND:
        Buf[0] = 0;
        Buf[1] = 0;
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 2, 1000);
        break;
      }
    }

    if (button_down_pressed) {
      button_down_pressed = 0;

      switch (mode) {
      case MODE_BRIGHTNESS:
        brightness_index = MAX(brightness_index - 1, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness[brightness_index]);
        p10_putrect(0, 0, 64, 16, 0);
        p10_putnumber(46, brightness[brightness_index] / 10 % 10, 0);
        p10_putnumber(56, brightness[brightness_index] % 10, 0);
        HAL_Delay(1000);
        break;
      case MODE_HOUR:
        Buf[0] = 2;
        Buf[1] = Buf[2] ? IntToBCD(BCDToInt(Buf[2]) - 1) : 0x23;
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 2, 1000);
        break;
      case MODE_MINUTE:
        Buf[0] = 1;
        Buf[1] = Buf[1] ? IntToBCD(BCDToInt(Buf[1]) - 1) : 0x59;
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 2, 1000);
        break;
      case MODE_SECOND:
        Buf[1] = ((Buf[0] & 0xF0) + 0x10) % 0x60;
        Buf[0] = 0;
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 2, 1000);
        break;
      }
    }
  }
}

void p10_zero()
{
  p10_putrect(0, 0, 64, 16, 0);
  p10_putnumber(10, 0, 0);
  p10_putnumber(23, 0, 0);
  p10_putnumber(33, 0, 0);
  p10_putnumber(46, 0, 0);
  p10_putrect(19, 4, 3, 2, 1);
  p10_putrect(19, 10, 3, 2, 1);
  p10_putrect(42, 14, 3, 2, 1);
}

void main_loop_stopwatch()
{
  uint8_t is_setting = 0;
  uint8_t subseconds = 0;

  uint8_t Buf[8];
  uint8_t last = 0;

  int button_mode = -BUTTON_RANGE;
  int button_up = -BUTTON_RANGE;
  int button_down = -BUTTON_RANGE;
  int button_mode_pressed = 0;
  int button_up_pressed = 0;
  int button_down_pressed = 0;

  uint8_t is_started = 0;
  uint8_t is_stopped = 0;
  p10_zero();

  int loop_cnt = 0;
  int last_loop_cnt = 0;
  int last_subseconds = 0;
  while (1) {
    //Get Time
    Buf[0] = 0;
    HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 1, 1000);
    HAL_Delay(10); //?????? ???? ????????????
    HAL_I2C_Master_Receive(&hi2c1, DEV_ADDR, (uint8_t*) Buf, 3, (uint32_t)1000);

    loop_cnt++;
    if (last != Buf[0]) {
      last = Buf[0];

      last_loop_cnt = loop_cnt;
      loop_cnt = 0;

      if (is_started && !is_stopped) {
        if (Buf[1] >> 4 > 0) p10_putnumber(0, Buf[1] >> 4, 0);
        p10_putnumber(10, Buf[1] & 0x0F, 0);
        p10_putnumber(23, Buf[0] >> 4 & 0x0F, 0);
        p10_putnumber(33, Buf[0] & 0x0F, 0);
      }
    }

    if (is_started && !is_stopped) {
      if (loop_cnt >= last_loop_cnt) {
        subseconds = 9;
      } else {
        subseconds = loop_cnt * 10 / last_loop_cnt % 10;
      }

      if (subseconds != last_subseconds) {
        p10_putnumber(46, subseconds, 0);
        last_subseconds = subseconds;
      }
    }

    HAL_Delay(10);
    //HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, HAL_GPIO_ReadPin(BUT_MODE_GPIO_Port, BUT_MODE_Pin));

    //???????????? ????????????
    if (!HAL_GPIO_ReadPin(BUT_MODE_GPIO_Port, BUT_MODE_Pin)) {
      if (button_mode < BUTTON_RANGE) button_mode++;
      if (button_mode == 0) {
        button_mode_pressed = 1;
        button_mode = BUTTON_RANGE;
      }
    } else {
      if (button_mode > -BUTTON_RANGE) button_mode--;
      if (button_mode == 0) button_mode = -BUTTON_RANGE;
    }
    if (!HAL_GPIO_ReadPin(BUT_UP_GPIO_Port, BUT_UP_Pin)) {
      if (button_up < BUTTON_RANGE) button_up++;
      if (button_up == 0) {
        button_up_pressed = 1;
        button_up = BUTTON_RANGE;
      }
    } else {
      if (button_up > -BUTTON_RANGE) button_up--;
      if (button_up == 0) button_up = -BUTTON_RANGE;
    }
    if (!HAL_GPIO_ReadPin(BUT_DOWN_GPIO_Port, BUT_DOWN_Pin)) {
      if (button_down < BUTTON_RANGE) button_down++;
      if (button_down == 0) {
        button_down_pressed = 1;
        button_down = BUTTON_RANGE;
      }
    } else {
      if (button_down > -BUTTON_RANGE) button_down--;
      if (button_down == 0) button_down = -BUTTON_RANGE;
    }

    if (button_mode_pressed) {
      button_mode_pressed = 0;

      if (!is_started) {
        is_setting = is_setting ? 0 : 1;
        if (is_setting) {
          p10_putrect(0, 0, 64, 16, 0);
          p10_putnumber(46, brightness[brightness_index] / 10 % 10, 0);
          p10_putnumber(56, brightness[brightness_index] % 10, 0);
        } else {
          p10_zero();
        }
      }
    }

    if (button_up_pressed) {
      button_up_pressed = 0;

      if (is_setting) {
        brightness_index = MIN(brightness_index + 1, sizeof(brightness));
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness[brightness_index]);
        p10_putrect(0, 0, 64, 16, 0);
        p10_putnumber(46, brightness[brightness_index] / 10 % 10, 0);
        p10_putnumber(56, brightness[brightness_index] % 10, 0);
      } else if (is_stopped) {
        //reset
        is_started = 0;
        is_stopped = 0;
        p10_zero();
      } else if (!is_started) {
        //start
        is_started = 1;
        Buf[0] = 0;
        Buf[1] = 0;
        Buf[2] = 0;
        Buf[3] = 0;
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Buf, 4, 1000);
        last_loop_cnt = 45;
        loop_cnt = 0;
        last = 0;
      }
    }

    if (button_down_pressed) {
      button_down_pressed = 0;

      if (is_setting) {
        brightness_index = MAX(brightness_index - 1, 0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness[brightness_index]);
        p10_putrect(0, 0, 64, 16, 0);
        p10_putnumber(46, brightness[brightness_index] / 10 % 10, 0);
        p10_putnumber(56, brightness[brightness_index] % 10, 0);
      } else if (is_started && !is_stopped) {
        is_stopped = 1;
      }
    }
  }
}

int button_mode = -BUTTON_RANGE;
int button_up = -BUTTON_RANGE;
int button_down = -BUTTON_RANGE;
int button_mode_repeat = BUTTON_REPEAT_DELAY;
int button_up_repeat = BUTTON_REPEAT_DELAY;
int button_down_repeat = BUTTON_REPEAT_DELAY;
int button_mode_pressed = 0;
int button_mode_unpressed = 0;
int button_mode_repeated = 0;
int button_up_pressed = 0;
int button_up_unpressed = 0;
int button_up_repeated = 0;
int button_down_pressed = 0;
int button_down_unpressed = 0;
int button_down_repeated = 0;

void button_filter() {
  //???????????? ????????????
  button_mode_pressed = 0;
  button_mode_unpressed = 0;
  button_mode_repeated = 0;
  button_up_pressed = 0;
  button_up_unpressed = 0;
  button_up_repeated = 0;
  button_down_pressed = 0;
  button_down_unpressed = 0;
  button_down_repeated = 0;

  if (!HAL_GPIO_ReadPin(BUT_MODE_GPIO_Port, BUT_MODE_Pin)) {
    if (button_mode < BUTTON_RANGE) button_mode++;
    if (button_mode == 0) {
      button_mode_pressed = 1;
      button_mode = BUTTON_RANGE;
    }
    if (button_mode == BUTTON_RANGE) {
      if (button_mode_repeat-- <= 0) {
        button_mode_repeated = 1;
        button_mode_repeat = BUTTON_REPEAT_PERIOD;
      }
    }
  } else {
    if (button_mode > -BUTTON_RANGE) button_mode--;
    if (button_mode == 0) {
      button_mode_unpressed = 1;
      button_mode = -BUTTON_RANGE;
      button_mode_repeat = BUTTON_REPEAT_DELAY;
    }
  }

  if (!HAL_GPIO_ReadPin(BUT_UP_GPIO_Port, BUT_UP_Pin)) {
    if (button_up < BUTTON_RANGE) button_up++;
    if (button_up == 0) {
      button_up_pressed = 1;
      button_up = BUTTON_RANGE;
    }
    if (button_up == BUTTON_RANGE) {
      if (button_up_repeat-- <= 0) {
        button_up_repeated = 1;
        button_up_repeat = BUTTON_REPEAT_PERIOD;
      }
    }
  } else {
    if (button_up > -BUTTON_RANGE) button_up--;
    if (button_up == 0) {
      button_up_unpressed = 1;
      button_up = -BUTTON_RANGE;
      button_up_repeat = BUTTON_REPEAT_DELAY;
    }
  }

  if (!HAL_GPIO_ReadPin(BUT_DOWN_GPIO_Port, BUT_DOWN_Pin)) {
    if (button_down < BUTTON_RANGE) button_down++;
    if (button_down == 0) {
      button_down_pressed = 1;
      button_down = BUTTON_RANGE;
    }
    if (button_down == BUTTON_RANGE) {
      if (button_down_repeat-- <= 0) {
        button_down_repeated = 1;
        button_down_repeat = BUTTON_REPEAT_PERIOD;
      }
    }
  } else {
    if (button_down > -BUTTON_RANGE) button_down--;
    if (button_down == 0) {
      button_down_unpressed = 1;
      button_down = -BUTTON_RANGE;
      button_down_repeat = BUTTON_REPEAT_DELAY;
    }
  }
}

void main_loop_sound()
{

  p10_putrect(0, 0, 64, 16, 0);

  int prescaler = 159;
  int last = 0;
  while (1) {
    HAL_Delay(10);

    if (last != prescaler) {
      last = prescaler;
      p10_clrscr();
//      p10_putint(64, 320000 / (prescaler+1));
      p10_putint(64, prescaler, 0);
    }

    //???????????? ????????????
    button_filter();

    //?????????????????? ?????????????? ????????????
    if (button_mode_pressed) {
      button_mode_pressed = 0;
      __HAL_TIM_SET_PRESCALER(&htim2, prescaler);
      HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
    }
    if (button_mode_unpressed) {
      button_mode_unpressed = 0;
      HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
      HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    }

    if (button_up_pressed || button_up_repeated) {
      button_up_pressed = 0;
      button_up_repeated = 0;
      prescaler++;
    }
    if (button_down_pressed || button_down_repeated) {
      button_down_pressed = 0;
      button_down_repeated = 0;
      prescaler--;
    }
  }
}

void show_clock(uint8_t Buf[]) {
  p10_clrscr();
  if (Buf[2] >> 4 > 0) p10_putnumber(0,  Buf[2] >> 4, 0);
  p10_putnumber(10, Buf[2] & 0x0F, 0);
  p10_putnumber(23, Buf[1] >> 4, 0);
  p10_putnumber(33, Buf[1] & 0x0F, 0);
  p10_putnumber(46, Buf[0] >> 4 & 0x0F, 0);
  p10_putnumber(56, Buf[0] & 0x0F, 0);

  p10_putrect(19, 4, 3, 2, 1);
  p10_putrect(19, 10, 3, 2, 1);
  p10_putrect(42, 4, 3, 2, 1);
  p10_putrect(42, 10, 3, 2, 1);
}

void show_stopwatch(int time) {
  p10_clrscr();
  time = time / 10;
  p10_putnumber(46, time % 10, 0);
  time = time / 10;
  p10_putnumber(33, time % 10, 0);
  p10_putnumber(23, time % 60 / 10, 0);
  time = time / 60;
  p10_putnumber(10, time % 10, 0);
  time = time / 10;
  if (time > 0) p10_putnumber(0, time % 10, 0);
  p10_putrect(19, 4, 3, 2, 1);
  p10_putrect(19, 10, 3, 2, 1);
  p10_putrect(42, 14, 3, 2, 1);
}

void show_timer3(int time) {
  p10_clrscr();

  if (time < 0) {
    p10_putrect(24, 7, 6, 2, 1);
    time = 99 - time;
  }

  time = time / 100;
  p10_putnumber(56, time % 10, 0);
  time = time / 10;
  p10_putnumber(46, time % 6, 0);
  time = time / 6;
  p10_putnumber(33, time % 10, 0);
  time = time / 10;
  if (time > 0) p10_putnumber(23, time % 6, 0);
  p10_putrect(42, 4, 3, 2, 1);
  p10_putrect(42, 10, 3, 2, 1);
}

int buzzer_timer = 0;
void buzzer(int prescaler, int duration) {
  __HAL_TIM_SET_PRESCALER(&htim2, prescaler);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
  buzzer_timer = duration;
}
void buzzer_off() {
  if (buzzer_timer > 0) {
    buzzer_timer--;
    if (buzzer_timer == 0) {
      HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    }
  }
}

void main_loop_v2() {
  uint8_t Time[3];
  uint8_t last_second = 0xFF;
  int loop_cnt = 0;
  int last_loop_cnt = 0;
  int second = 0;
  int subsecond = 0;
  int last_subsecond = 0;
  int freeze = 0;
  int program = PROGRAM_CLOCK;
  int mode = 0;
  int mode_back = 0;
  int sw_state = SW_READY;
  int sw_time = 0;
  int t3_time = TIMER3_START;

//  p10_putscreen(p10_start);
//  p10_flip();
//  HAL_Delay(1000);
  while (1) {
    int second_changed = 0;
    int subsecond_changed = 0;
    int refresh = 0;
    HAL_Delay(5);

    //Get Time
    Time[0] = 0;
    HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Time, 1, 1000);
    HAL_Delay(5); //?????? ???? ????????????
    HAL_I2C_Master_Receive(&hi2c1, DEV_ADDR, (uint8_t*) Time, 3, (uint32_t)1000);

    loop_cnt++;
    if (last_second != Time[0]) {
      last_second = Time[0];
      last_loop_cnt = loop_cnt;
      loop_cnt = 0;
      second = BCDToInt(Time[1]) * 60 + BCDToInt(Time[0]);
      second_changed = 1;
    }

    if (last_loop_cnt > 0) {
      subsecond = loop_cnt * 10 / last_loop_cnt;
      if (subsecond > 9) subsecond = 9;
    }
    if (last_subsecond != subsecond) {
      last_subsecond = subsecond;
      subsecond_changed = 1;
    }

    buzzer_off();
    button_filter();

    //??????
    if (second_changed) {
      if (program == PROGRAM_CLOCK && mode == 0 && signals[signal_index] > 0) {
        if ((second + 4) % signals[signal_index] < 4 && signals[signal_index] >= 10) {
          buzzer(BUZZER_PRESCALER1, last_loop_cnt / 3);
        }
        if ((second) % signals[signal_index] == 0) {
          buzzer(BUZZER_PRESCALER2, last_loop_cnt);
        }
      }
    }
    if (button_mode_pressed || button_up_pressed || button_down_pressed) {
//      buzzer(BUZZER_PRESCALER_BUTTON, 1);
    }

    //?????????? ??????????????????????
    if (subsecond_changed && program == PROGRAM_STOPWATCH && mode == 0) {
      if (sw_state == SW_STARTED) {
        sw_time = second * 100 + subsecond * 10;
        refresh = 1;
      }
    }

    //???????????????????? - ??????????
    if (button_up_pressed && program == PROGRAM_STOPWATCH && mode == 0) {
      if (sw_state == SW_READY) {
        uint8_t Out4[4];
        memset(Out4, 0, 4);
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Out4, 4, 1000);
        second = 0;
        last_second = 0;
        loop_cnt = 0;
        sw_state = SW_STARTED;
        buzzer(BUZZER_PRESCALER1, last_loop_cnt / 3);
        continue;
      }
      if (sw_state == SW_STOPPED) {
        sw_state = SW_STARTED;
        refresh = 1;
      }
    }

    //???????????????????? - ??????????
    if (button_up_repeated && program == PROGRAM_STOPWATCH && mode == 0 && sw_state != SW_READY) {
      sw_state = SW_READY;
      sw_time = 0;
      refresh = 1;
    }

    //???????????????????? - ????????
    if (button_down_pressed && program == PROGRAM_STOPWATCH && mode == 0 && sw_state == SW_STARTED) {
      sw_state = SW_STOPPED;
//      sw_time = second * 100 + subsecond * 10;
      sw_time = second * 100 + MIN(99, loop_cnt * 100 / MAX(1, last_loop_cnt));
      refresh = 1;
      buzzer(BUZZER_PRESCALER2, last_loop_cnt);
    }

    //?????????? ??????????????
    if (subsecond_changed && program == PROGRAM_TIMER3 && mode == 0) {
      if (sw_state == SW_STARTED) {
        t3_time = 60*60*100 * BCDToInt(Time[2]) + 60*100 * BCDToInt(Time[1]) + 100 * BCDToInt(Time[0]) + 10 * subsecond;
        if (Time[2] >= 0x23) t3_time -= 24*60*60*100;
        refresh = 1;
        if (t3_time == 0) {
          buzzer(BUZZER_PRESCALER1, last_loop_cnt);
        }
      }
    }

    //???????????? - ??????????
    if (button_up_pressed && program == PROGRAM_TIMER3 && mode == 0) {
      if (sw_state == SW_READY) {
        uint8_t Out4[4];
        Out4[0] = 0;
        t3_time = 24*60*60*100 + t3_time;
        Out4[1] = IntToBCD(t3_time / 100 % 60);
        Out4[2] = IntToBCD(t3_time / (100*60) % 60);
        Out4[3] = IntToBCD(t3_time / (100*60*60) % 24);
        HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Out4, 4, 1000);
        second = 0;
        last_second = 0;
        loop_cnt = 0;
        sw_state = SW_STARTED;
        buzzer(BUZZER_PRESCALER1, last_loop_cnt / 3);
        continue;
      }
      if (sw_state == SW_STOPPED) {
        //TODO?
        sw_state = SW_STARTED;
        refresh = 1;
      }
    }

    //???????????? - ?????????? ???? -3
    if (button_up_repeated && program == PROGRAM_TIMER3 && mode == 0 && sw_state != SW_READY) {
      sw_state = SW_READY;
      t3_time = TIMER3_START;
      refresh = 1;
    }

    //???????????? - ????????  TODO
    if (button_down_pressed && program == PROGRAM_TIMER3 && mode == 0 && sw_state == SW_STARTED) {
      sw_state = SW_STOPPED;
//      t3_time = second * 100 + MIN(99, loop_cnt * 100 / MAX(1, last_loop_cnt));
      refresh = 1;
      buzzer(BUZZER_PRESCALER2, last_loop_cnt);
    }

    //?????????????????? ???????????? ?????????????? ????????????
    if (button_mode_pressed) {
      mode++;
      if (mode_back) {
        mode_back = 0;
        mode = 0;
      }
      if (program == PROGRAM_CLOCK && mode > 5) mode = 0;
      if (program == PROGRAM_STOPWATCH && mode > 2) mode = 0;
      if (program == PROGRAM_TIMER3 && mode > 2) mode = 0;
      refresh = 1;
    }

    //??????????????
    if ((button_up_pressed || button_down_pressed)
        && ((program == PROGRAM_CLOCK && mode == 0) || (program != PROGRAM_CLOCK && mode == 2)))
    {
      if (button_up_pressed) {
        if (brightness_index < BRIGHTNESS_STEPS - 1) brightness_index++;
      }
      if (button_down_pressed) {
        if (brightness_index > 0) brightness_index--;
      }
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness[brightness_index]);

      p10_putscreen(p10_brightness);
      p10_putint(64, brightness[brightness_index] + 1, 0);
      p10_flip();
      freeze = last_loop_cnt;
    }

    //???????????????????????? ????????????????
    if (mode == 1 && (button_up_pressed || button_down_pressed || button_mode_pressed)) {
      if (button_up_pressed || button_down_pressed) {
        program += button_up_pressed ? 1 : -1;
        if (program >= PROGRAM_CNT) program = 0;
        if (program < 0) program = PROGRAM_CNT - 1;
        mode_back = 1;
        refresh = 1;
      }
    }

    //?????????????? ????????
    if (program == PROGRAM_CLOCK && mode >= 3
        && (button_up_pressed || button_up_repeated || button_down_pressed || button_down_repeated))
    {
      uint8_t Out[2];
      if (button_up_pressed || button_up_repeated) {
        switch (mode) {
        case 5: Out[1] = IntToBCD((BCDToInt(Time[2]) + 1) % 24); break;
        case 4: Out[1] = IntToBCD((BCDToInt(Time[1]) + 1) % 60); break;
        case 3: Out[1] = 0; break;
        }
      }
      if (button_down_pressed || button_down_repeated) {
        switch (mode) {
        case 5: Out[1] = IntToBCD((BCDToInt(Time[2]) + 23) % 24); break;
        case 4: Out[1] = IntToBCD((BCDToInt(Time[1]) + 59) % 60); break;
        case 3: Out[1] = IntToBCD((BCDToInt(Time[0])/10 + 1) * 10 % 60); break;
        }
      }
      Out[0] = mode - 3;
      HAL_I2C_Master_Transmit(&hi2c1, DEV_ADDR, Out, 2, 1000);
      refresh = 1;
    }

    //?????????????? ??????????????
    if (program == PROGRAM_CLOCK && mode == 2
        && (button_up_pressed || button_up_repeated || button_down_pressed || button_down_repeated))
    {
      if (button_up_pressed || button_up_repeated) {
        signal_index = MIN(signal_index + 1, SIGNAL_STEPS - 1);
      }
      if (button_down_pressed || button_down_repeated) {
        signal_index = MAX(signal_index - 1, 0);
      }
      mode_back = 1;
      refresh = 1;
    }

    //view
    if (freeze > 0) freeze--;

    if (!freeze || refresh) {
      if (subsecond_changed) {
        if (program == PROGRAM_CLOCK
            && mode >= 3
            && subsecond == 5)
        {
          show_clock(Time);
          p10_putrect((5 - mode) * 23, 0, 18, 16, 0);
          p10_flip();
        }
      }

      if (second_changed || refresh) {
        if (program == PROGRAM_CLOCK && (mode == 0 || mode >= 3)) {
          show_clock(Time);
          p10_flip();
        }
      }

      if (refresh && program != PROGRAM_CLOCK && mode == 2)
      {
        p10_putscreen(p10_brightness);
        p10_putint(64, brightness[brightness_index] + 1, 0);
        p10_flip();
      }

      //??????????
      if (refresh && mode == 1) {
        switch (program) {
        case PROGRAM_CLOCK: p10_putscreen(p10_program_clock); break;
        case PROGRAM_STOPWATCH: p10_putscreen(p10_program_stopwatch); break;
        case PROGRAM_TIMER3: p10_putscreen(p10_program_timer3); break;
        }
        p10_flip();
      }

      //????????????????
      if (refresh && program == PROGRAM_CLOCK && mode == 2) {
        p10_putscreen(p10_signal);
        if (signals[signal_index] != 0) {
          p10_putrect(33, 0, 32, 16, 0);
          p10_putnumber(33, signals[signal_index] / 60 % 10, 0);
          p10_putnumber(46, signals[signal_index] % 60 / 10, 0);
          p10_putnumber(56, signals[signal_index] % 10, 0);
          p10_putrect(42, 4, 3, 2, 1);
          p10_putrect(42, 10, 3, 2, 1);
        }
        p10_flip();
      }

      if (refresh && program == PROGRAM_STOPWATCH && mode == 0) {
        show_stopwatch(sw_time);
        p10_flip();
      }

      //view ????????????
      if (refresh && program == PROGRAM_TIMER3 && mode == 0) {
        show_timer3(t3_time);
        p10_flip();
      }
    }

  }
}

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
  MX_I2C1_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
//  ST7789_Init();

  p10_init(&hspi2);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);

  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, brightness[brightness_index]);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//  main_loop_stopwatch();
//  main_loop_clock();
//  main_loop_sound();
  main_loop_v2();

  while (0)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 399;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 99;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
  sConfigOC.Pulse = 10;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 79;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
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
  sConfigOC.Pulse = 49;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ST7789_DC_Pin|P10_A_Pin|P10_B_Pin|P10_L_Pin
                          |P10_OE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ST7789_RST_Pin|LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED1_Pin */
  GPIO_InitStruct.Pin = LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7789_DC_Pin P10_A_Pin P10_B_Pin P10_L_Pin
                           P10_OE_Pin */
  GPIO_InitStruct.Pin = ST7789_DC_Pin|P10_A_Pin|P10_B_Pin|P10_L_Pin
                          |P10_OE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ST7789_RST_Pin LED2_Pin */
  GPIO_InitStruct.Pin = ST7789_RST_Pin|LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUT_MODE_Pin BUT_DOWN_Pin BUT_UP_Pin */
  GPIO_InitStruct.Pin = BUT_MODE_Pin|BUT_DOWN_Pin|BUT_UP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void Test_P10() {
//  uint8_t buf[16] = {0,0,0,0,255,255,255,255,0,0,0,0,255,255,255,255};
  uint8_t buf[] = {0x00, 0xF0,0xFF,0x0F,0xFF, 255,255,255,255, 255,255,255,255, 255,255,255,0xEF};

  HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_RESET);
  HAL_Delay(1000);

  while(1) {
    HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);

    for (uint8_t a = 0; a <= 1; a++) {
      for (uint8_t b = 0; b <= 1; b++) {
        memset(buf, 0xFF, 16);
        buf[14] = 0xFD;
        buf[15] = 0xFE;
        HAL_SPI_Transmit(&hspi2, buf, 16, 100);
        HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(P10_A_GPIO_Port, P10_A_Pin, a);
        HAL_GPIO_WritePin(P10_B_GPIO_Port, P10_B_Pin, b);

        HAL_GPIO_WritePin(P10_L_GPIO_Port, P10_L_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(P10_L_GPIO_Port, P10_L_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_SET);

        HAL_Delay(1);
        HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_RESET);
        HAL_Delay(2);
      }
    }
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
