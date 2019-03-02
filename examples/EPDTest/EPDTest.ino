/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_EPD.h"


#define EPD_CS     10
#define EPD_DC      9
#define SRAM_CS     8
#define EPD_RESET   5 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    3 // can set to -1 to not use a pin (will wait a fixed delay)

/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 display(152, 152, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.13" tricolor EPD */
Adafruit_IL0373 display(212, 104, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//#define FLEXIBLE_213

/* Uncomment the following line if you are using 2.7" tricolor or grayscale EPD */
//Adafruit_IL91874 display(264, 176, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);

/* Uncomment the following line if you are using 2.9" EPD */
//Adafruit_IL0373 display(296, 128, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//#define FLEXIBLE_290


#if defined(FLEXIBLE_213) || defined(FLEXIBLE_290)
  #define COLOR1 EPD_RED
  #define COLOR2 EPD_RED
#else // any other tricolor
  #define COLOR1 EPD_BLACK
  #define COLOR2 EPD_RED
#endif

void setup() {
  Serial.begin(115200);
  //while (!Serial) { delay(10); }
  Serial.println("Adafruit EPD test");
  
  display.begin();
#if defined(FLEXIBLE_213) || defined(FLEXIBLE_290)
  display.invertColorLogic(0, false);
  display.invertColorLogic(1, false);
#endif
  display.clearBuffer();
  
  //draw some pretty lines
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, COLOR1);
  }

  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, COLOR2);  // on grayscale this will be mid-gray
  }

  display.display();
}

void loop() {
  //don't do anything!
}
