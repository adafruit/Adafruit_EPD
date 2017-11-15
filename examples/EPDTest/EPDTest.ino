#include "Adafruit_EPD.h"

#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
#define OLED_BUSY   5
#define SRAM_CS     10

/* Uncomment the following line if you are using 1.54" tricolor EPD */
Adafruit_IL91874 display(152, 152 ,OLED_DC, OLED_RESET, OLED_CS, OLED_BUSY, SRAM_CS);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
//Adafruit_IL91874 display(104, 212 ,OLED_DC, OLED_RESET, OLED_CS, OLED_BUSY, SRAM_CS);

void setup() {
  display.begin();
  display.clearBuffer();

  //draw some pretty lines
  uint8_t color = EPD_WHITE;
  for (int16_t i=0; i<display.width(); i+=4) {
    if(i == 64) color = EPD_BLACK;
    if(i == 128) color = EPD_RED;
    display.drawLine(0, 0, i, display.height()-1, color);
  }

  color = EPD_WHITE;
  for (int16_t i=0; i<display.height(); i+=4) {
  if(i == 64) color = EPD_BLACK;
  if(i == 128) color = EPD_RED;
    display.drawLine(display.width()-1, 0, 0, i, color);
  }

  //refresh the display
  display.display();
}

void loop() {
  //don't do anything!
}