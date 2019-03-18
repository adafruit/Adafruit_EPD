/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_EPD.h"

// Default is FeatherWing pinouts

#define EPD_CS     10
#define EPD_DC      9
#define SRAM_CS     6
#define EPD_RESET   4 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)
/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 epd(152, 152 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
//Adafruit_IL0373 epd(212, 104 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.7" tricolor EPD */
Adafruit_IL91874 epd(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);


void setup(void) {
  Serial.begin(115200);
  Serial.print("Hello! EPD Test");

  epd.begin();
  Serial.println("Initialized");
}

void loop() {
  for (int rot=0; rot<4; rot++) {
    epd.setRotation(rot);
    epd.clearBuffer();
    epd.fillScreen(EPD_WHITE);
    epd.drawRect(10, 20, 10, 20, EPD_BLACK);
  
    epd.display();
  
    delay(15 * 1000);
  }
}
