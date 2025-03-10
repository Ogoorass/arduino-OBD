#include <stdio.h>
#include "Arduino.h"
#include "HardwareSerial.h"
#include "HardwareSerial_private.h"
#include <string.h>
#include <stdint.h>
#ifndef Hc05obd_h
#define Hc05obd_h

#define BUF_SIZE 62

#include "Tasks.h"

// #define debug

// order of this enum MUST be the same as in requests array
enum Request { RPM, COOLANT_TEMP, SHORT_TRIM, TIMING_ADVANCE };
const char *PIDs[4] = { 
  "010c\r\n",   // rpm
  "0105\r\n",   // coolant temp
  "0106\r\n",   // short term fuel trim
  "010e\r\n"    // timing advance
}; 

class Hc05obd : public HardwareSerial {
  private:
    const char 
      *echo_off = "ate0\r\n",
      *headers_off = "ath0\r\n",
      *proto_auto = "atsp00\r\n",
      *atok = "at\r\n";

    uint8_t _is_waiting_for_response = 0, _initProgress = 0, _start_waiting_time = 0, _len = 0;
    Request _last_request; 

    uint8_t _a=0, _b=0;

    char _msg[BUF_SIZE] = {0}, _buf[BUF_SIZE+1] = {0};
 

  public:
    using HardwareSerial::HardwareSerial;
  
    int8_t init() {
      int c = Serial.read();
      if (c != -1) {
        if(_initProgress < 2) {
          if(c >= 'A' && c <= 'Z') {
            _msg[_len++] = c;
            return 1;
          }

          if(c != '>')
            return 1;

          if(strcmp(_msg, "OK") == 0) {
            _initProgress++;
            _len = 0;
            memset(_msg, 0, BUF_SIZE);
          }
          else {
            memset(_msg, 0, BUF_SIZE);
            _len = 0;
            return -1;
          }
        }

        if(_initProgress == 3 && c == '>')
          return 0;
      }
      
      
      // runs only on status 1 - serial not available
      switch(_initProgress){
        case 0:
          Serial.print(echo_off);
        break;
        case 1:
          Serial.print(headers_off);
        break;
        case 2:
          Serial.print(PIDs[RPM]);
          _initProgress++;
        break;
      }      

      return 1;
    }


    void sendPidRequest(Request r) {
      _last_request = r;
      _is_waiting_for_response = true;
      Serial.print(PIDs[r]);  
    }

    int8_t readPID(void *response) {

      _buf[_len++] = Serial.read();
      if(_buf[_len-1] != '>')
        return 1;

      memset(_msg, 0, BUF_SIZE);

      int j=0;
      for(int i=0; i<_len-1; i++) {
        char c = _buf[i];
        if((c >= 'A' && c <= 'F') || (c >= '0' && c <= '9')) 
          _msg[j++] = c;
      }

      if(j<6 || (_last_request == RPM && j<8)) {
        _is_waiting_for_response=0;
        memset(_buf, 0, BUF_SIZE);
        return -1;
      }

      
      strncpy(_buf, _msg+4, 2);
      _a = (int) strtol(_buf, 0, 16);
      switch(_last_request) {
        // add more pid request, which require B byte
        case RPM:
          strncpy(_buf, _msg+6, 2);
          _b = (int) strtol(_buf, 0, 16);
        break;
      }


      switch(_last_request) {
        case RPM:
          // to get rpm from this response you need to do some arithmetic
          // rpm = (256*a+b)/4 as in obd-pid wikipedia
          {
            uint16_t x = (uint16_t) ((256*_a + _b) / 4.0);
            memcpy(response, &x, 2);
          }
        break;
        case COOLANT_TEMP:
          {
            int8_t x = (int8_t) (_a-40);
            memcpy(response, &x, 1);
          }
        break;
        case SHORT_TRIM:
          {
            int8_t x = (int8_t) (100.0/128 * _a - 100);
            memcpy(response, &x, 1);
          }
        break;
        case TIMING_ADVANCE:
          {
            float x = (float) (_a/2.0 - 64);
            memcpy(response, &x, 4);
          }
        break;
      }

      _is_waiting_for_response=0;
      memset(_buf, 0, BUF_SIZE);
      _len = 0;
      return 0;
      

    }

    int8_t isWaitingForResponse() {
      return _is_waiting_for_response;
    }

    Request getPID() {
      return _last_request;
    }

};


#endif