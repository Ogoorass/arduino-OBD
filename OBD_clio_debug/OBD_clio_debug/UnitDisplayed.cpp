#pragma once

#include "Arduino.h"
#include "UnitDisplayed.h"
#include <Adafruit_SSD1306.h>

// constructor
UnitDisplayed::UnitDisplayed(
  Adafruit_SSD1306* display,
  int screen_width, 
  int unit_fractional_dividing_line_length, 
  int text_size_unit_fractional, 
  int nominator_y_offset,
  int denominator_y_offset,
  int unit_normal_x_offset,
  int unit_normal_y_offset,
  int text_size_unit_normal
)
{

  _display = display;
  _screen_width = screen_width;
  _unit_fractional_dividing_line_length = unit_fractional_dividing_line_length; 
  _text_size_unit_fractional = text_size_unit_fractional; 
  _nominator_y_offset = nominator_y_offset;
  _denominator_y_offset = denominator_y_offset;
  _unit_normal_x_offset = unit_normal_x_offset;
  _unit_normal_y_offset = unit_normal_y_offset;
  _text_size_unit_normal = text_size_unit_normal;

}


// puts unit in buffer
void UnitDisplayed::putInBuffer()
  {

    if (_type == FRACTIONAL)
    {

      int nominator_count = 0, denominator_count = 0;

      // get how many characters is in dominator and denominator
      for (int i = 0; i < 3; i++)
      {
        if (_nominator[i] != '\0')
          nominator_count++;

        if (_denominator[i] != '\0')
          denominator_count++;
      }      


      // length to fraction line
      int length_to_line = _screen_width - _unit_fractional_dividing_line_length,
      // length to nominator text
      lenght_to_nominator = length_to_line + (6 / nominator_count),
      // length to denominator text
      length_to_denominator = length_to_line + (6 / denominator_count);



      _display->setTextSize(_text_size_unit_fractional);

      // make fraction line
      for (int i = length_to_line; i < _screen_width - 1; i++)
      {
        _display->drawPixel(i, 16, SSD1306_WHITE);
        _display->drawPixel(i, 17, SSD1306_WHITE);
      }

      // make nominator and denominator
      _display->setCursor(lenght_to_nominator, _nominator_y_offset);
      _display->print(_nominator);
      _display->setCursor(length_to_denominator, _denominator_y_offset);
      _display->print(_denominator);

    }

    else if (_type == NORMAL)
    {

      int length_to_start_of_unit_text = _screen_width - _unit_normal_x_offset;

      _display->setTextSize(_text_size_unit_normal);
      _display->setTextColor(SSD1306_WHITE);
      _display->setCursor(length_to_start_of_unit_text, _unit_normal_y_offset);
      
      _display->print(_normal_unit);

    }

  }

  // set type
  void UnitDisplayed::setUnitType(Unit_type type)
  {
    _type = type;
  }

  // set nominator
  void UnitDisplayed::setNominator(char nominator[])
  {
    _nominator = nominator;
  }

  // set denominator
  void UnitDisplayed::setDenominator(char denominator[])
  {
    _denominator = denominator;
  }

  // set normal unit
  void UnitDisplayed::setNormalUnit(char normal_unit[])
  {
    _normal_unit = normal_unit;
  }