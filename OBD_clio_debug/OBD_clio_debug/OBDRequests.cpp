#include "Arduino.h"
#include <Adafruit_SSD1306.h>
#include "ELMduino.h"
#include "OBDRequests.h"
#include "UnitDisplayed.h"

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
  int text_size_unit_normal
)
{

  // set private variables
  _OBD = OBD;
  _obd_state = RPM;
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

    case RPM:
    {
      temp_uint32 = _OBD->rpm();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {

        obdValue = String(temp_uint32);
        _unit->setUnitType(FRACTIONAL);
        _unit->setNominator("obr");
        _unit->setDenominator("min");
        
      }
      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case REF_TORQUE:
    {
      temp_uint16 = _OBD->referenceTorque();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {

        obdValue = String(temp_uint16);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("Nm");

      }
      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case KPH:
    {
      temp_int32 = _OBD->kph();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp_int32);
        _unit->setUnitType(FRACTIONAL);
        _unit->setNominator("km");
        _unit->setDenominator("h");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case MAF_RATE:
    {
      temp = _OBD->mafRate();

      if (_OBD->nb_rx_state == ELM_SUCCESS) 
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("MR");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case THROTTLE:
    {
      temp = _OBD->throttle();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case SHORT_TERM_FUEL_TRIM_BANK:
    {
      temp = _OBD->shortTermFuelTrimBank_1();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case LONG_TERM_FUEL_TRIM_BANK:
    {
      temp = _OBD->longTermFuelTrimBank_1();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
        obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case ENGINE_LOAD:
    {
      temp = _OBD->engineLoad();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case RELATIVE_THROTTLE:
    {
      temp = _OBD->relativeThrottle();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case AMBIENT_AIR_TEMP:
    {
      temp = _OBD->ambientAirTemp();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("*C");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case COMMANDED_THROTTLE_ACTUATOR:
    {
      temp = _OBD->commandedThrottleActuator();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("%");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case MAX_MAF_RATE:
    {
      temp = _OBD->maxMafRate();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("MM");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case OIL_TEMP:
    {
      temp = _OBD->oilTemp();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("*C");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case FUEL_INJECT_TIMING:
    {
      temp = _OBD->fuelInjectTiming();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(NORMAL);
        _unit->setNormalUnit("*");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    case FUEL_RATE:
    {
      temp = _OBD->fuelRate();

      if (_OBD->nb_rx_state == ELM_SUCCESS)
      {
       obdValue = String(temp);
        _unit->setUnitType(FRACTIONAL);
        _unit->setNominator("L");
        _unit->setDenominator("h");
      }

      else if (_OBD->nb_rx_state != ELM_GETTING_MSG)
        obdValue = "ERROR R";

      break;
    }

    


    default:
    {
      obdValue = "ERR";
      break;
    }
  }

}