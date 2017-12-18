/* Based on SparkFun sketched and guides for the following components:
* CCS811
* BME280
* SparkFun OpenSegment Shield with 20mm Segment display

*/

#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>
#include "Wire.h"

#define CCS811_ADDR 0x5B //Default I2C Address
#define DISPLAY_ADDRESS1 0x71 //This is the default address of the OpenSegment with both solder jumpers open

// Comment this out for not printing data to the serialport after Setup()
#define DEBUG

//From https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#wiki-decimal
#define APOSTROPHE  5
#define COLON       4
#define DECIMAL4    3
#define DECIMAL3    2
#define DECIMAL2    1
#define DECIMAL1    0

float BMEtempC;
float BMEhumid;

String displayText;
int displayCounter = 0;

//Global sensor objects
CCS811 myCCS811(CCS811_ADDR);
BME280 myBME280;

void setup()
{
  Wire.begin(); //Join the bus as master

  Serial.begin(9600);
  Serial.println();
  Serial.println("Apply BME280 data to CCS811 for compensation.");

  //Send the reset command to the display - this forces the cursor to return to the beginning of the display
  Serial.print("Staring and resetting Display...");
  Wire.beginTransmission(DISPLAY_ADDRESS1);
  Wire.write('v');
  Wire.endTransmission();
  Serial.println("Display reset done...");

  displayText = "INIT";
  i2cSendString(&displayText[0]);

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
      i2cSendString(&displayText[0]);
      displayCounter++;
      break;
    case 1:
      i2cSendValue(static_cast<int>(BMEtempC*10));
      i2cWriteSpecialChar(DECIMAL2);
      displayCounter++;
      break;
    case 2:
      displayText = "CO2";
      i2cSendString(&displayText[0]);
      displayCounter++;
      break;
    case 3:
      i2cSendValue(static_cast<int>(myCCS811.getCO2()));
      i2cDeleteSpecialChar();
      displayCounter++;
      break;
    case 4:
      displayText = "HUMI";
      i2cSendString(&displayText[0]);
      displayCounter++;
      break;
    case 5:
      i2cSendValue(static_cast<int>(BMEhumid));
      displayCounter++;
      break;
    case 6:
      displayText = "TVOC";
      i2cSendString(&displayText[0]);
      displayCounter++;
      break;
    case 7:
      i2cSendValue(static_cast<int>(BMEhumid));
      displayCounter = 0;
      break;
    default:
      Serial.print("Unspecified error.");
  }
}

//Given a number, i2cSendValue chops up an integer into four values and sends them out over I2C
void i2cSendValue(int tempCycles)
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  Wire.write(tempCycles / 1000); //Send the left most digit
  tempCycles %= 1000; //Now remove the left most digit from the number we want to display
  Wire.write(tempCycles / 100);
  tempCycles %= 100;
  Wire.write(tempCycles / 10);
  tempCycles %= 10;
  Wire.write(tempCycles); //Send the right most digit
  Wire.endTransmission(); //Stop I2C transmission
}

//Given a string, i2cSendString chops up the string and sends out the first four characters over i2c
void i2cSendString(char *toSend)
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  for(byte x = 0 ; x < 4 ; x++)
    Wire.write(toSend[x]); //Send a character from the array out over I2C
  Wire.endTransmission(); //Stop I2C transmission
}

// Write colon, apostrophoe or other special character
void i2cWriteSpecialChar(uint8_t controlChar) 
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  Wire.write(0x77);  // Decimal, colon, apostrophe control command
  Wire.write(1<<controlChar); // Turns on colon, apostrophoe, and far-right decimal
  Wire.endTransmission(); //Stop I2C transmission
}

// Remove colon, apostrophoe or other special character
void i2cDeleteSpecialChar() 
{
  Wire.beginTransmission(DISPLAY_ADDRESS1); // transmit to device #1
  Wire.write(0x77);  // Decimal, colon, apostrophe control command
  Wire.write((byte) 0); // Turns on colon, apostrophoe, and far-right decimal
  Wire.endTransmission(); //Stop I2C transmission
}