/* Based on SparkFun sketched and guides for the following components:
* CCS811
* BME280
* SparkFun OpenSegment Shield with 20mm Segment display

*/

#include "Wire.h"
#include "SegmentDisplay.h"
#include "Bme280Wrapper.h"
#include "Ccs811Wrapper.h"

// Comment this out for not printing data to the serialport after Setup()
#define DEBUG

float BMEtempC;
float BMEhumid;
float CCSco2;
float CCStvoc;

String displayText;
int displayCounter = 0;

// Global sensor objects
Ccs811Wrapper Ccs811Wrapper;
Bme280Wrapper Bme280Wrapper;
SegmentDisplay mySegmentDisplay;

void setup()
{
  Wire.begin(); //Join the bus as master

  Serial.begin(115200);  // initialize Serial interface
  while (!Serial) 
  {
    delay(200); // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Apply BME280 data to CCS811 for compensation.");

  // Start SegmentDisplay
  mySegmentDisplay.init();

  // Initialize CCS811
  Ccs811Wrapper.init();

  // Initialize BME820
  Bme280Wrapper.init();
}

void loop() 
{
  if (Ccs811Wrapper.dataAvailable()) //Check to see if CCS811 has new data
  {
  #ifdef DEBUG
    printData(); //Pretty print all the data
  #endif

    BMEtempC = Bme280Wrapper.readTempC();
    BMEhumid = Bme280Wrapper.readFloatHumidity();
    CCSco2 = Ccs811Wrapper.getCO2();
    CCStvoc = Ccs811Wrapper.getTVOC();

    //This sends the temperature data to the CCS811
    Ccs811Wrapper.setEnvironmentalData(BMEhumid, BMEtempC);

#ifdef DEBUG
    Serial.print("Applying new values (deg C, %): ");
    Serial.print(BMEtempC);
    Serial.print(",");
    Serial.println(BMEhumid);
    Serial.println();
#endif
  }

  updateDisplay();

  delay(2000); //Wait for next reading
}

void printData()
{
  Serial.print(" CO2[");
  Serial.print(Ccs811Wrapper.getCO2());
  Serial.print("]ppm");

  Serial.print(" TVOC[");
  Serial.print(Ccs811Wrapper.getTVOC());
  Serial.print("]ppb");

  Serial.print(" temp[");
  Serial.print(Bme280Wrapper.readTempC(), 1);
  Serial.print("]C");

  //Serial.print(" temp[");
  //Serial.print(Bme280Wrapper.readTempF(), 1);
  //Serial.print("]F");

  Serial.print(" pressure[");
  Serial.print(Bme280Wrapper.readFloatPressure(), 2);
  Serial.print("]Pa");

  //Serial.print(" pressure[");
  //Serial.print((Bme280Wrapper.readFloatPressure() * 0.0002953), 2);
  //Serial.print("]InHg");

  //Serial.print("altitude[");
  //Serial.print(Bme280Wrapper.readFloatAltitudeMeters(), 2);
  //Serial.print("]m");

  //Serial.print("altitude[");
  //Serial.print(Bme280Wrapper.readFloatAltitudeFeet(), 2);
  //Serial.print("]ft");

  Serial.print(" humidity[");
  Serial.print(Bme280Wrapper.readFloatHumidity(), 0);
  Serial.print("]%");

  Serial.println();
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