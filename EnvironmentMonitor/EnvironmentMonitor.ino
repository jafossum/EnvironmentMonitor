/* Based on SparkFun sketched and guides for the following components:
* CCS811
* BME280
* SparkFun OpenSegment Shield with 20mm Segment display

*/

#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>
#include "Wire.h"
#include "SegmentDisplay.h"

#define CCS811_ADDR 0x5B //Default I2C Address

// Comment this out for not printing data to the serialport after Setup()
#define DEBUG

float BMEtempC;
float BMEhumid;
float CCSco2;
float CCStvoc;

String displayText;
int displayCounter = 0;

//Global sensor objects
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;
SegmentDisplay mySegmentDisplay;

void setup()
{
  Wire.begin(); //Join the bus as master

  Serial.begin(9600);
  Serial.println();
  Serial.println("Apply BME280 data to CCS811 for compensation.");

  // Start SegmentDisplay
  mySegmentDisplay.init();

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

void loop() 
{
  if (myCCS811.dataAvailable()) //Check to see if CCS811 has new data (it's the slowest sensor)
  {
    myCCS811.readAlgorithmResults(); //Read latest from CCS811 and update tVOC and CO2 variables
  #ifdef DEBUG
    printData(); //Pretty print all the data
  #endif

    BMEtempC = myBME280.readTempC();
    BMEhumid = myBME280.readFloatHumidity();
    CCSco2 = myCCS811.getCO2();
    CCStvoc = myCCS811.getTVOC();

#ifdef DEBUG
    Serial.print("Applying new values (deg C, %): ");
    Serial.print(BMEtempC);
    Serial.print(",");
    Serial.println(BMEhumid);
    Serial.println();
#endif

    //This sends the temperature data to the CCS811
    myCCS811.setEnvironmentalData(BMEhumid, BMEtempC);
  }
  else if (myCCS811.checkForStatusError()) //Check to see if CCS811 has thrown an error
  {
    Serial.println(myCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
  }

  updateDisplay();

  delay(2000); //Wait for next reading
}

void printData()
{
  Serial.print(" CO2[");
  Serial.print(myCCS811.getCO2());
  Serial.print("]ppm");

  Serial.print(" TVOC[");
  Serial.print(myCCS811.getTVOC());
  Serial.print("]ppb");

  Serial.print(" temp[");
  Serial.print(myBME280.readTempC(), 1);
  Serial.print("]C");

  //Serial.print(" temp[");
  //Serial.print(myBME280.readTempF(), 1);
  //Serial.print("]F");

  Serial.print(" pressure[");
  Serial.print(myBME280.readFloatPressure(), 2);
  Serial.print("]Pa");

  //Serial.print(" pressure[");
  //Serial.print((myBME280.readFloatPressure() * 0.0002953), 2);
  //Serial.print("]InHg");

  //Serial.print("altitude[");
  //Serial.print(myBME280.readFloatAltitudeMeters(), 2);
  //Serial.print("]m");

  //Serial.print("altitude[");
  //Serial.print(myBME280.readFloatAltitudeFeet(), 2);
  //Serial.print("]ft");

  Serial.print(" humidity[");
  Serial.print(myBME280.readFloatHumidity(), 0);
  Serial.print("]%");

  Serial.println();
}

void printDriverError( CCS811Core::status errorCode )
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

void updateDisplay() 
{
  switch ( displayCounter )
  {
    case 0:
      displayText = "TEMP";
      mySegmentDisplay.SendString(&displayText[0]);
      displayCounter++;
      break;
    case 1:
      mySegmentDisplay.SendValue(static_cast<int>(BMEtempC*10));
      mySegmentDisplay.SendSpecialChar(SpecialChar::DECIMAL2);
      displayCounter++;
      break;
    case 2:
      displayText = "CO2";
      mySegmentDisplay.SendString(&displayText[0]);
      mySegmentDisplay.DeleteSpecialChar();
      displayCounter++;
      break;
    case 3:
      mySegmentDisplay.SendValue(static_cast<int>(CCSco2));
      displayCounter++;
      break;
    case 4:
      displayText = "HUMI";
      mySegmentDisplay.SendString(&displayText[0]);
      displayCounter++;
      break;
    case 5:
      mySegmentDisplay.SendValue(static_cast<int>(BMEhumid));
      displayCounter++;
      break;
    case 6:
      displayText = "TVOC";
      mySegmentDisplay.SendString(&displayText[0]);
      displayCounter++;
      break;
    case 7:
      mySegmentDisplay.SendValue(static_cast<int>(CCStvoc));
      displayCounter = 0;
      break;
    default:
      Serial.print("Unspecified error.");
  }
}