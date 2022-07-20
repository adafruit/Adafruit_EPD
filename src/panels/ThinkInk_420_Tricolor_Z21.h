#ifndef _THINKINK_420_TRICOLOR_Z21_H
#define _THINKINK_420_TRICOLOR_Z21_H

#include "Adafruit_EPD.h"

class ThinkInk_420_Tricolor_Z21 : public Adafruit_UC8276 {
private:
public:
  ThinkInk_420_Tricolor_Z21(int16_t SID, int16_t SCLK, int16_t DC, int16_t RST,
                            int16_t CS, int16_t SRCS, int16_t MISO,
                            int16_t BUSY = -1)
      : Adafruit_UC8276(300, 400, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_420_Tricolor_Z21(int16_t DC, int16_t RST, int16_t CS, int16_t SRCS,
                            int16_t BUSY = -1, SPIClass *spi = &SPI)
      : Adafruit_UC8276(300, 400, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    (void)mode;
    Adafruit_EPD::begin(true);
    setBlackBuffer(0, true);
    setColorBuffer(1, false);

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b10;
    layer_colors[EPD_GRAY] = 0b10;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 13000;
    setRotation(1);
    powerDown();
  };
};

#endif // _THINKINK_420_TRI
