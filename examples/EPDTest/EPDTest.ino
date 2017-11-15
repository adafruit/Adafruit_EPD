#include "Adafruit_EPD.h"

#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
#define OLED_BUSY   5
Adafruit_IL91874 display(152, 152 ,OLED_DC, OLED_RESET, OLED_CS, OLED_BUSY, 10);

void setup() {
  // put your setup code here, to run once:
  display.begin();
  display.clearBuffer();
  
  uint8_t color = EPD_DARK;
  for (int16_t i=0; i<display.width(); i+=4) {
  if(i == 64) color = EPD_BLACK;
  if(i == 128) color = EPD_RED;
    display.drawLine(0, 0, i, display.height()-1, color);
  }

  color = EPD_DARK;
  for (int16_t i=0; i<display.height(); i+=4) {
  if(i == 64) color = EPD_BLACK;
  if(i == 128) color = EPD_RED;
    display.drawLine(display.width()-1, 0, 0, i, color);
  }
  
  display.display();
}

void loop() {
  // put your main code here, to run repeatedly:

}
