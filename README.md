# ADS1015-BME280-Linux-Lib
Custom ADS1115 &amp; BME280 Library for Linux

Tested in Raspberry Pi 3B & Orange Pi 3B (Debian Bullseye OS)

To Compile:

``g++ -o main main.cpp bmp280.cpp Adafruit_ADS1015.cpp -lwiringPi -lm``

``sudo ./main``
