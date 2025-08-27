#ifndef _THINKINK_352_QUADCOLOR_AJHE5_H
#define _THINKINK_352_QUADCOLOR_AJHE5_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

class ThinkInk_352_Quadcolor_AJHE5 : public Adafruit_JD79667 {
 public:
  ThinkInk_352_Quadcolor_AJHE5(int16_t SID, int16_t SCLK, int16_t DC,
                               int16_t RST, int16_t CS, int16_t SRCS,
                               int16_t MISO, int16_t BUSY = -1)
      : Adafruit_JD79667(180, 384, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_352_Quadcolor_AJHE5(int16_t DC, int16_t RST, int16_t CS,
                               int16_t SRCS, int16_t BUSY = -1,
                               SPIClass* spi = &SPI)
      : Adafruit_JD79667(180, 384, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_QUADCOLOR) {
    Adafruit_JD79667::begin(true);

    inkmode = mode; // Preserve ink mode for ImageReader or others

    default_refresh_delay = 13000;
    setRotation(1);
    powerDown();
  }
};

#endif // _THINKINK_352_Quad
