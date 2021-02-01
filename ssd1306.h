/********************************************************************************
 * nrf24l01-ble/ssd1306.h
 *
 * This file is part of the at85_btle distribution.
 *  (https://github.com/yjdwbj/at85_btle).
 * Copyright (c) 2021 Liu Chun Yang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ********************************************************************************/

#ifndef __SSD1306_H
#define __SSD1306_H

#include "at85_i2c.h"

#define SSD1306_ADDRESS                        0x3c

#define SSD1306_SETCONTRAST                    0x81
#define SSD1306_DISPLAYALLON_RESUME            0xA4
#define SSD1306_DISPLAYALLON                   0xA5
#define SSD1306_NORMALDISPLAY                  0xA6
#define SSD1306_INVERTDISPLAY                  0xA7
#define SSD1306_DISPLAYOFF                     0xAE
#define SSD1306_DISPLAYON                      0xAF
#define SSD1306_SETPAGESTARTADDRPAM            0xB0
#define SSD1306_SETDISPLAYOFFSET               0xD3
#define SSD1306_SETCOMPINS                     0xDA
#define SSD1306_SETVCOMDETECT                  0xDB
#define SSD1306_SETDISPLAYCLOCKDIV             0xD5
#define SSD1306_SETPRECHARGE                   0xD9
#define SSD1306_SETMULTIPLEX                   0xA8
#define SSD1306_SETLOWCOLUMN                   0x00
#define SSD1306_SETHIGHCOLUMN                  0x10
#define SSD1306_SETSTARTLINE                   0x40
#define SSD1306_MEMORYMODE                     0x20
#define SSD1306_SETMEMORYMODE_HORIZONTAL       0x00
#define SSD1306_SETMEMORYMODE_VERTICAL         0x01
#define SSD1306_SETMEMORYMODE_PAGE             0x02

#define SSD1306_COLUMNADDR                     0x21
#define SSD1306_PAGEADDR                       0x22
#define SSD1306_COMSCANINC                     0xC0
#define SSD1306_COMSCANDEC                     0xC8
#define SSD1306_SEGREMAP                       0xA0
#define SSD1306_CHARGEPUMP                     0x8D
#define SSD1306_SWITCHCAPVCC                   0x02
#define SSD1306_NOP                            0xE3
#define SH1106_SETPUMPVOLTAGEVALE              0x30

#define SSD1306_WIDTH                          128
#define SSD1306_HEIGHT                          64

void ssd1306_init(void);
static void ssd1306_sendcmd(uint8_t cmd);
void ssd1306_senddata(uint8_t cmd);
void ssd1306_setpos(uint8_t x,uint8_t y);
void ssd1306_clear(void);
void ssd1306_fillscreen(uint8_t);

#endif /* __SSD1306_H */
