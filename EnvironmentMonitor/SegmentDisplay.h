#ifndef segment_display_h
#define segment_display_h

#include <Arduino.h>
#include "Wire.h"

#define DISPLAY_ADDRESS 0x71 //This is the default address of the OpenSegment with both solder jumpers open

//From https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#wiki-decimal
enum SpecialChar {
    APOSTROPHE,
    COLON,
    DECIMAL4,
    DECIMAL3, 
    DECIMAL2,
    DECIMAL1,
    CLEAR,
};

class SegmentDisplay {
private:
    void WriteSpecialChar(uint8_t controlChar);
    void DeleteSpecialChar();

public:
    SegmentDisplay();
    void init();
    void SendValue(int tempCycles);
    void SendString(char *toSend);
    void SendSpecialChar(SpecialChar specialChar);
};

#endif