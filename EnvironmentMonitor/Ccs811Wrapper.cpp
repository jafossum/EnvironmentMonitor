#include "Ccs811Wrapper.h"

CCS811 myCCS811(CCS811_ADDR);

Ccs811Wrapper::Ccs811Wrapper() {}

void Ccs811Wrapper::init() {
    //This begins the CCS811 sensor and prints error status of .begin()
    CCS811Core::status returnCode = myCCS811.begin();
    if (returnCode != CCS811Core::SENSOR_SUCCESS)
    {
        Serial.println("Problem with CCS811");
        printDriverError(returnCode);
    }
    else
    {
        Serial.println("CCS811 online");
    }
}

boolean Ccs811Wrapper::dataAvailable() {
    if (myCCS811.dataAvailable()) //Check to see if CCS811 has new data (it's the slowest sensor)
    {
      myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
      return true;
    } 
    else if (myCCS811.checkForStatusError()) //Check to see if CCS811 has thrown an error
    {
      Serial.println(myCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
    }
    return false;
}

long getCO2private() {
  return myCCS811.getCO2();
}

long getTVOCprivate() {
    return myCCS811.getTVOC();
}

long Ccs811Wrapper::getCO2() {
  return this->getAverage(&getCO2private, 5);
}

long Ccs811Wrapper::getTVOC() {
  return this->getAverage(&getTVOCprivate, 5);
}

void Ccs811Wrapper::setEnvironmentalData(float Humid, float TempC) {
    myCCS811.setEnvironmentalData(Humid, TempC);
}

void Ccs811Wrapper::printDriverError( CCS811Core::status errorCode )
{
  switch ( errorCode )
  {
    case CCS811Core::SENSOR_SUCCESS:
      Serial.print("SUCCESS");
      break;
    case CCS811Core::SENSOR_ID_ERROR:
      Serial.print("ID_ERROR");
      break;
    case CCS811Core::SENSOR_I2C_ERROR:
      Serial.print("I2C_ERROR");
      break;
    case CCS811Core::SENSOR_INTERNAL_ERROR:
      Serial.print("INTERNAL_ERROR");
      break;
    case CCS811Core::SENSOR_GENERIC_ERROR:
      Serial.print("GENERIC_ERROR");
      break;
    default:
      Serial.print("Unspecified error.");
  }
}

long Ccs811Wrapper::getAverage(ccs811Ptr func, int numIterations) {
  long sum = 0;
  for (int i = 0; i < numIterations; i++) {
    sum += func();
    delay(2);
  }
  return sum / numIterations;
}