#include "Tasks.h"
#include "Stepper.h"
#include "Hc05obd.h"
#include "Display.h"
#include <Wire.h>
// #define debug

#define STOP_ON_ERROR false // determine if stops on error

// pnis for stepper motor
#define SP_1 2
#define SP_2 3
#define SP_3 4
#define SP_4 5

#define TRIG 6                    // limit switch pin to zero out stepper motor
#define BUTT 7

// time in microseconds
#define SEND_INTERVAL 250000
#define READ_INTERVAL 20000
#define MIN_STEP_INTERVAL 2500
#define MAX_STEP_INTERVAL 15000
#define TIME_OUT 5000000
#define LOADING_INTERVAL 500000
#define MIN_LOGO_TIME 2000000
#define DISPLAY_REFRESH_INTERVAL 5000000

#define REST_POS 90               // steps to release limit swith
#define STEP_ANGLE 0.00306796       //(2*M_PI)/2048 angle of each step in rad
#define MAX_ANGLE 4.18879
#define MAX_RPM 8000              // max displayd rpm

#define SCREEN_WIDTH 128           
#define SCREEN_HEIGHT 32           

#define OLED_RESET -1              // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C        ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32



int8_t cool_temp, short_term_fuel_trim;
uint16_t rpm;
float timing_advance;
int32_t res;

unsigned long 
  readMicros      = 0, 
  sendMicros      = 0, 
  prevStepMicros  = 0, 
  currentMicros   = 0, 
  prevDispMicros  = 0, 
  timeOutMicros   = 0, 
  stepInterval    = 0, 
  debaunceMicros  = 0;

uint8_t 
  pid_switch[2]           = {RPM, COOLANT_TEMP}, 
  pid_switch_id           = 0, 
  last_available_bytes    = 0, 
  current_available_bytes = 0, 
  gotFirstDisplayValue    = 0, 
  isPressed               = 0, 
  pid_id                  = 1;

Stepper st(
  SP_1, 
  SP_2, 
  SP_3, 
  SP_4, 
  MAX_RPM, 
  STEP_ANGLE, 
  MAX_ANGLE, 
  &stepInterval, 
  MIN_STEP_INTERVAL, 
  MAX_STEP_INTERVAL
);

Hc05obd bt(
  (uint8_t *)(&UBRR0H), 
  (uint8_t *)(&UBRR0L), 
  (uint8_t *)(&UCSR0A), 
  (uint8_t *)(&UCSR0B), 
  (uint8_t *)(&UCSR0C), 
  (uint8_t *)(&UDR0)
); // hope its the first serial (on arduino nano it should be)

Display display(
  SCREEN_WIDTH, 
  SCREEN_HEIGHT, 
  &Wire, 
  OLED_RESET
);

void setup() {
  pinMode(TRIG, INPUT_PULLUP);
  pinMode(BUTT, INPUT_PULLUP);
  

  //display initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (STOP_ON_ERROR)
      ;
  }

  display.init();

  Serial.begin(115200);
  while(!Serial && STOP_ON_ERROR);


  display.clearDisplay();               // adafruit librarry initializes with its logo, so clear the buffer
  display.putLogoInDisplayBuffer();
  display.display();

  int8_t status = 1, isZero = 0, state = 0;
  unsigned long prevTimeInit = 0, prevTimeZero = 0, prevTimeDisp = 0, startDispLogo = micros();

  do {

    unsigned long currentTime = micros();

    if((currentTime - prevTimeInit > SEND_INTERVAL) || Serial.available() && status != 0) {
      prevTimeInit = currentTime;
      status = bt.init();
    }

    if(currentTime - prevTimeDisp > LOADING_INTERVAL && currentTime - startDispLogo > MIN_LOGO_TIME) {
      prevTimeDisp = currentTime;
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(48,0);
      switch(state) {
        case 0:
          display.print(".");
        break;
        case 1:
          display.print("..");
        break;
        case 2:
          display.print("...");
        break;
      }
      display.display();
      state++;
      if(state > 2)
        state = 0;
    }

    if(currentTime - prevTimeZero > MIN_STEP_INTERVAL) {
      prevTimeZero = currentTime;
      // it's a bit faster directly modifing registers
      if(PIND >> TRIG & 1 == 1 && isZero == 0) {
        st.step(-1);
      } else if(digitalRead(TRIG) == LOW && isZero == 0) {
        isZero = 1;
        st.zeroPosition();
        st.setDestination(REST_POS);
      } else {
        st.update();
        if(st.isAtRest()) {
          st.zeroPosition();
          st.setDestination(0);
          isZero = 2;
        }
      }
    }

  } while(status != 0 || isZero != 2);

  
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("Coolant");
  display.setCursor(72, 16);
  display.print("temp");
  display.display();
}



