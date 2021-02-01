/********************************************************************************
 * nrf24l01-ble/nrf24l01_btle.h
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

#ifndef __NRF24L01_BTLE_H
#define __NRF24L01_BTLE_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/*************************************
 *    ATtiny85         NRF24l01+ mini
 *  PB0 (MOSI)   <--->    pin7 (MISO)
 *  PB1 (MISO)   <--->    pin6 (MOSI)
 *  PB2 (SCK)    <--->    pin5 (SCK)
 *  PB3 (CSN)    <--->    pin4 (CSN)
 *   +3.3v       <--->    pin3 (CE)
 *
 *************************************/

/* nRF24L01 hardware definitions */

#define ADVE_NAME "nRF24"
#define MAC       "yjdwbj"

/* Commands */

#define NRF24L01_R_REGISTER             0x00
#define NRF24L01_W_REGISTER             0x20
#define NRF24L01_R_RX_PAYLOAD           0x61
#define NRF24L01_W_TX_PAYLOAD           0xA0
#define NRF24L01_FLUSH_TX               0xE1
#define NRF24L01_FLUSH_RX               0xE2
#define NRF24L01_REUSE_TX_PL            0xE3

#define NRF24L01_ACTIVATE               0x50
#define NRF24L01_R_RX_PL_WID            0x60
#define NRF24L01_W_TX_PAYLOAD_NOACK     0xB0
#define NRF24L01_W_ACK_PAYLOAD          0xA8
#define NRF24L01_NOP                    0xFF

/* Registers */

#define NRF24L01_CONFIG         0x00
#define NRF24L01_EN_AA          0x01
#define NRF24L01_EN_RXADDR      0x02
#define NRF24L01_SETUP_AW       0x03
#define NRF24L01_SETUP_RETR     0x04
#define NRF24L01_RF_CH          0x05
#define NRF24L01_RF_SETUP       0x06
#define NRF24L01_STATUS         0x07
#define NRF24L01_OBSERVE_TX     0x08
#define NRF24L01_CD             0x09
#define NRF24L01_RX_ADDR_P0     0x0A
#define NRF24L01_RX_ADDR_P1     0x0B
#define NRF24L01_RX_ADDR_P2     0x0C
#define NRF24L01_RX_ADDR_P3     0x0D
#define NRF24L01_RX_ADDR_P4     0x0E
#define NRF24L01_RX_ADDR_P5     0x0F
#define NRF24L01_TX_ADDR        0x10
#define NRF24L01_RX_PW_P0       0x11
#define NRF24L01_RX_PW_P1       0x12
#define NRF24L01_RX_PW_P2       0x13
#define NRF24L01_RX_PW_P3       0x14
#define NRF24L01_RX_PW_P4       0x15
#define NRF24L01_RX_PW_P5       0x16
#define NRF24L01_FIFO_STATUS    0x17
#define NRF24L01_DYNPD          0x1C
#define NRF24L01_FEATURE        0x1D

/* STATUS register definitions */

#define NRF24L01_RX_DR          (1 << 6)
#define NRF24L01_TX_DS          (1 << 5)
#define NRF24L01_MAX_RT         (1 << 4)
#define NRF24L01_RX_P_NO_SHIFT  1
#define NRF24L01_RX_P_NO_MASK   (7 << NRF24L01_RX_P_NO_SHIFT)
#define NRF24L01_STAT_TX_FULL   (1 << 0)

/* CONFIG register definitions */

#define NRF24L01_MASK_RX_DR     (1 << 6)
#define NRF24L01_MASK_TX_DS     (1 << 5)
#define NRF24L01_MASK_MAX_RT    (1 << 4)
#define NRF24L01_EN_CRC         (1 << 3)
#define NRF24L01_CRCO           (1 << 2)
#define NRF24L01_PWR_UP         (1 << 1)
#define NRF24L01_PRIM_RX        (1 << 0)

/* RF_SETUP register definition */

#define NRF24L01_CONT_WAVE      (1 << 7)
#define NRF24L01_RF_DR_LOW      (1 << 5)
#define NRF24L01_PLL_LOCK       (1 << 4)
#define NRF24L01_RF_DR_HIGH     (1 << 3)

#define NRF24L01_RF_PWR_SHIFT   1
#define NRF24L01_RF_PWR_MASK    (3 << NRF24L01_RF_PWR_SHIFT)

/* FIFO STATUS register definitions */

#define NRF24L01_TX_REUSE       (1 << 6)
#define NRF24L01_TX_FULL        (1 << 5)
#define NRF24L01_TX_EMPTY       (1 << 4)
#define NRF24L01_RX_FULL        (1 << 1)
#define NRF24L01_RX_EMPTY       (1 << 0)

/* FEATURE register definitions */

#define NRF24L01_EN_DPL          (1 << 2)
#define NRF24L01_EN_ACK_PAY      (1 << 1)
#define NRF24L01_EN_DYN_ACK      (1 << 0)

/* Service UUIDs used on the nRF8001 and nRF51822 platforms */

#define NRF_TEMPERATURE_SERVICE_UUID		    0x1809
#define NRF_ENVIRONMENTAL_SERVICE_UUID      0x181A

/* Data rates available for transmission */

typedef enum
{
  RATE_1Mbps,
  RATE_2Mbps,
  RATE_250kbps
} nrf24l01_datarate_t;

/* helper struct for sending temperature as BT service data */

struct nrf_service_data
  {
    int16_t   service_uuid;
    uint8_t   value;
  };

/* advertisement PDU */

struct btle_adv_pdu
  {
    /* PDU type, most of it 0x42  */

    uint8_t pdu_type;

    /* payload size */

    uint8_t pl_size;

    /* MAC address */

    uint8_t mac[6];

    /* payload (including 3 bytes for CRC) */

    uint8_t payload[24];
  };

/* payload chunk in advertisement PDU payload */

struct btle_pdu_chunk
  {
    uint8_t size;
    uint8_t type;
    uint8_t data[];
  };

/****************************************************************************
 * Public Data
 ****************************************************************************/

/* helper macro to access chunk at specific offset */

#define chunk(x, y) ((struct btle_pdu_chunk *)(x.payload+y))

int nrf24_cfg();
void nrf24_adv();

#endif /* __NRF24L01_BTLE_H */
