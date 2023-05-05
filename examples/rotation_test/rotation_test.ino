/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_EPD.h"
#include <Adafruit_GFX.h> // Core graphics library

// Default is FeatherWing pinouts

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_THINKINK // detects if compiling for
                                                // Feather RP2040 ThinkInk
#define EPD_DC PIN_EPD_DC       // ThinkInk 24-pin connector DC
#define EPD_CS PIN_EPD_CS       // ThinkInk 24-pin connector CS
#define EPD_BUSY PIN_EPD_BUSY   // ThinkInk 24-pin connector Busy
#define SRAM_CS -1              // use onboard RAM
#define EPD_RESET PIN_EPD_RESET // ThinkInk 24-pin connector Reset
#define EPD_SPI &SPI1           // secondary SPI for ThinkInk
#else
#define EPD_DC 10
#define EPD_CS 9
#define EPD_BUSY 7 // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS 6
#define EPD_RESET 8  // can set to -1 and share with microcontroller Reset!
#define EPD_SPI &SPI // primary SPI
#endif

/* Uncomment the following line if you are using 1.54" tricolor EPD */
// Adafruit_IL0373 display(152, 152, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 1.54" monochrome EPD */
// Adafruit_SSD1608 display(200, 200, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 2.13" tricolor EPD */
Adafruit_IL0373 display(212, 104, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                        EPD_SPI);
//#define FLEXIBLE_213

/* Uncomment the following line if you are using 2.13" monochrome 250*122 EPD */
// Adafruit_SSD1675 display(250, 122, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 2.7" tricolor or grayscale EPD
 */
// Adafruit_IL91874 display(264, 176, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

/* Uncomment the following line if you are using 2.9" EPD */
// Adafruit_IL0373 display(296, 128, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI); #define FLEXIBLE_290

/* Uncomment the following line if you are using 4.2" tricolor EPD */
// Adafruit_IL0398 display(300, 400, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

void setup(void) {
  Serial.begin(115200);
  Serial.print("Hello! EPD Test");

  display.begin();
  Serial.println("Initialized");

#if defined(FLEXIBLE_213) || defined(FLEXIBLE_290)
  // The flexible displays have different buffers and invert settings!
  display.setBlackBuffer(1, false);
  display.setColorBuffer(1, false);
#endif
}

void loop() {
  for (int rot = 0; rot < 4; rot++) {
    display.setRotation(rot);
    display.clearBuffer();
    display.fillScreen(EPD_WHITE);
    display.drawRect(10, 20, 10, 20, EPD_BLACK);

    display.display();

    delay(1000);
  }
}
