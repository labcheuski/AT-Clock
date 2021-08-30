/*
 * p10.c
 *
 *  Created on: Jun 30, 2020
 *      Author: Albatros
 */

#include "main.h"
#include "p10.h"
#include "string.h"

uint8_t p10_buf1[P10_BUF_SIZE];
uint8_t p10_buf2[P10_BUF_SIZE];
uint8_t *p10_buf = &p10_buf1[0];
uint8_t *p10_buf_live = &p10_buf2[0];
uint8_t p10_page = 0;
SPI_HandleTypeDef *p10_hspi;

uint8_t font8x16numbers[10][16] = {
  {
    0b11000011,
    0b10000001,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00011000,
    0b10000001,
    0b11000011
  }, {
    0b11110001,
    0b11100001,
    0b11000001,
    0b10001001,
    0b10011001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001,
    0b11111001
  }, {
    0b11000011,
    0b10000001,
    0b00011000,
    0b00111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111000,
    0b11110001,
    0b11100011,
    0b11000111,
    0b10001111,
    0b00011111,
    0b00111111,
    0b00000000,
    0b00000000
  }, {
    0b00000000,
    0b00000000,
    0b11111000,
    0b11110001,
    0b11100011,
    0b11000111,
    0b11000011,
    0b11000001,
    0b11111000,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b00011000,
    0b10000001,
    0b11000011
  }, {
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00000000,
    0b10000000,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100
  }, {
    0b00000000,
    0b00000000,
    0b00111111,
    0b00111111,
    0b00111111,
    0b00111111,
    0b00000011,
    0b00000001,
    0b11111000,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b00011000,
    0b10000001,
    0b11000011
  }, {
    0b11100001,
    0b11000001,
    0b10001111,
    0b00011111,
    0b00111111,
    0b00111111,
    0b00000011,
    0b00000001,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00011000,
    0b10000001,
    0b11000011
  }, {
    0b00000000,
    0b00000000,
    0b11111100,
    0b11111100,
    0b11111000,
    0b11110001,
    0b11100011,
    0b11000111,
    0b11001111,
    0b11001111,
    0b11001111,
    0b11001111,
    0b11001111,
    0b11001111,
    0b11001111,
    0b11001111
  }, {
    0b11000011,
    0b10000001,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00011000,
    0b10000001,
    0b10000001,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00011000,
    0b10000001,
    0b11000011
  }, {
    0b11000011,
    0b10000001,
    0b00011000,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00111100,
    0b00011000,
    0b10000000,
    0b11000000,
    0b11111100,
    0b11111100,
    0b11111000,
    0b11110001,
    0b10000011,
    0b10000111
  }
};

int xy2addr(int x, int y) {
  y = P10_HEIGHT - 1 - y;
  return ((y & 0b11) * P10_BUF_PAGE_SIZE) + (x >> 3 << 2) + (y >> 2);
}

int x2shift(int x) {
  return 7 - (x & 0b111);
}

int in_domain(int x, int y) {
  return ((0 <= x) && (x < P10_WIDTH) && (0 <= y) && (y < P10_HEIGHT));
}

void p10_flip() {
  uint8_t *buf;
  buf = p10_buf_live;
  p10_buf_live = p10_buf;
  p10_buf = buf;
}

void p10_setxy(int x, int y, uint8_t value) {
  if (!in_domain(x, y)) return;
  if (!value) {
    p10_buf[xy2addr(x, y)] |= 1 << x2shift(x);
  } else {
    p10_buf[xy2addr(x, y)] &= ~(1 << x2shift(x));
  }
}

uint8_t p10_getxy(int x, int y) {
  if (!in_domain(x, y)) return 0;
  return ~ (p10_buf[xy2addr(x, y)] >> x2shift(x)) & 1;
}

