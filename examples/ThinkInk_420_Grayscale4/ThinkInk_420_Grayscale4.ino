/***************************************************
  4-level grayscale info card for the Adafruit 4.2" 400x300 E-Ink Display
  with the SSD1683 chipset (product #6381, GDEY042T81 / FPC-190 ribbon).

  Hardware-verified on the Adafruit Feather RP2040 ThinkInk (24-pin connector).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_THINKINK // Feather RP2040 ThinkInk
#define EPD_DC PIN_EPD_DC
#define EPD_CS PIN_EPD_CS
#define EPD_BUSY PIN_EPD_BUSY
#define SRAM_CS -1 // use onboard RAM
#define EPD_RESET PIN_EPD_RESET
#define EPD_SPI &SPI1 // secondary SPI for ThinkInk
#else
#define EPD_DC 10
#define EPD_CS 9
#define EPD_BUSY 7   // -1 to not use a pin (waits a fixed delay)
#define SRAM_CS 6    // -1 to use microcontroller RAM
#define EPD_RESET 8  // -1 to share with microcontroller Reset
#define EPD_SPI &SPI // primary SPI
#endif

ThinkInk_420_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                     EPD_SPI);

void setup() {
  display.begin(THINKINK_GRAYSCALE4);
  display.clearBuffer();

  int16_t w = display.width();
  int16_t h = display.height();

  display.fillScreen(EPD_WHITE);

  display.setTextColor(EPD_BLACK);
  display.setTextSize(3);
  display.setCursor(6, 10);
  display.print("Adafruit ThinkInk");
  display.setTextSize(4);
  display.setCursor(6, 50);
  display.print("4.2\" 400x300");
  display.setTextColor(EPD_DARK);
  display.setCursor(6, 95);
  display.print("4-Gray E-Ink");
  display.setTextColor(EPD_BLACK);
  display.setTextSize(3);
  display.setCursor(6, 140);
  display.print("SSD1683  #6381");

  // 4-level gray ramp filling the bottom: black | dark | light | white
  const int rampTop = 180;
  const int rampH = h - rampTop;
  const int seg = w / 4;
  const uint16_t order[4] = {EPD_BLACK, EPD_DARK, EPD_LIGHT, EPD_WHITE};
  for (int i = 0; i < 4; i++) {
    int x = i * seg;
    int segW = (i < 3) ? seg : (w - x);
    display.fillRect(x, rampTop, segW, rampH, order[i]);
  }
  display.drawRect(0, rampTop, w, rampH, EPD_BLACK);
  for (int i = 1; i < 4; i++) {
    display.drawFastVLine(i * seg, rampTop, rampH, EPD_BLACK);
  }

  display.display();
}

void loop() {}
