#ifndef _THINKINK_270_GRAY4_W3_H
#define _THINKINK_270_GRAY4_W3_H

#include "Adafruit_EPD.h"

class ThinkInk_270_Grayscale4_W3 : public Adafruit_IL91874 {
private:
public:
  ThinkInk_270_Grayscale4_W3(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST,
                             int8_t CS, int8_t SRCS, int8_t MISO,
                             int8_t BUSY = -1)
      : Adafruit_IL91874(264, 176, SID, SCLK, DC, RST, CS, SRCS, MISO, -1){};

  ThinkInk_270_Grayscale4_W3(int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
                             int8_t BUSY = -1, SPIClass *spi = &SPI)
      : Adafruit_IL91874(264, 176, DC, RST, CS, SRCS, -1, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    Adafruit_IL91874::begin(true);

    layer_colors[EPD_WHITE] = 0b10;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b10;
    layer_colors[EPD_GRAY] = 0b10;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b11;

    default_refresh_delay = 1000;
    powerDown();
  };
};

#endif // _THINKINK_270_GRAY4_W3_H
