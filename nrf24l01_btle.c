/********************************************************************************
 * nrf24l01-ble/nrf24l01_btle.c
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

#include "nrf24l01_btle.h"
#include "at85_spi.h"
#include "at85_i2c.h"
#include "dht11.h"
#include "oled.h"

#define CONFIG_NRF24L01_BTLE_DHT11 1

/* logical BTLE channel number (37-39) */

const uint8_t channel[3] =
  {
    37, 38, 39
  };

/* physical frequency (2400+x MHz)  */

const uint8_t frequency[3] =
  {
    2, 26, 80
  };


struct btle_adv_pdu buffer;

uint8_t current = 0;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

uint8_t swapbits(uint8_t a)
{
  /* reverse the bit order in a single byte */

  uint8_t v = 0;
  if (a & 0x80) v |= 0x01;
  if (a & 0x40) v |= 0x02;
  if (a & 0x20) v |= 0x04;
  if (a & 0x10) v |= 0x08;
  if (a & 0x08) v |= 0x10;
  if (a & 0x04) v |= 0x20;
  if (a & 0x02) v |= 0x40;
  if (a & 0x01) v |= 0x80;
  return v;
}

/* see BT Core Spec 4.0, Section 6.B.3.2 */

static inline void whiten(uint8_t len)
{
  uint8_t i;
  uint8_t * buf = (uint8_t *)&buffer;

  /* initialize LFSR with current channel, set bit 6 */

  uint8_t lfsr = channel[current] | 0x40;

  while (len--)
    {
      uint8_t res = 0;

      /* LFSR in "wire bit order" */

      for (i = 1; i; i <<= 1)
        {
          if (lfsr & 0x01)
            {
              lfsr ^= 0x88;
              res |= i;
            }

          lfsr >>= 1;
        }

      *(buf++) ^= res;
    }
}

/* see BT Core Spec 4.0, Section 6.B.3.1.1 */

static inline void crc(uint8_t len, uint8_t * dst)
{
  uint8_t i;
  uint8_t * buf = (uint8_t *)&buffer;

  /**
   * initialize 24-bit shift register in "wire bit order"
   * dst[0] = bits 23-16, dst[1] = bits 15-8, dst[2] = bits 7-0.
   **/

  dst[0] = 0xaa;
  dst[1] = 0xaa;
  dst[2] = 0xaa;

  while (len--)
    {
      uint8_t d = *(buf++);
      for (i = 1; i; i <<= 1, d >>= 1)
        {
          /**
           * save bit 23 (highest-value),
           * left-shift the entire register by one
           **/

          uint8_t t = dst[0] & 0x01;         dst[0] >>= 1;
          if (dst[1] & 0x01) dst[0] |= 0x80; dst[1] >>= 1;
          if (dst[2] & 0x01) dst[1] |= 0x80; dst[2] >>= 1;

          /**
           * if the bit just shifted out (former bit 23) and the incoming
           * data bit are not equal (i.e. bit_out ^ bit_in == 1) => toggle
           * tap bits
           */

          if (t != (d & 1))
            {
              /**
               * toggle register tap bits (=XOR with 1)
               * according to CRC polynom
               **/

              /* 0b11011010 inv. = 0b01011011 ^= x^6+x^4+x^3+x+1 */

              dst[2] ^= 0xda;

              /* 0b01100000 inv. = 0b00000110 ^= x^10+x^9 */

              dst[1] ^= 0x60;
            }
        }
    }
}

/* change buffer contents to "wire bit order" */

static inline void swapbuf(uint8_t len)
{
  uint8_t * buf = (uint8_t *)&buffer;
  while (len--)
    {
      uint8_t a = *buf;
      *(buf++) = swapbits(a);
    }
}

int nrf24_cfg()
{
  int error = 0;
  spi_setup();

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_CONFIG , NRF24L01_PWR_UP); /* on, no crc, int on RX/TX done */
  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_EN_AA , 0x0); /* no auto-acknowledge */

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_EN_RXADDR , 0x00); /* no RX , TX only */

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_SETUP_AW , 0x2); /* 4-byte address */

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_SETUP_RETR , 0x0); /* no auto-retransmit */

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_RF_CH , 2);

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_STATUS , 0x3e);

  /* setup rate */

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_RF_SETUP , 3 << NRF24L01_RF_PWR_SHIFT);

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_DYNPD , 0x00); /* no dynamic payloads */

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_FEATURE , 0); /* set features */

  /**************************************************************************
   * Define the pipe #0 parameters (AA enabled and dynamic payload length).
   * 4 byte of access address, which is always 0x8E89BED6 for advertizing
   * packets.
   *
   **************************************************************************/
   uint8_t *outbuf = (uint8_t *)&buffer;
   outbuf[0] = NRF24L01_W_REGISTER | NRF24L01_TX_ADDR;
   outbuf[1] = swapbits(0x8e);
   outbuf[2] = swapbits(0x89);
   outbuf[3] = swapbits(0xbe);
   outbuf[4] = swapbits(0xd6);

   spi_sendblock(outbuf,5);

  spi_sendcmd( NRF24L01_W_REGISTER | NRF24L01_RX_PW_P0 , 0x20); /* always RX 32 bytes */
  return error;
}