void loop() {
  currentMicros = micros();
  
  // stepper logic
  if (currentMicros - prevStepMicros >= stepInterval) {
    prevStepMicros = currentMicros;
    st.update();
  }

  // send PID logic
  if((!bt.isWaitingForResponse() && currentMicros - sendMicros >= SEND_INTERVAL) || currentMicros - sendMicros >= TIME_OUT) {
    sendMicros = currentMicros;
    pid_switch_id=!pid_switch_id;
    bt.sendPidRequest(pid_switch[pid_switch_id]);
  }

  

  // read PID logic
  if(bt.available() && bt.readPID(&res) == 0){

    switch (bt.getPID()) {
      case RPM:
        memcpy(&rpm, &res, 2);
        if(rpm >= 0 && rpm <= 8000)
          st.setDestinationInRPM(rpm);
      break;
      case COOLANT_TEMP:
        memcpy(&cool_temp, &res, 1);
      break;
      case SHORT_TRIM:
        memcpy(&short_term_fuel_trim, &res, 1);
      break;
      case TIMING_ADVANCE:
        memcpy(&timing_advance, &res, 4);
      break;
    }

    if(bt.getPID() != RPM && !gotFirstDisplayValue) {
      gotFirstDisplayValue = 1;
    }
  }


  // display logic
  if(currentMicros - prevDispMicros > DISPLAY_REFRESH_INTERVAL && gotFirstDisplayValue) {
    prevDispMicros = currentMicros;
    display.clearDisplay();
    display.setTextSize(4);

    switch(pid_switch[1]) {
      case COOLANT_TEMP: 
        {
          display.setCursor(24, 0);
          display.print(cool_temp);
          int16_t x = display.getCursorX();
          int16_t y = display.getCursorY();
          display.drawDegree(x, y);
          display.setCursor(x+15,0);
          display.print("C");
        }
      break;
      case SHORT_TRIM:
        display.setCursor(16, 0);
        display.print(short_term_fuel_trim);
        display.setCursor(96, 0);
        display.print("%");
      break;
      case TIMING_ADVANCE:
        display.setTextSize(3);
        display.setCursor(16, 0);
        display.print(timing_advance, 1);
        display.drawDegree(112,0);
      break;
    }

    display.display();
  }


  // button loginc
  if(digitalRead(BUTT) == LOW && !isPressed && currentMicros - debaunceMicros >= 20000) {
    prevDispMicros = currentMicros - DISPLAY_REFRESH_INTERVAL / 2;
    debaunceMicros = currentMicros;
    isPressed = 1;

    pid_id++;
    if(pid_id > 3)
      pid_id = 1;

    display.clearDisplay();
    switch(pid_id) {
      case COOLANT_TEMP:
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print("Coolant");
        display.setCursor(72, 16);
        display.print("temp");
      break;
      case SHORT_TRIM:
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print("Short");
        display.setCursor(72, 16);
        display.print("trim");
      break;
      case TIMING_ADVANCE:
        display.setTextSize(2);
        display.setCursor(0,0);
        display.print("Timing");
        display.setCursor(36, 16);
        display.print("advance");
      break;
    }
    display.display();
    pid_switch[1] = pid_id;
  }

  if(digitalRead(BUTT) == HIGH && currentMicros - debaunceMicros >= 20000) {
    isPressed = 0;
    debaunceMicros = currentMicros;
  }
}

