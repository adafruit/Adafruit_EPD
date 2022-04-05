#ifndef _THINKINK_290_TRICOLOR_Z10_H
#define _THINKINK_290_TRICOLOR_Z10_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

class ThinkInk_290_Tricolor_Z10 : public Adafruit_IL0373 {
public:
  ThinkInk_290_Tricolor_Z10(int16_t SID, int16_t SCLK, int16_t DC, int16_t RST,
                            int16_t CS, int16_t SRCS, int16_t MISO,
                            int16_t BUSY = -1)
      : Adafruit_IL0373(296, 128, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_290_Tricolor_Z10(int16_t DC, int16_t RST, int16_t CS, int16_t SRCS,
                            int16_t BUSY = -1, SPIClass *spi = &SPI)
      : Adafruit_IL0373(296, 128, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    Adafruit_IL0373::begin(true);
    setColorBuffer(0, true); // layer 0 uninverted
    setBlackBuffer(1, true); // layer 1 uninverted

    inkmode = mode; // Preserve ink mode for ImageReader or others

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b10;
    layer_colors[EPD_RED] = 0b01;
    layer_colors[EPD_GRAY] = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b10;

    default_refresh_delay = 13000;

    powerDown();
  }
};

#endif // _THINKINK_290_TRICOLOR_H
