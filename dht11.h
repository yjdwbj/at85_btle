/********************************************************************************
 * nrf24l01-ble/dht11.h
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

#ifndef __DTH11_H
#define __DTH11_H
#include <avr/io.h>
#include <stdbool.h>

#define  DHT_PIN     PB4
#define  DHT_PORT    PINB

#define DHT_PIN_INPUT()    (DDRB  &= ~_BV(DHT_PIN))
#define DHT_PIN_OUTPUT()   (DDRB  |= _BV(DHT_PIN))
#define DHT_PIN_LOW()      (PORTB &= ~_BV(DHT_PIN))
#define DHT_PIN_HIGH()     (PORTB |= _BV(DHT_PIN))
#define DHT_PIN_READ()     (PINB  &  _BV(DHT_PIN))
#define IS_SET(bit)   (((PINB) & (1UL << (bit))) >> (bit))
#define DHT_TIMEOUT        (30)

bool dht11_read(uint8_t *buf);
#endif /* __DTH11_H */