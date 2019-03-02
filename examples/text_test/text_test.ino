/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_EPD.h"

#define EPD_CS     10
#define EPD_DC      9
#define SRAM_CS     6
#define EPD_RESET   5 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 epd(152, 152 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.13" tricolor or flexible EPD */
Adafruit_IL0373 epd(212, 104 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//#define FLEXIBLE_213     // if you're using the monochrome flex 2.13"

/* Uncomment the following line if you are using 2.7" tricolor EPD */
//Adafruit_IL91874 epd(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);


#if defined(FLEXIBLE_213)
  #define COLOR1 EPD_RED
  #define COLOR2 EPD_RED
#else // any other tricolor
  #define COLOR1 EPD_BLACK
  #define COLOR2 EPD_RED
#endif


void setup(void) {
  Serial.begin(115200);
  Serial.print("Hello! EPD Test");

  epd.begin();
#if defined(FLEXIBLE_213)
  epd.invertColorLogic(0, false);
  epd.invertColorLogic(1, false);
#endif

  Serial.println("Initialized");

  epd.setRotation(2);

  // large block of text
  epd.clearBuffer();
  epd.setTextWrap(true);

  epd.setCursor(10, 10);
  epd.setTextSize(1);
  epd.setTextColor(COLOR1);
  epd.print("Get as much education as you can. Nobody can take that away from you");

  epd.setCursor(50, 70);
  epd.setTextColor(COLOR2);
  epd.print("--Eben Upton");

  epd.display();
}

void loop() {
  delay(500);
}
