#ifndef OBDRequests_h
#define OBDRequests_h

#include "WString.h"
#include "Arduino.h"
#include "ELMduino.h"
#include "UnitDisplayed.h"
#include <Adafruit_SSD1306.h>

// bullshit for determination what pid is curretly running
enum Obd_pid { _RPM, _REF_TORQUE, _KPH, _MAF_RATE, _THROTTLE, _SHORT_TERM_FUEL_TRIM_BANK, _LONG_TERM_FUEL_TRIM_BANK, _ENGINE_LOAD, _COOLANT_TEMP, _FUEL_RAIL_PRESSURE, _FUEL_RAIL_GAUGE_PRESSURE, _FUEL_LEVEL, ABS_LOAD, _COMMANDED_AIR_FUEL_RATIO,
_RELATIVE_THROTTLE, _AMBIENT_AIR_TEMP, _COMMANDED_THROTTLE_ACTUATOR, _MAX_MAF_RATE, _OIL_TEMP, _FUEL_INJECT_TIMING, _FUEL_RATE, _COMMANDED_EGR };

class OBDRequests
{
  private:
    ELM327* _OBD;
    Obd_pid _obd_state;
    UnitDisplayed* _unit;

    // buffers for reciving data from OBD
    float temp;
    uint32_t temp_uint32;
    uint16_t temp_uint16;
    int32_t temp_int32;

    // for displying
    int _text_size, _text_y_offset;

    Adafruit_SSD1306* _display;

    String obdValue = "";

  public:
    OBDRequests(
      // obdrequest variables
      ELM327* OBD,
      int text_size,
      int text_y_offset,
      Adafruit_SSD1306* display,

      // unitdisplay variables
      int screen_width, 
      int unit_fractional_dividing_line_length, 
      int text_size_unit_fractional, 
      int nominator_y_offset,
      int denominator_y_offset,
      int unit_normal_x_offset,
      int unit_normal_y_offset,
      int text_size_unit_normal,
      Servo servo
    );

    void setObdState(Obd_pid obd_state);

    void main();

};
#endif