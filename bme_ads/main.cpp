#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "bmp280.h"
#include "Adafruit_ADS1015.h"

#define Buzzer_Pin 13

bmp280 bmp(BMP280_ADDRESS);
Adafruit_ADS1015 ads(0x48);

void callTempPress() {
     bmp280_raw_data raw;
     bmp.getRawData(bmp.getFD(), &raw);

     int32_t t_fine = bmp.getTemperatureCalibration(bmp.getCalibrationData(), raw.temperature);
     float temp = bmp.compensateTemperature(t_fine);
     float press = bmp.compensatePressure(raw.pressure, bmp.getCalibrationData(), t_fine) / 100;

     printf("Temperatur: %.2f C\nPressure: %.2f hPa\n", temp, press);
     usleep(1000000);

     return;
}

void callADS() {
     ads.begin();
     ads.setGain(GAIN_ONE);

     while (1) {
        for (int i = 0; i < 5; i++) {
           for (int channel = 0; channel < 4; channel++) {
              uint16_t adcValue = ads.readADC_SingleEnded(channel);
              float voltage = ads.computeVolts(adcValue);
              printf("Channel %d: ADC = %d, Voltage = %.3f V\n", channel, adcValue, voltage);
           }
           printf("-------------------\n");
           usleep(1000000);
        }
     }

     return;
}

int main() {

    int fd = wiringPiI2CSetupInterface("/dev/i2c-2", 0x48);
    if (fd < 0) {
        printf("Inisialisasi Komunikasi I2C Error, Cek:\n1. Wiring Hardware\n2. I2C Address 0x48 dan 0x76");
        return 1;
    }

    if (wiringPiSetup() == -1) {
        printf("Modul Wiring Pi Error\n");
        return -1;
    }

    if (bmp.setup() == -1) {
        printf("Setup BMP280 Failed\n");
        return -1;
    }

    while (1) {
        printf("===========================================================================");
        callTempPress();
        printf("===========================================================================");
        callADS();
        printf("===========================================================================");
    }

    return 0;
}
