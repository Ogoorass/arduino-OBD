#include "Arduino.h"
#include <Adafruit_SSD1306.h>
#include "ELMduino.h"
#include "OBDRequests.h"
#include "UnitDisplayed.h"
#include <Servo.h>

// constructor
OBDRequests::OBDRequests(
  ELM327* OBD,
  int text_size,
  int text_y_offset,
  Adafruit_SSD1306* display,

  int screen_width, 
  int unit_fractional_dividing_line_length, 
  int text_size_unit_fractional, 
  int nominator_y_offset,
  int denominator_y_offset,
  int unit_normal_x_offset,
  int unit_normal_y_offset,
  int text_size_unit_normal,
  Servo servo
)
{

  // set private variables
  _OBD = OBD;
  _obd_state = _RPM;
  _text_size = text_size;
  _text_y_offset = text_y_offset;
  _display = display;

  // make new object
  _unit = new UnitDisplayed(
    display,
    screen_width,
    unit_fractional_dividing_line_length, 
    text_size_unit_fractional,
    nominator_y_offset,
    denominator_y_offset,
    unit_normal_x_offset,
    unit_normal_y_offset,
    text_size_unit_normal
  );

}

// set _obd_state
void OBDRequests::setObdState(Obd_pid obd_state)
{
  _obd_state = obd_state;
}


// put it in in loop()
void OBDRequests::main()
{

  // set display buffor and display it
  _display->clearDisplay();
  _display->setTextSize(_text_size);
  _display->setCursor(0, _text_y_offset);
  _display->print(obdValue);
  _unit->putInBuffer();         // put unit in buffer
  _display->display();

  // determine for what send request to OBD
  switch (_obd_state)
  {

    case _RPM:
    {
      temp_uint32 = _OBD->rpm();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {

        obdValue = String(temp_uint32);
        _unit->setUnitType(FRACTIONAL);
        _unit->setNominator("obr");
        _unit->setDenominator("min");
        
        servo.write(map(obdValue - 1000, 0, 5000, 0, 180)); // -1000 to compensate 180 degree roation of the servo

      }       

      break;
    }

    case _REF_TORQUE:
    {
      temp_uint16 = _OBD->referenceTorque();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {

        obdValue = String(temp_uint16);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("Nm");

      }       

      break;
    }

    case _KPH:
    {
      temp_int32 = _OBD->kph();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp_int32);
        _unit->setUnitType(FRACTIONAL);
        _unit->setNominator("km");
        _unit->setDenominator("h");
      }       

      break;
    }

    case _MAF_RATE:
    {
      temp = _OBD->mafRate();

      if (_OBD->nb_rx_state == ELM_SUCCESS) 
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("MR");
      }       

      break;
    }

    case _THROTTLE:
    {
      temp = _OBD->throttle();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }       

      break;
    }

    case _SHORT_TERM_FUEL_TRIM_BANK:
    {
      temp = _OBD->shortTermFuelTrimBank_1();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }       

      break;
    }

    case _LONG_TERM_FUEL_TRIM_BANK:
    {
      temp = _OBD->longTermFuelTrimBank_1();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }       

      break;
    }

    case _ENGINE_LOAD:
    {
      temp = _OBD->engineLoad();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }       

      break;
    }

    case _RELATIVE_THROTTLE:
    {
      temp = _OBD->relativeThrottle();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }       

      break;
    }

    case _AMBIENT_AIR_TEMP:
    {
      temp = _OBD->ambientAirTemp();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("*C");
      }       

      break;
    }

    case _COMMANDED_THROTTLE_ACTUATOR:
    {
      temp = _OBD->commandedThrottleActuator();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }       

      break;
    }

    case _MAX_MAF_RATE:
    {
      temp = _OBD->maxMafRate();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("MM");
      }       

      break;
    }

    case _OIL_TEMP:
    {
      temp = _OBD->oilTemp();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("*C");
      }      

      break;
    }

    case _FUEL_INJECT_TIMING:
    {
      temp = _OBD->fuelInjectTiming();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("*");
      }

      break;
    }

    case _FUEL_RATE:
    {
      temp = _OBD->fuelRate();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(FRACTIONAL);
        _unit->setNominator("L");
        _unit->setDenominator("h");
      }

      break;
    }

    case _COMMANDED_EGR:
    {
      temp = _OBD->commandedEGR();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      break;
    }

    


    default:
    {
      obdValue = "ERR";
      break;
    }
  }

  if (_OBD->nb_rx_state != ELM_GETTING_MSG)
    obdValue = "ERROR R";

}
