#ifndef bme280_wrapper_h
#define bme280_wrapper_h

#include <Arduino.h>
#include <SparkFunBME280.h>
#include "Wire.h"

class Bme280Wrapper {
private:
    BME280 myBME280;
public:
    Bme280Wrapper();
    void init();
    long readTempC();
    long readFloatHumidity();
    long readFloatPressure();
};

#endif