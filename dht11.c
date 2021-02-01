/********************************************************************************
 * nrf24l01-ble/dht11.c
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

#include "dht11.h"
#include <util/delay.h>

static uint8_t read_byte(void)
{
    uint8_t i,temp =0,cnt;
    for(i = 0; i < 8; i ++)
    {
        TCNT0 = 0;

        /* First there is always a 50µs low period */
        while(!IS_SET(DHT_PIN));
        {
            if(TCNT0 > 70)
                return 0;
        }
        TCNT0 = 0;

        /* Then the data signal is sent. 26 to 28µs (ideally)
         indicate a low bit, and around 70µs a high bit */

        while(IS_SET(DHT_PIN))
        {
            if (TCNT0 >= 90)
             return 0;
        }

        cnt = TCNT0;
        temp <<= 1;
        if(cnt >= 60 && cnt <= 80)
        {
            temp |= 1;
        }
    }
    return temp;
}

bool dht11_read(uint8_t *buf)
{
    uint8_t sum;
    DHT_PIN_OUTPUT();

    /* First we need milliseconds delay, so set clk/1024 prescaler */
    TCCR0B = 0x5;
    TCNT0 = 0;
    DHT_PIN_LOW();

    /* Wait about 20 ms */
    while(TCNT0 < 160);

    /* Now set Timer0 with clk/8 prescaling. Gives 1µs per cycle @8Mhz */
    TCCR0B = 0x02;

    TCNT0 = 0;

    DHT_PIN_HIGH();

    DHT_PIN_INPUT();

    /* Wait for response from sensor, 20-40µs according to datasheet */
    while(IS_SET(DHT_PIN))
    {
        if (TCNT0 >= 60)
            return 0;
    }

    TCNT0 = 0;

    /* Now wait for the first response to finish, low ~80µs */
    while(!IS_SET(DHT_PIN))
    {
        if (TCNT0 >= 100)
            return 0;
    }

    TCNT0 = 0;

    /* Then wait for the second response to finish, high ~80µs */
    while(IS_SET(DHT_PIN))
    {
        if (TCNT0 >= 100)
            return 0;
    }


    buf[0] = read_byte();
    buf[1] = read_byte();
    buf[2] = read_byte();
    buf[3] = read_byte();
    buf[4] = read_byte();

    /* verify checksum */

    sum = (buf[0] + buf[1] + buf[2] + buf[3]) & 0xff;
    return (sum == buf[4]);
}