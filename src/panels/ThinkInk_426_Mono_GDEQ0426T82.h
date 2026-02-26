#ifndef _THINKINK_426_MONO_GDEQ0426T82_H
#define _THINKINK_426_MONO_GDEQ0426T82_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

class ThinkInk_426_Mono_GDEQ0426T82 : public Adafruit_SSD1677 {
 public:
  ThinkInk_426_Mono_GDEQ0426T82(int16_t SID, int16_t SCLK, int16_t DC,
                                 int16_t RST, int16_t CS, int16_t SRCS,
                                 int16_t MISO, int16_t BUSY = -1)
      : Adafruit_SSD1677(800, 480, SID, SCLK, DC, RST, CS, SRCS, MISO,
                         BUSY) {};

  ThinkInk_426_Mono_GDEQ0426T82(int16_t DC, int16_t RST, int16_t CS,
                                 int16_t SRCS, int16_t BUSY = -1,
                                 SPIClass *spi = &SPI)
      : Adafruit_SSD1677(800, 480, DC, RST, CS, SRCS, BUSY, spi) {};

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_SSD1677::begin(true);
    setColorBuffer(0, true);  // layer 0 inverted
    setBlackBuffer(0, true);  // only one logical buffer for mono

    inkmode = mode;

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b01;
    layer_colors[EPD_GRAY] = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 15000;

    powerDown();
  }
};

#endif // _THINKINK_426_MONO_GDEQ0426T82_H