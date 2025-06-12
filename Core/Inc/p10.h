/*
 * p10.h
 *
 *  Created on: Jun 30, 2020
 *      Author: Albatros
 */

#ifndef INC_P10_H_
#define INC_P10_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define P10_WIDTH 64
#define P10_HEIGHT 16
#define P10_BUF_SIZE (P10_WIDTH * P10_HEIGHT / 8)
#define P10_BUF_PAGE_SIZE (P10_BUF_SIZE / 4)

void p10_flip();
void p10_setxy(int x, int y, uint8_t value);
uint8_t p10_getxy(int x, int y);
void p10_init(SPI_HandleTypeDef * phspi);
void p10_tick();
void p10_tock();
void p10_putnumber(int x, uint8_t n, uint8_t invers);
void p10_putnumber4x7(int x, int y, uint8_t n, uint8_t invers);
void p10_putint(int x, int value, uint8_t invers);
int p10_intwidth(int value);
void p10_putint4x7(int x, int y, int value, uint8_t invers);
void p10_putrect(int x, int y, int w, int h, uint8_t v);
void p10_invertrect(int x, int y, int w, int h);
void p10_putscreen(uint8_t * screen);
void p10_clrscr();
void p10_putsprite(int x, int y, int w, int h, uint8_t *sprite, uint8_t invers);

#endif /* INC_P10_H_ */