void p10_init(SPI_HandleTypeDef * phspi) {
  p10_hspi = phspi;
  p10_clrscr();
  p10_flip();
//  p10_buf[ 0]  = ~0b00111100;
//  p10_buf[32]  = ~0b01111110;
//  p10_buf[64]  = ~0b11100111;
//  p10_buf[96]  = ~0b11000011;
//  p10_buf[ 1]  = ~0b11000011;
//  p10_buf[33]  = ~0b11000011;
//  p10_buf[65]  = ~0b11000011;
//  p10_buf[97]  = ~0b11000011;
//  p10_buf[ 2]  = ~0b11000011;
//  p10_buf[34]  = ~0b11000011;
//  p10_buf[66]  = ~0b11000011;
//  p10_buf[98]  = ~0b11000011;
//  p10_buf[ 3]  = ~0b11000011;
//  p10_buf[35]  = ~0b11100111;
//  p10_buf[67]  = ~0b01111110;
//  p10_buf[99]  = ~0b00111100;
}

void p10_tick() {
  //Выключаем светодиоды
  HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_RESET);

  //Передаём следующую страницу
  p10_page += 1;
  if (p10_page >= 4) p10_page = 0;
  HAL_SPI_Transmit(p10_hspi, &p10_buf_live[p10_page * P10_BUF_PAGE_SIZE], P10_BUF_PAGE_SIZE, 100);

  //Устанавливем адрес переданой страницы
  HAL_GPIO_WritePin(P10_A_GPIO_Port, P10_A_Pin, (~p10_page & 1));
  HAL_GPIO_WritePin(P10_B_GPIO_Port, P10_B_Pin, (~p10_page >> 1 & 1));

  //Импульс на фиксацию данных
  HAL_GPIO_WritePin(P10_L_GPIO_Port, P10_L_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(P10_L_GPIO_Port, P10_L_Pin, GPIO_PIN_RESET);

  //Зажигаем светодиоды
  HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
}

void p10_tock() {
  //Выключаем светодиоды
  HAL_GPIO_WritePin(P10_OE_GPIO_Port, P10_OE_Pin, GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
}

void p10_putnumber(int x, uint8_t n, uint8_t invers) {
  invers = invers ? 0xFF : 0x00;
  for (int y = 0; y < 16; y++) {
    int a = xy2addr(x, y);
    int s = 7 - x2shift(x);

    if (s == 0) {
      if (in_domain(x, y)) {
        p10_buf[a] = invers ^ font8x16numbers[n][y];
      }
    } else {
      if (in_domain(x, y)) {
        p10_buf[a] &= ~ (0xFF >> s);
        p10_buf[a] |= (invers ^ font8x16numbers[n][y]) >> s;
      }
      if (in_domain(x + 8, y)) {
        a += 4;
        s = 8 - s;
        p10_buf[a] &= ~ (0xFF << s);
        p10_buf[a] |= (invers ^ font8x16numbers[n][y]) << s;
      }
    }
  }
}

void p10_putint(int x, int value, uint8_t invers) {
  for (int i = 1; i <= 6; i++) {
    p10_putnumber(x - i*10 + 2, value % 10, invers);
    value = value / 10;
    if (value == 0) break;
  }
}

void p10_putscreen(uint8_t * screen) {
  for (int y = 0; y < P10_HEIGHT; y++) {
    for (int x = 0; x < P10_WIDTH / 8; x++) {
      p10_buf[xy2addr(x * 8, y)] = ~ screen[y * P10_WIDTH/8 + x];
    }
  }
}

void p10_putrect(int x, int y, int w, int h, uint8_t v) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      p10_setxy(ix, iy, v);
    }
  }
}

void p10_clrscr() {
  memset(p10_buf, 0xFF, sizeof(p10_buf1));
}

void p10_invertrect(int x, int y, int w, int h) {
  for (int ix = x; ix < x + w; ix++) {
    for (int iy = y; iy < y + h; iy++) {
      p10_setxy(ix, iy, !p10_getxy(ix, iy));
    }
  }
}