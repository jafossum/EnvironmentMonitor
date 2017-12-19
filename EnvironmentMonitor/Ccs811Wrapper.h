#ifndef ccs811_wrapper_h
#define ccs811_wrapper_h

#include <Arduino.h>
#include <SparkFunCCS811.h>
#include "Wire.h"

#define CCS811_ADDR 0x5B //Default I2C Address

class Ccs811Wrapper {
private:
    void printDriverError( CCS811Core::status errorCode );
public:
    Ccs811Wrapper();
    void init();
    boolean dataAvailable();
    long getCO2();
    long getTVOC();
    void setEnvironmentalData(float Humid, float TempC);
};

#endif