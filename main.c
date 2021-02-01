/********************************************************************************
 * nrf24l01-ble/main.c
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

#include <avr/io.h>
#include <util/delay.h>
#include "nrf24l01_btle.h"
#include "oled.h"
#include "at85_i2c.h"
extern uint8_t current;

extern struct btle_adv_pdu buffer;

int main(void)
{
  nrf24_cfg();

  const char* lcdbuffer = &buffer;
  oled_init();
  oled_clear();
  strcpy(lcdbuffer,"name: ");
  strncat(lcdbuffer,ADVE_NAME,strlen(ADVE_NAME));
  oled_p8x16str(0,0,lcdbuffer);
  memset(lcdbuffer,0,16);
  sprintf(lcdbuffer,"mac:%02X%02X%02X%02X%02X%02X",
          MAC[5],MAC[4],MAC[3],MAC[2],MAC[1],MAC[0]);
  oled_p8x16str(0,2,lcdbuffer);
  memset(lcdbuffer,0,16);

  while (1)
  {
    if(current++ == 2)
    {
       current = 0;
    }
    nrf24_adv();
    _delay_ms(1000);
  }

  return 1;
}
