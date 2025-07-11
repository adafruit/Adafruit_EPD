#ifndef _THINKINK_583_MONO_AAAMFGN_H
#define _THINKINK_583_MONO_AAAMFGN_H

#include "Adafruit_EPD.h"

const uint8_t uc8179_mono_init_code[]{UC8179_POWERSETTING,
                                      4,
                                      0x07, // VGH=20V
                                      0x07, // VGL=-20V
                                      0x3F, // VDH=15V
                                      0x3F, // VDL=-15V

                                      UC8179_POWERON,
                                      0,
                                      0xFF,
                                      100, // busy wait

                                      UC8179_PANELSETTING,
                                      1,
                                      0b010111, // BW OTP LUT

                                      UC8179_TRES,
                                      4,
                                      0x02,
                                      0x88,
                                      0x01,
                                      0xE0,

                                      UC8179_DUALSPI,
                                      1,
                                      0x00,

                                      UC8179_WRITE_VCOM,
                                      2,
                                      0x10,
                                      0x07,
                                      UC8179_TCON,
                                      1,
                                      0x22,

                                      0xFE};

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

    _epd_init_code = uc8179_mono_init_code;

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
