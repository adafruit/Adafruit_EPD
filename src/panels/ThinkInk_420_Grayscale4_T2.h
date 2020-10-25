#ifndef _THINKINK_420_GRAY4_T2_H
#define _THINKINK_420_GRAY4_T2_H

#include "Adafruit_EPD.h"

class ThinkInk_420_Grayscale4_T2 : public Adafruit_IL0398 {
private:
public:
  ThinkInk_420_Grayscale4_T2(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST,
                       int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY = -1)
      : Adafruit_IL0398(300, 400, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_420_Grayscale4_T2(int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
                       int8_t BUSY = -1, SPIClass *spi = &SPI)
      : Adafruit_IL0398(300, 400, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_EPD::begin(true);
    setColorBuffer(0, true); // layer 0 uninverted
    setBlackBuffer(1, true); // layer 1 uninverted

    if (mode == THINKINK_MONO) {
      layer_colors[EPD_WHITE] = 0b00;
      layer_colors[EPD_BLACK] = 0b01;
      layer_colors[EPD_RED] = 0b01;
      layer_colors[EPD_GRAY] = 0b01;
      layer_colors[EPD_LIGHT] = 0b00;
      layer_colors[EPD_DARK] = 0b01;
    }

    default_refresh_delay = 1000;
    setRotation(1);
    powerDown();
  };
};

#endif // _THINKINK_420_GRAY4_T2_H
