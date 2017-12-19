
#include "SegmentDisplay.h"

SegmentDisplay::SegmentDisplay() {}

void SegmentDisplay::init()
{
    Wire.begin(); //Join the bus as master

    //Send the reset command to the display - this forces the cursor to return to the beginning of the display
    Serial.print("Staring and resetting Display...");
    Wire.beginTransmission(DISPLAY_ADDRESS);
    Wire.write('v');
    Wire.endTransmission();
    Serial.println("Display reset done...");

    String displayText = "INIT";
    SendString(&displayText[0]);
}

//Given a number, SendValue chops up an int into four values and sends them out over I2C
void SegmentDisplay::SendValue(int value)
{
  Wire.beginTransmission(DISPLAY_ADDRESS); // transmit to device
  Wire.write(value / 1000); //Send the left most digit
  value %= 1000; //Now remove the left most digit from the number we want to display
  Wire.write(value / 100);
  value %= 100;
  Wire.write(value / 10);
  value %= 10;
  Wire.write(value); //Send the right most digit
  Wire.endTransmission(); //Stop I2C transmission
}

//Given a string, SendString chops up the string and sends out the first four characters over i2c
void SegmentDisplay::SendString(char *toSend)
{
  Wire.beginTransmission(DISPLAY_ADDRESS); // transmit to device
  for(byte x = 0 ; x < 4 ; x++)
    Wire.write(toSend[x]); //Send a character from the array out over I2C
  Wire.endTransmission(); //Stop I2C transmission
}

//SendSpecialChar sends known / defined specialchars over i2c
void SegmentDisplay::SendSpecialChar(SpecialChar specialChar)
{
  Wire.beginTransmission(DISPLAY_ADDRESS); // transmit to device
    if (specialChar != SpecialChar::CLEAR) {
      uint8_t controlChar;
      switch ( specialChar ) 
      {
        case SpecialChar::APOSTROPHE:
          controlChar = 5;
          break;
        case SpecialChar::COLON:
          controlChar = 4;
          break;
        case SpecialChar::DECIMAL4:
          controlChar = 3;
          break;
        case SpecialChar::DECIMAL3:
          controlChar = 2;
          break;
        case SpecialChar::DECIMAL2:
          controlChar = 1;
          break;
        case SpecialChar::DECIMAL1:
          controlChar = 0;
          break;
        default:
          Serial.print("Unspecified error.");
        }
        WriteSpecialChar(controlChar);
    } else {
        DeleteSpecialChar();
    }
    Wire.endTransmission(); //Stop I2C transmission
}

// Write colon, apostrophoe or other special character
void SegmentDisplay::WriteSpecialChar(uint8_t controlChar) 
{
  Wire.write(0x77);  // Decimal, colon, apostrophe control command
  Wire.write(1<<controlChar); // Turns on colon, apostrophoe, and far-right decimal
}

// Remove colon, apostrophoe or other special character
void SegmentDisplay::DeleteSpecialChar() 
{
  Wire.write(0x77);  // Decimal, colon, apostrophe control command
  Wire.write((byte) 0); // Turns on colon, apostrophoe, and far-right decimal
}