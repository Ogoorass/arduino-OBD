#include <BluetoothSerial.h>
#include "ELMduino.h"
#include "OBDRequests.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>

BluetoothSerial SerialBT;
#define ELM_PORT SerialBT

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define STOP_ON_ERROR false // determine if stops on error

#define BAUD_RATE 115200 // serial baud rate

// display stuff
#define TEXT_SIZE 3 // size for main text
#define TEXT_SIZE_UNIT_FRACTIONAL 1 // size for fractional unit text
#define TEXT_SIZE_UNIT_NORMAL 2 // size of normal unit text
#define TEXT_SIZE_ERROR 2 // size of error text
#define TEXT_Y_OFFSET 7 // vertical offset of main text
#define UNIT_FRACTIONAL_DIVIDING_LINE_LENGTH 20 // length of an dividing line in unit
#define NOMINATOR_Y_OFFSET 5 // y offset of nominator 
#define DENOMINATOR_Y_OFFSET 23 //y offset of denominator  
#define UNIT_NORMAL_X_OFFSET 20 // x offset of the start of the normal unit
#define UNIT_NORMAL_Y_OFFSET 16 // y offset of normal unit

//default I2C pins for esp32 are GPIO21 (SDA) and GPIO22 (SCL)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// elm327 interface object
ELM327 OBD;

//bullshit
uint32_t rpm = 0;
uint16_t referenceTorque = 0;
int32_t kph = 0;
float mafRate = 0, throttle = 0, fuelPressure = 0, shortTermFuelTrimBank_1 = 0, longTermFuelTrimBank_1 = 0, shortTermFuelTrimBank_2 = 0, longTermFuelTrimBank_2 = 0, engineLoad = 0, engineCoolantTemp = 0, fuelRailPressure = 0,
fuelRailGuagePressure = 0, fuelLevel = 0, absLoad = 0, commandedAirFuelRatio = 0, relativeThrottle = 0, ambientAirTemp = 0, commandedThrottleActuator = 0, maxMafRate = 0, oilTemp = 0, fuelInjectTiming = 0, fuelRate = 0,
demandedTorque = 0, torque = 0;

// bullshit for obd requests and displaying it
OBDRequests obdRequests(
  // OBDRequests
  &OBD,
  TEXT_SIZE,
  TEXT_Y_OFFSET,
  &display,

  // UnitDisplay
  SCREEN_WIDTH,
  UNIT_FRACTIONAL_DIVIDING_LINE_LENGTH,
  TEXT_SIZE_UNIT_FRACTIONAL,
  NOMINATOR_Y_OFFSET,
  DENOMINATOR_Y_OFFSET,
  UNIT_NORMAL_X_OFFSET,
  UNIT_NORMAL_Y_OFFSET,
  TEXT_SIZE_UNIT_NORMAL
);

