#ifndef _THINKINK_370_TRI_BABMFGNR_H
#define _THINKINK_370_TRI_BABMFGNR_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

const uint8_t uc8253_tricolor_init_code[]{UC8253_POWERON,
                                          0, // soft reset
                                          0xFF,
                                          50, // busy wait
                                          UC8253_PANELSETTING,
                                          2,
                                          0b11001111,
                                          0x8D,
                                          0xFE};

class ThinkInk_370_Tricolor_BABMFGNR : public Adafruit_UC8253 {
 public:
  ThinkInk_370_Tricolor_BABMFGNR(int16_t SID, int16_t SCLK, int16_t DC,
                                 int16_t RST, int16_t CS, int16_t SRCS,
                                 int16_t MISO, int16_t BUSY = -1)
      : Adafruit_UC8253(416, 240, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_370_Tricolor_BABMFGNR(int16_t DC, int16_t RST, int16_t CS,
                                 int16_t SRCS, int16_t BUSY = -1,
                                 SPIClass* spi = &SPI)
      : Adafruit_UC8253(416, 240, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    Adafruit_UC8253::begin(true);

    setColorBuffer(0, true);
    setBlackBuffer(1, false);

    inkmode = mode; // Preserve ink mode for ImageReader or others
    _epd_init_code = uc8253_tricolor_init_code;

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b10;
    layer_colors[EPD_RED] = 0b01;
    layer_colors[EPD_GRAY] = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 13000;
    setRotation(0);
    // DO NOT power down again!
  }
};

#endif // _THINKINK_370_TRI_BABMFGNR_H
