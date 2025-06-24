#ifndef _THINKINK_290_MONO_M06_H
#define _THINKINK_290_MONO_M06_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

class ThinkInk_290_Mono_M06 : public Adafruit_UC8151D {
 public:
  ThinkInk_290_Mono_M06(int16_t SID, int16_t SCLK, int16_t DC, int16_t RST,
                        int16_t CS, int16_t SRCS, int16_t MISO,
                        int16_t BUSY = -1)
      : Adafruit_UC8151D(296, 128, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_290_Mono_M06(int16_t DC, int16_t RST, int16_t CS, int16_t SRCS,
                        int16_t BUSY = -1, SPIClass* spi = &SPI)
      : Adafruit_UC8151D(296, 128, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_UC8151D::begin(true);

    setColorBuffer(1, true); // layer 1 uninverted
    setBlackBuffer(1, true); // only one buffer

    inkmode = mode; // Preserve ink mode for ImageReader or others

    _epd_init_code = uc8151d_monofull_init_code;
    _epd_partial_init_code = uc8151d_partial_init_code;
    _epd_partial_lut_code = uc8151d_partialmono_lut;

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b01;
    layer_colors[EPD_GRAY] = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 1000;
    setRotation(0);
    powerDown();
  }
};

#endif // _THINKINK_290_MONO_M06_H
