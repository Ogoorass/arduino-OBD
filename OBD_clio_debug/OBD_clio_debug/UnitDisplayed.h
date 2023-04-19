#pragma once

#ifndef UnitDisplayed_h
#define UnitDisplayed_h

#include "Arduino.h"
#include <Adafruit_SSD1306.h>

// unit type
enum Unit_type { FRACTIONAL, NORMAL };

// bullshit for displaying unit
class UnitDisplayed
{
  private:
    int _screen_width, 
    _unit_fractional_dividing_line_length, 
    _text_size_unit_fractional, 
    _nominator_y_offset,
    _denominator_y_offset,
    _unit_normal_x_offset,
    _unit_normal_y_offset,
    _text_size_unit_normal;

    char* _nominator;
    char* _denominator;
    char* _normal_unit;

    Unit_type _type;

    Adafruit_SSD1306* _display;

  public:

    UnitDisplayed(
      Adafruit_SSD1306* display,
      int screen_width, 
      int unit_fractional_dividing_line_length, 
      int text_size_unit_fractional, 
      int nominator_y_offset,
      int denominator_y_offset,
      int unit_normal_x_offset,
      int unit_normal_y_offset,
      int text_size_unit_normal
    );

    void putInBuffer();
    void setNominator(char nominator[]); // max 4 characters, when puting string like this: "...", there are 3 characters and empty string so its 4 characters!
    void setDenominator(char denominator[]); // max 4 characters like above
    void setNormalUnit(char normal_unit[]); // max 3 characters like above
    void setUnitType(Unit_type type);

};

#endif