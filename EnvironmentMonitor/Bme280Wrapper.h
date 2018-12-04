#ifndef bme280_wrapper_h
#define bme280_wrapper_h

#include "Arduino.h"
#include <SparkFunBME280.h>
#include "Wire.h"

// Typedef for a function pointer
typedef float(*bme820Ptr)();

class Bme280Wrapper {
private:
    float getAverage(bme820Ptr func, int numIterations);
public:
    Bme280Wrapper();
    void init();
    float readTempC();
    float readTempF();
    float readHumidity();
    float readPressure();
    float readAltitudeMeters();
    float readAltitudeFeet();
};

#endif