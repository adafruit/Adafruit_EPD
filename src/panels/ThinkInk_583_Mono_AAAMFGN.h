#ifndef _THINKINK_583_MONO_AAAMFGN_H
#define _THINKINK_583_MONO_AAAMFGN_H

#include "Adafruit_EPD.h"

class ThinkInk_583_Mono_AAAMFGN : public Adafruit_UC8179 {
 private:
 public:
  ThinkInk_583_Mono_AAAMFGN(int16_t SID, int16_t SCLK, int16_t DC, int16_t RST,
                        int16_t CS, int16_t SRCS, int16_t MISO,
                        int16_t BUSY = -1)
      : Adafruit_UC8179(648, 480, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_583_Mono_AAAMFGN(int16_t DC, int16_t RST, int16_t CS, int16_t SRCS,
                        int16_t BUSY = -1, SPIClass* spi = &SPI)
      : Adafruit_UC8179(648, 480, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_EPD::begin(true);
    setColorBuffer(1, false); // layer 1 uninverted
    setBlackBuffer(1, false); // only one buffer

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b10;
    layer_colors[EPD_GRAY] = 0b10;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 3000;
    setRotation(0);
    powerDown();
  };
};

#endif // _THINKINK_583_MONO_AAAMFGN
