/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_EPD.h"

#define SD_CS       5
#define SRAM_CS     6
#define EPD_CS      9
#define EPD_DC      10  
#define EPD_RESET   -1 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 epd(152, 152 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
Adafruit_IL0373 epd(212, 104 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.7" tricolor EPD */
//Adafruit_IL91874 epd(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);


void setup(void) {
  Serial.begin(115200);
  Serial.print("Hello! EPD Test");

  epd.begin();
  Serial.println("Initialized");

  epd.setRotation(2);

  // large block of text
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  epd.setTextWrap(true);

  epd.setCursor(10, 10);
  epd.setTextSize(1);
  epd.setTextColor(EPD_BLACK);
  epd.print("Get as much education as you can. Nobody can take that away from you");

  epd.setCursor(50, 70);
  epd.setTextColor(EPD_RED);
  epd.print("--Eben Upton");

  epd.display();
}

void loop() {
  delay(500);
}
