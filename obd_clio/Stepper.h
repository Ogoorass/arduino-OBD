#include <math.h>
#ifndef Stepper_h
#define Stepper_h

#include "Tasks.h"

class Stepper {
  private:
    int _p1, _p2, _p3, _p4, _pos, _step_number, _dest, _max_rpm;
    float _step_angle, _max_angle; // in rad
    unsigned long *_step_interval, _min_step_interval, _max_step_interval;

  public:
    Stepper(
      int pin1, 
      int pin2, 
      int pin3, 
      int pin4, 
      int max_rpm, 
      float step_angle, 
      float max_angle, 
      unsigned long *step_interval,
      unsigned long min_step_interval,
      unsigned long max_step_interval
    ) :
      _p1{pin1},
      _p2{pin2},
      _p3{pin3},
      _p4{pin4},
      _step_number{0},
      _step_angle{step_angle},
      _max_rpm{max_rpm},
      _max_angle{max_angle},
      _step_interval{step_interval},
      _min_step_interval{min_step_interval},
      _max_step_interval{max_step_interval}
    {
      pinMode(pin1, OUTPUT);
      pinMode(pin2, OUTPUT);
      pinMode(pin3, OUTPUT);
      pinMode(pin4, OUTPUT);
      }

  void step(int dir) {
    switch(_step_number) {
      case 3:
        digitalWrite(_p1, HIGH);
        digitalWrite(_p2, LOW);
        digitalWrite(_p3, LOW);
        digitalWrite(_p4, LOW);
      break;
      case 2:
        digitalWrite(_p1, LOW);
        digitalWrite(_p2, HIGH);
        digitalWrite(_p3, LOW);
        digitalWrite(_p4, LOW);
      break;
      case 1:
        digitalWrite(_p1, LOW);
        digitalWrite(_p2, LOW);
        digitalWrite(_p3, HIGH);
        digitalWrite(_p4, LOW);
      break;
      case 0:
        digitalWrite(_p1, LOW);
        digitalWrite(_p2, LOW);
        digitalWrite(_p3, LOW);
        digitalWrite(_p4, HIGH);
      break;
    }
    if(dir > 0) {
      if(_step_number==3)
       _step_number=0;
      else
        _step_number++;
      _pos++;
    } else if (dir < 0) {
      if(_step_number==0)
       _step_number=3;
      else
        _step_number--;
      _pos--; 
    }
  }

  void zeroPosition() {
    _pos = 0;
  }

  int getPosition() {
    return _pos;
  }

  void setPosition(int position) {
    while(_pos != position) {
      step(position - _pos);
      delay(3);
    }
  }

  void setDestinationInRPM(int rpm) {
    // function to solve offset between motor shaft axis and display axis
    int dest;        
    float angle,b;
    if (rpm < 600 || rpm > 7000) {
      b = ((float)rpm/_max_rpm) * 3.781;
      dest = (int) lroundf(b/_step_angle);
    } else {
      b = ((float)rpm/_max_rpm) * _max_angle;
      angle = 1.89054 - asinf((37.5*sinf(b+1.04719))/sqrtf(1470.25-600*cosf(b+1.04719)));
      dest = (int) lroundf(angle/_step_angle);
    }

    _dest = dest;
  }

  void setDestination(int dest) {
    _dest = dest;
  }

  void update() {
    int distance = _dest - _pos;
    step(distance);
    int abs_distance = abs(distance);
    if(abs_distance > 200) {
      *_step_interval = _min_step_interval;
    } else {
      unsigned long interval = (200.0 / abs_distance) * _min_step_interval;
      if (interval > _max_step_interval) {
        *_step_interval = _max_step_interval;
      } else {
        *_step_interval = interval;
      }
    }
  }

  uint8_t isAtRest() {
    return _dest == _pos;
  }
};

#endif