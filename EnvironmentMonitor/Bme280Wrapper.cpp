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

long Bme280Wrapper::readTempC() {
    return myBME280.readTempC();
}

long Bme280Wrapper::readTempF() {
    return myBME280.readTempF();
}

long Bme280Wrapper::readFloatHumidity() {
    return myBME280.readFloatHumidity();
}

long Bme280Wrapper::readFloatPressure() {
    return myBME280.readFloatPressure();
}

long Bme280Wrapper::readFloatAltitudeMeters() {
    return myBME280.readFloatAltitudeMeters();
}
long Bme280Wrapper::readFloatAltitudeFeet() {
    return myBME280.readFloatAltitudeFeet();
}