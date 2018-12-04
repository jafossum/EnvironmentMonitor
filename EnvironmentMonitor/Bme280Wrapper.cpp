#include "Bme280Wrapper.h"

BME280 myBME280;

Bme280Wrapper::Bme280Wrapper() {}

void Bme280Wrapper::init() {
    //Initialize BME280
    myBME280.settings.commInterface = I2C_MODE;
    myBME280.settings.I2CAddress = 0x77;
    myBME280.settings.runMode = 3; //Normal mode
    myBME280.settings.tStandby = 0;
    myBME280.settings.filter = 4;
    myBME280.settings.tempOverSample = 5;
    myBME280.settings.pressOverSample = 5;
    myBME280.settings.humidOverSample = 5;

    //Calling .begin() causes the settings to be loaded
    delay(10);  //Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
    byte id = myBME280.begin(); //Returns ID of 0x60 if successful
    if (id != 0x60)
    {
        Serial.println("Problem with BME280");
    }
    else
    {
        Serial.println("BME280 online");
    }
}

float getTempC() {
    return myBME280.readTempC();
}

float getTempF() {
    return myBME280.readTempF();
}

float getHumidity() {
    return myBME280.readFloatHumidity();
}

float getPressure() {
    return myBME280.readFloatPressure();
}

float getAltitudeMeters() {
    return myBME280.readFloatAltitudeMeters();
}

float getAltitudeFeet() {
    return myBME280.readFloatAltitudeFeet();
}

float Bme280Wrapper::readTempC() {
  return this->getAverage(&getTempC, 5);
}

float Bme280Wrapper::readTempF() {
  return this->getAverage(&getTempF, 5);
}

float Bme280Wrapper::readHumidity() {
  return this->getAverage(&getHumidity, 5);
}

float Bme280Wrapper::readPressure() {
  return this->getAverage(&getPressure, 5);
}

float Bme280Wrapper::readAltitudeMeters() {
  return this->getAverage(&getAltitudeMeters, 5);
}

float Bme280Wrapper::readAltitudeFeet() {
    return this->getAverage(&getAltitudeFeet, 5);
}

float Bme280Wrapper::getAverage(bme820Ptr func, int numIterations) {
  float sum = 0;
  for (int i = 0; i < numIterations; i++) {
    sum += func();
    delay(5);
  }
  return sum / numIterations;
}