#include "Adafruit_EPD.h"

#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
#define OLED_BUSY   5
#define SRAM_CS     10

/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 display(152, 152 ,OLED_DC, OLED_RESET, OLED_CS, OLED_BUSY, SRAM_CS);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
Adafruit_IL0373 display(212, 104 ,OLED_DC, OLED_RESET, OLED_CS, OLED_BUSY, SRAM_CS);

/* Uncomment the following line if you are using 2.7" tricolor EPD */
//Adafruit_IL91874 display(264, 176 ,OLED_DC, OLED_RESET, OLED_CS, OLED_BUSY, SRAM_CS);

void setup() {
  display.begin();
  display.clearBuffer();

  //draw some pretty lines
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, EPD_BLACK);
  }

  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, EPD_RED);
  }
  
  //refresh the display
  display.display();
}

void loop() {
  //don't do anything!
}