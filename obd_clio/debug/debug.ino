#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128           
#define SCREEN_HEIGHT 32           

#define OLED_RESET -1              // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C        ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {
  Serial.begin(115200);
  while(!Serial);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);  // set display color to white even though my display cant display anything else
  display.setRotation(2);
  display.setTextSize(1);
  display.setCursor(0,0);

  display.display();
}

char buf[128] = {0}, c;
int i=0;

unsigned long prevMillis = 0;
void loop() {

  unsigned long currentMillis = millis();

  if(currentMillis - prevMillis > 2000) {
    prevMillis = currentMillis;
    Serial.print("010c\r\n");
  }
  

  if(Serial.available() > 13) {
      memset(buf, 0, 128);
      Serial.readBytes(buf, Serial.available());

        display.clearDisplay();
        display.setCursor(0, 0);
        display.print(buf);
        display.display();

    }
}