// Renault logo
const char* renaultLogo[] = {
"         .......                                                                                                                ",
"        .........                                                                                                               ",
"        ..    . ..                                                                                                              ",
"       ..     .  .                                                                                                              ",
"      ..     ..  ..                                                                                                             ",
"      ..     .   ..                                                                                                             ",
"     ..     ..    ..                                                                                                            ",
"     ..     .      .                                                                                                            ",
"    ..     ...     ..       ............     ............  ......   ......   .......     .......   .... ........   .............",
"    ..     . .      ..      .............    .............  ......   ....    .......     .......   .... ........   .............",
"   ..     .. ..     ..       .....   .....     ....    ...   .....    ..       ......     .....     ..   ......    ... ..... ...",
"  ..      .   ..     ..       ....    .....    ....    ...   ......   ..      .......     .....     ..    ....     ..  .....  ..",
"  ..     ..    .     ..       ....    .....    ....          ......   ..      .. ....     .....     ..    ....     ..   ...   ..",
" ..     ..     ..     ..      ....    .....    ....          .......  ..      .. .....    .....     ..    ....          ...     ",
" ..     .       .     ..      ....   .....     ....  ..      .......  ..     ... .....    .....     ..    ....          ...     ",
"..     ..       ..     ..     ...........      ........      ........ ..     ..   ....    .....     ..    ....          ...     ",
".........       .........     ..........       ........      .. ..... ..     ..   ....    .....     ..    ....          ...     ",
" ........       ........      ....  ....       ....  ..      .. ........    ...........   .....     ..    ....          ...     ",
" .........     .........      ....  .....      ....          ..  .......    ..    .....   .....     ..    ....          ...     ",
"  .........   .........       ....   ....      ....     ..   ..  .......    ..     ....   .....     ..    ....    ..    ...     ",
"   ........   .........       ....   .....     ....     ..   ..   ......   ..      ....   .....     ..    ....    ..    ...     ",
"   ......... .........        ....    ....     ....     ..   ..    .....   ..       ....   ....     ..    ....    ..    ...     ",
"    ........ ........        ......   .....   ......   ...  ....   .....   ..       ....   ..........    ......  ...   .....    ",
"    .................       ........   ..... ............. ......   ..... ....    .......    .......    ............  .......   ",
"     ...............                                                                                                            ",
"     ...............                                                                                                            ",
"      .............                                                                                                             ",
"       ............                                                                                                             ",
"       ...........                                                                                                              ",
"        .........                                                                                                               ",
"        .........                                                                                                               ",
"         .......                                                                                                                "};




void setup() {

  Serial.begin(BAUD_RATE);  
  while(!Serial);

  //display initialization
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    while (STOP_ON_ERROR)
      ;
  }
  display.setTextColor(SSD1306_WHITE);  // set display color to white even though my display cant display anything else
  display.setTextSize(TEXT_SIZE);       // set size of the text

  display.clearDisplay();               // adafruit librarry initializes with its logo, so clear the buffer
  putLogoInDisplayBuffer(renaultLogo);  // put Renault logo in display buffer and...
  display.display();                    // display it

  // start bluetooth
  ELM_PORT.begin("ArduHUD", true);

  // connect to OBDII
  if (!ELM_PORT.connect("OBDII")) {
    display.clearDisplay();
    display.setCursor(0, TEXT_Y_OFFSET);
    display.setTextSize(TEXT_SIZE_ERROR);
    display.println("ERROR C1");
    display.display();
    while (STOP_ON_ERROR)
      ;
  }

  // make communication with ELM327
  if (!OBD.begin(ELM_PORT, false, 2000)) {
    display.clearDisplay();
    display.setCursor(0, TEXT_Y_OFFSET);
    display.setTextSize(TEXT_SIZE_ERROR);
    display.println("ERROR C2");
    display.display();
    while (STOP_ON_ERROR)
      ;
  }

}


void loop() {

  // main obd request and displaying logic
  obdRequests.main();

  // serial control of obd_state variable
  if (Serial.available() > 0)
  {

    String message = "";

    while (Serial.available() > 0)
      message += char(Serial.read());

    if (message == "RPM")
      obdRequests.setObdState(RPM);
    else if (message == "REF_TORQUE")
      obdRequests.setObdState(REF_TORQUE);
    else if (message == "KPH")
      obdRequests.setObdState(KPH);
    else if (message == "MAF_RATE")
      obdRequests.setObdState(MAF_RATE);
    else if (message == "THROTTLE")
      obdRequests.setObdState(THROTTLE);
    
    else
    {
      Serial.println("Bad token!");
      Serial.println(message);
      return;
    }

    Serial.println("OK");
  }

}


// puts char pointer array in display buffer
// in logo_bmp[] should be 32 pointers to 128-length strings
void putLogoInDisplayBuffer(const char* logo_bmp[])
{

  for (int y = 0; y < SCREEN_HEIGHT; y++)
  {
    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
      if (logo_bmp[y][x] == '.')
        display.drawPixel(x, y, SSD1306_WHITE);
    }
  }

}
