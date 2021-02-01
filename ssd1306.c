/********************************************************************************
 * nrf24l01-ble/ssd1306.c
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

#include "ssd1306.h"
#include <avr/pgmspace.h>

const uint8_t ssd1306_init_sequence[] PROGMEM = {

  /*  Display OFF (sleep mode) */

  SSD1306_DISPLAYOFF,

  /**
   *  Set Memory Addressing Mode
	 *		 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
	 *		 10=Page Addressing Mode (RESET); 11=Invalid
   */

  SSD1306_MEMORYMODE,
  SSD1306_SETMEMORYMODE_PAGE,

  /* set low column address */
  SSD1306_SETLOWCOLUMN,

  /* set high column address */
  SSD1306_SETHIGHCOLUMN,

  /* set start line address */

  SSD1306_SETSTARTLINE,

  /* Set contrast control register */
  SSD1306_SETCONTRAST, 0x7F,

  /*  Set Segment Re-map. A0=address mapped; A1=address 127 mapped.  */
  SSD1306_SEGREMAP+1,

  /* Set COM Output Scan Direction */
  SSD1306_COMSCANDEC,
  // SSD1306_COMSCANINC,

  /* Set multiplex ratio(1 to 64) */
  SSD1306_SETMULTIPLEX, 0x3F,

  /* Set display offset. 00 = no offset */
  SSD1306_SETDISPLAYOFFSET, 0x00,

  /* set display clock divide ratio/oscillator frequency */
  SSD1306_SETDISPLAYCLOCKDIV, 0x80,

  /* Set pre-charge period */

  SSD1306_SETPRECHARGE, 0x22,

  /* Set com pins hardware configuration		 */
  SSD1306_SETCOMPINS, 0x12,

  /* set vcomh, 0x20, 0.77xVcc */
  SSD1306_SETVCOMDETECT, 0x20,

  /* Set Page Addressing Mode (0x00/0x01/0x02) */

  SSD1306_MEMORYMODE, 0x02,

  /* Set DC-DC enable */

  SSD1306_CHARGEPUMP,0x14,
  /**
   * Output RAM to Display
   *    0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
   */
  SSD1306_DISPLAYALLON_RESUME,

  /*  Disable Inverse Display On (0xa6/a7)  */
  SSD1306_NORMALDISPLAY,

  /* Display ON in normal mode */
  SSD1306_DISPLAYON
};

#define ssd1306_datastop() i2c_stop()

static void init(void)
{
  uint8_t i;
  i2c_start(SSD1306_ADDRESS,USI_WRITE);
  i2c_write(0x00);
  for(i=0; i < sizeof(ssd1306_init_sequence);i++)
    {
      ssd1306_sendcmd(pgm_read_byte(&ssd1306_init_sequence[i]));
    }
  i2c_stop();
}

void ssd1306_init(void)
{
    i2c_setup();

    /*
     * With 6pins I2C/SPI 0.96 OLED that need to try many times init() if use
     * low level reset circuit to generate a short pulse for the reset (RST)pin
     */

    _delay_ms(10);
    init();
    _delay_ms(10);
    init();
    _delay_ms(10);
    init();
    _delay_ms(10);
    init();
    _delay_ms(10);
    init();
    _delay_ms(10);
    init();
}

static void ssd1306_datastart()
{
  i2c_start(SSD1306_ADDRESS,USI_WRITE);
  i2c_write(0x40);
}

static void ssd1306_sendcmd(uint8_t cmd)
{

#if defined(SOFT_IIC)
  WRITE_CMD();
#else
  i2c_start(SSD1306_ADDRESS,USI_WRITE);
  i2c_write(0x80);
#endif
  i2c_write(cmd);
  i2c_stop();
}

void ssd1306_senddata(uint8_t data)
{
#if defined(SOFT_IIC)
  WRITE_DATA();
#endif
  i2c_write(data);
}

void ssd1306_fillscreen(uint8_t data)
{
  uint8_t y,x;

	for(y = 0; y < 8; y++)
	{
		ssd1306_sendcmd(SSD1306_SETPAGESTARTADDRPAM+y);
		ssd1306_sendcmd(0x0);
		ssd1306_sendcmd(SSD1306_SETHIGHCOLUMN);
#ifndef SOFT_IIC
    ssd1306_datastart();
#endif
    for(x = 0; x < SSD1306_WIDTH; x++)
    {
      ssd1306_senddata(data);
    }
#ifndef SOFT_IIC
    ssd1306_datastop();
#endif
	}
}

void ssd1306_setpos(uint8_t x,uint8_t y)
{
    ssd1306_sendcmd(SSD1306_SETPAGESTARTADDRPAM + y);
    ssd1306_sendcmd(SSD1306_SETHIGHCOLUMN |(( x & 0xf0) >> 4));
    ssd1306_sendcmd(0x1 | (x & 0x0f));
}

void ssd1306_clear(void)
{
  ssd1306_fillscreen(0);
}
