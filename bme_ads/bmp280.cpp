#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <math.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "bmp280.h"

 //#define BMP280_ADDRESS 0X76

bmp280::bmp280(uint8_t i2cAddress) {
    m_i2cAddress = wiringPiI2CSetupInterface("/dev/i2c-2", i2cAddress);
}

int bmp280::setup() {

  fd = wiringPiI2CSetupInterface("/dev/i2c-2", BMP280_ADDRESS);
  if(fd < 0) {
    printf("Device not found");
    return -1;
  }

  // Check CHIP ID (0x60 expected)
  int chip_id = wiringPiI2CReadReg8(fd, BMP280_REGISTER_CHIPID);
  printf("Sensor CHIP ID: 0x%02X\n", chip_id);

if (chip_id != 0x58 && chip_id != 0x60) { //BMP280 (0x58) || BME280 (0x60)
  printf("Invalid CHIP ID. Check sensor type.\n");
  return -1;
}

  wiringPiI2CWriteReg8(fd, BMP280_REGISTER_SOFTRESET, BMP280_RESET);
  delay(300);

  readCalibrationData(fd, &cal);

  // Display some calibration values to verify
 // printf("T1=%u, T2=%d, T3=%d\n", cal.dig_T1, cal.dig_T2, cal.dig_T3); -> debugging

  // Set config register (0xF5): standby time, filter, etc.
  // 0xA0 = Filter off, 1000ms standby time
  wiringPiI2CWriteReg8(fd, BMP280_REGISTER_CONFIG, 0x00);

  // Set control register (0xF4): operation mode, oversampling
  // 0x27 = Normal mode, temp x1, pressure x1
  wiringPiI2CWriteReg8(fd, BMP280_REGISTER_CONTROL, 0x27);

  // Wait at least 10ms for the sensor to be ready
  delay(100);
  return 0;
}


int32_t bmp280::getTemperatureCalibration(bmp280_calib_data *cal, int32_t adc_T) {
  int32_t var1  = ((((adc_T>>3) - ((int32_t)cal->dig_T1 <<1))) *
     ((int32_t)cal->dig_T2)) >> 11;

  int32_t var2  = (((((adc_T>>4) - ((int32_t)cal->dig_T1)) *
       ((adc_T>>4) - ((int32_t)cal->dig_T1))) >> 12) *
     ((int32_t)cal->dig_T3)) >> 14;

  return var1 + var2;
}

void bmp280::readCalibrationData(int fd, bmp280_calib_data *data) {

  data->dig_T1 = (uint8_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_T1 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_T1);
  data->dig_T2 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_T2 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_T2);
  data->dig_T3 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_T3 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_T3);

  data->dig_P1 = (uint8_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P1 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P1);
  data->dig_P2 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P2 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P2);
  data->dig_P3 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P3 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P3);
  data->dig_P4 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P4 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P4);
  data->dig_P5 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P5 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P5);
  data->dig_P6 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P6 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P6);
  data->dig_P7 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P7 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P7);
  data->dig_P8 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P8 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P8);
  data->dig_P9 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P9 + 1) << 8 | wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_P9);

  data->dig_H1 = (uint8_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H1);
  data->dig_H2 = (int16_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H2 + 1) << 8 |wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H2);
  data->dig_H3 = (uint8_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H3);
  data->dig_H4 = (wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H4) << 4) |(wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H4+1) & 0xF);
  data->dig_H5 = (wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H5+1) << 4) |(wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H5) >> 4);
  data->dig_H6 = (int8_t)wiringPiI2CReadReg8(fd, BMP280_REGISTER_DIG_H6);
}

float bmp280::compensateTemperature(int32_t t_fine) {
  float T  = (t_fine * 5 + 128) >> 8;
  return T/100;
}

float bmp280::compensatePressure(int32_t adc_P, bmp280_calib_data *cal, int32_t t_fine) {
  int64_t var1, var2, p;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)cal->dig_P6;
  var2 = var2 + ((var1*(int64_t)cal->dig_P5)<<17);
  var2 = var2 + (((int64_t)cal->dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)cal->dig_P3)>>8) +
    ((var1 * (int64_t)cal->dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)cal->dig_P1)>>33;

  if (var1 == 0) {
    return 0;  //x Except -> :0
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)cal->dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)cal->dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)cal->dig_P7)<<4);
  return (float)p/256;
}


float bmp280::compensateHumidity(int32_t adc_H, bmp280_calib_data *cal, int32_t t_fine) {
  int32_t v_x1_u32r;

  v_x1_u32r = (t_fine - ((int32_t)76800));

  v_x1_u32r = (((((adc_H << 14) - (((int32_t)cal->dig_H4) << 20) -
      (((int32_t)cal->dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) *
         (((((((v_x1_u32r * ((int32_t)cal->dig_H6)) >> 10) *
        (((v_x1_u32r * ((int32_t)cal->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
      ((int32_t)2097152)) * ((int32_t)cal->dig_H2) + 8192) >> 14));

  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
           ((int32_t)cal->dig_H1)) >> 4));

  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
  float h = (v_x1_u32r>>12);
  return  h / 1024.0;
}

void bmp280::getRawData(int fd, bmp280_raw_data *raw) {

 /* raw->pmsb = wiringPiI2CRead(fd);
  raw->plsb = wiringPiI2CRead(fd);
  raw->pxsb = wiringPiI2CRead(fd);

  raw->tmsb = wiringPiI2CRead(fd);
  raw->tlsb = wiringPiI2CRead(fd);
  raw->txsb = wiringPiI2CRead(fd);*/

  raw->hmsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_HUMIDDATA);
  raw->hlsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_HUMIDDATA+1);

 /* raw->temperature = 0;
  raw->temperature = (raw->tmsb << 12) | (raw->tlsb << 4) | (raw->txsb >> 4);

  raw->pressure = 0;
  raw->pressure = (raw->pmsb << 12) | (raw->plsb << 4) | (raw->pxsb >> 4);
*/

  raw->pmsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_PRESSUREDATA);
  raw->plsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_PRESSUREDATA+1);
  raw->pxsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_PRESSUREDATA+2);

  // r temp reg (0xFA-0xFC)
  raw->tmsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_TEMPDATA);
  raw->tlsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_TEMPDATA+1);
  raw->txsb = wiringPiI2CReadReg8(fd, BMP280_REGISTER_TEMPDATA+2);

  // 20-bit value
  raw->pressure = ((uint32_t)raw->pmsb << 12) | ((uint32_t)raw->plsb << 4) | ((uint32_t)raw->pxsb >> 4);
  raw->temperature = ((uint32_t)raw->tmsb << 12) | ((uint32_t)raw->tlsb << 4) | ((uint32_t)raw->txsb >> 4);

  printf("Debug: Read raw temp bytes: MSB=0x%02X LSB=0x%02X XLSB=0x%02X -> %d\n", 
         raw->tmsb, raw->tlsb, raw->txsb, raw->temperature);

  raw->humidity =  ((uint32_t)raw->hmsb << 8) | ((uint32_t)raw->hlsb);
}


float bmp280::getAltitude(float pressure) {

  return 44330.0 * (1.0 - pow(pressure / MEAN_SEA_LEVEL_PRESSURE, 0.190294957));
}