void nrf24_adv()
{
  uint8_t pls = 0;
  uint8_t i;
  uint8_t namelen;
  uint8_t *lcddata = &buffer;
  uint8_t data[5] = {0};
  namelen = sizeof(ADVE_NAME);
#ifdef CONFIG_NRF24L01_BTLE_DHT11

  struct nrf_service_data *temp;
  struct nrf_service_data *hum;
  dht11_read(&data);
#endif
  i2c_setup();
  memset(lcddata,0,16);
  sprintf(lcddata,"freq:%02d ch:%2d",frequency[current],channel[current]);
  oled_p8x16str(0,4,lcddata);
  memset(lcddata,0,16);
  sprintf(lcddata,"temp:%d humi:%d",data[2],data[0]);
  oled_p8x16str(0,6,lcddata);
  memset(lcddata,0,16);
  memcpy(&buffer.mac[0], &MAC[0], 6);

  /* add device descriptor chunk */

  chunk(buffer, pls)->size = 0x02;  /* chunk size: 2 */
  chunk(buffer, pls)->type = 0x01;  /* chunk type: device flags */

  /* flags: LE-only, limited discovery mode */

  chunk(buffer, pls)->data[0] = 0x06;
  pls += 3;

  /* add device name chunk */

  chunk(buffer, pls)->size = namelen + 1;
  chunk(buffer, pls)->type = 0x09;
  for (i = 0; i < namelen; i++)
    chunk(buffer, pls)->data[i] = ADVE_NAME[i];
  pls += namelen + 2;

  /* add custom data, if applicable */

#ifdef CONFIG_NRF24L01_BTLE_DHT11

  /* set temperature */

  chunk(buffer, pls)->size = 3 + 1;  /* chunk size */

  /* chunk type */

  chunk(buffer, pls)->type = 0x16;
  temp  = chunk(buffer, pls)->data;
  temp->service_uuid = NRF_TEMPERATURE_SERVICE_UUID;
  temp->value = data[2];
  pls += 3 + 2;

  /* set humidity  */

  chunk(buffer, pls)->size = 3 + 1;
  chunk(buffer, pls)->type = 0x16;
  hum  = chunk(buffer, pls)->data;
  hum->service_uuid = NRF_ENVIRONMENTAL_SERVICE_UUID;
  hum->value = data[0];
  pls += 3 + 2;
#else
  chunk(buffer, pls)->size = 4 + 1;
  chunk(buffer, pls)->type = 0xff; /* custom data */
  chunk(buffer, pls)->data[0] = 't';
  chunk(buffer, pls)->data[1] = 'e';
  chunk(buffer, pls)->data[2] = 's';
  chunk(buffer, pls)->data[3] = 't';
  pls += 4 + 2;
#endif
  buffer.payload[pls] = 0x55;
  buffer.payload[pls + 1] = 0x55;
  buffer.payload[pls + 2] = 0x55;

  /**************************************************************************
   * The Payload field consists of AdvA and AdvData fields.
   * The AdvA field shall  contain the advertiser’s public or
   * random device address as indicated by TxAdd.
   * -----------------------------------------------------------
   * |   PDU  |   Type  |  RFU   | TxAdd  | RxAdd   |Length RFU|
   * |--------+---------+--------+--------+---------+----------|
   * |(4 bits)| (2 bits)| (1 bit)| (1 bit)| (6 bits)| (2 bits) |
   * -----------------------------------------------------------
   * 0x42 = 0b1000010; include ADV_NONCONN_IND and TxAdd.
   *
   **************************************************************************/

  buffer.pdu_type = 0x42;

  /* set final payload size in header include MAC length */

  buffer.pl_size = pls + 6;

  uint8_t * outbuf = (uint8_t *)&buffer;

  crc(pls + 8, outbuf + pls + 8);
  whiten(pls + 11);
  swapbuf(pls + 11);

  /* set frequency */
  _delay_us(50);
  spi_setup();

  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_RF_CH , frequency[current]);
  spi_onecmd(NRF24L01_FLUSH_TX);  /* Clear TX Fifo */
  spi_sendcmd(NRF24L01_W_REGISTER | NRF24L01_STATUS , 0x6E); /* clear flags */

  /* send packet */

  spi_dosend(NRF24L01_W_TX_PAYLOAD,outbuf,pls + 11);

  /* Standby modes to TX/RX mode , need more than 130 us */

  _delay_us(150);
}
