#ifndef _THINKINK_750_TRI_AABMFGNR_H
#define _THINKINK_750_TRI_AABMFGNR_H

#include "Adafruit_EPD.h"

// clang-format off

const uint8_t uc8179_750_tricolor_init_code[] {
    UC8179_POWERSETTING, 4, 
	0x07, // VGH=20V
	0x07, // VGL=-20V
	0x3F, // VDH=15V
	0x3F, // VDL=-15V

    UC8179_POWERON, 0,
	0xFF, 100,          // busy wait
	
	UC8179_PANELSETTING, 1,
	0b000111, // Tricolor OTP LUT

    UC8179_TRES, 4,
	0x03, 0x20, 0x01, 0xE0,
	
	UC8179_DUALSPI, 1, 0x00,

    UC8179_WRITE_VCOM, 2, 0x90, 0x07,
    UC8179_TCON, 1, 0x22,

    0xFE};

// clang-format on

class ThinkInk_750_Tricolor_AABMFGNR : public Adafruit_UC8179 {
 private:
 public:
  ThinkInk_750_Tricolor_AABMFGNR(int16_t SID, int16_t SCLK, int16_t DC,
                                 int16_t RST, int16_t CS, int16_t SRCS,
                                 int16_t MISO, int16_t BUSY = -1)
      : Adafruit_UC8179(800, 480, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_750_Tricolor_AABMFGNR(int16_t DC, int16_t RST, int16_t CS,
                                 int16_t SRCS, int16_t BUSY = -1,
                                 SPIClass* spi = &SPI)
      : Adafruit_UC8179(800, 480, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    Adafruit_EPD::begin(true);

    setBlackBuffer(0, false);
    setColorBuffer(1, false);

    _epd_init_code = uc8179_750_tricolor_init_code;

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b10;
    layer_colors[EPD_GRAY] = 0b10;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 13000;
    setRotation(0);
    // DO NOT power down again!
  };
};

#endif // _THINKINK_750_TRI_AABMFGNR
