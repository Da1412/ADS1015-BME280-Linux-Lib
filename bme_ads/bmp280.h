/***************************************************************************
Modified BSD License
====================

Copyright © 2016, Andrei Vainik
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

This piece of code was combined from several sources
https://github.com/adafruit/Adafruit_BME280_Library
https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf
https://projects.drogon.net/raspberry-pi/wiringpi/i2c-library/

****************************************************************************/
#include <stdio.h>
#include <wiringPiI2C.h>
//==================================================//
#ifndef __BMP280_H__
#define __BMP280_H__
//==================================================//
#define BMP280_ADDRESS                0x76
#define BMP280_ADDRESS_ALT            0X77
#define BMP280_CHIPID                 0X60
//==================================================//
#define BMP280_REGISTER_DIG_T1        0x88
#define BMP280_REGISTER_DIG_T2        0x8A
#define BMP280_REGISTER_DIG_T3        0x8C
//==================================================//
#define BMP280_REGISTER_DIG_P1        0x8E
#define BMP280_REGISTER_DIG_P2        0x90
#define BMP280_REGISTER_DIG_P3        0x92
#define BMP280_REGISTER_DIG_P4        0x94
#define BMP280_REGISTER_DIG_P5        0x96
#define BMP280_REGISTER_DIG_P6        0x98
#define BMP280_REGISTER_DIG_P7        0x9A
#define BMP280_REGISTER_DIG_P8        0x9C
#define BMP280_REGISTER_DIG_P9        0x9E
//==================================================//
#define BMP280_REGISTER_DIG_H1        0xA1
#define BMP280_REGISTER_DIG_H2        0xE1
#define BMP280_REGISTER_DIG_H3        0xE3
#define BMP280_REGISTER_DIG_H4        0xE4
#define BMP280_REGISTER_DIG_H5        0xE5
#define BMP280_REGISTER_DIG_H6        0xE7
//==================================================//
#define BMP280_REGISTER_CHIPID        0xD0
#define BMP280_REGISTER_VERSION       0xD1
#define BMP280_REGISTER_SOFTRESET     0xE0
#define BMP280_RESET                  0xB6
#define BMP280_REGISTER_CAL26         0xE1
#define BMP280_REGISTER_CONTROLHUMID  0xF2
#define BMP280_REGISTER_CONTROL       0xF4
#define BMP280_REGISTER_CONFIG        0xF5
#define BMP280_REGISTER_PRESSUREDATA  0xF7
#define BMP280_REGISTER_TEMPDATA      0xFA
#define BMP280_REGISTER_HUMIDDATA     0xFD
#define MEAN_SEA_LEVEL_PRESSURE       1013
//==================================================//


typedef struct
{
  uint16_t dig_T1;
  int16_t  dig_T2;
  int16_t  dig_T3;

  uint16_t dig_P1;
  int16_t  dig_P2;
  int16_t  dig_P3;
  int16_t  dig_P4;
  int16_t  dig_P5;
  int16_t  dig_P6;
  int16_t  dig_P7;
  int16_t  dig_P8;
  int16_t  dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;
} bmp280_calib_data;

typedef struct
{
  uint8_t pmsb;
  uint8_t plsb;
  uint8_t pxsb;

  uint8_t tmsb;
  uint8_t tlsb;
  uint8_t txsb;

  uint8_t hmsb;
  uint8_t hlsb;

  uint32_t temperature;
  uint32_t pressure;
  uint32_t humidity;

} bmp280_raw_data;

class bmp280
{
protected:
  uint8_t m_i2cAddress;
  int fd;
  bmp280_calib_data cal;

public:
  bmp280(uint8_t i2cAddress =  BMP280_ADDRESS);
  int setup();
  void readCalibrationData(int fd, bmp280_calib_data *cal);
  int32_t getTemperatureCalibration(bmp280_calib_data *cal, int32_t adc_T);
  float compensateTemperature(int32_t t_fine);
  float compensatePressure(int32_t adc_P, bmp280_calib_data *cal, int32_t t_fine);
  float compensateHumidity(int32_t adc_H, bmp280_calib_data *cal, int32_t t_fine);
  void getRawData(int fd, bmp280_raw_data *raw);
  int getFD() {return fd;}
  bmp280_calib_data* getCalibrationData() { return &cal; }
  float getAltitude(float pressure);

private:
};

#endif
