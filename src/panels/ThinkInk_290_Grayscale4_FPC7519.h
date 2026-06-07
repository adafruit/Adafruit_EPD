#ifndef _THINKINK_290_GRAYSCALE4_FPC7519_H
#define _THINKINK_290_GRAYSCALE4_FPC7519_H

// MagTag 2.9" grayscale panel — FPC-7519rev.b (User ID first byte 0x44 or 0xca)
// Uses the GxEPD2_4G (GDEM029T94) waveform LUT embedded in
// ssd1680_fpc7519_init_code. For the original FPC-A005 panel use
// ThinkInk_290_Grayscale4_EAAMFGN instead.

class ThinkInk_290_Grayscale4_FPC7519 : public Adafruit_SSD1680 {
 public:
  ThinkInk_290_Grayscale4_FPC7519(int16_t SID, int16_t SCLK, int16_t DC,
                                  int16_t RST, int16_t CS, int16_t SRCS,
                                  int16_t MISO, int16_t BUSY = -1)
      : Adafruit_SSD1680(296, 128, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_290_Grayscale4_FPC7519(int16_t DC, int16_t RST, int16_t CS,
                                  int16_t SRCS, int16_t BUSY = -1,
                                  SPIClass* spi = &SPI)
      : Adafruit_SSD1680(296, 128, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_GRAYSCALE4) {
    Adafruit_SSD1680::begin(true);
    inkmode = mode;

    // _xram_offset stays at 1 (default): colstart = 8 pixels for FPC-7519rev.b
    // _epd_lut_code stays NULL: LUT is embedded in ssd1680_fpc7519_init_code

    if (mode == THINKINK_GRAYSCALE4) {
      setColorBuffer(1, false);
      setBlackBuffer(0, false);

      _epd_init_code = ssd1680_fpc7519_init_code;
      _display_update_val = 0xC7; // Display Mode 1 required with custom LUT

      layer_colors[EPD_WHITE] = 0b00;
      layer_colors[EPD_BLACK] = 0b11;
      layer_colors[EPD_RED] = 0b01;
      layer_colors[EPD_GRAY] = 0b10;
      layer_colors[EPD_LIGHT] = 0b01;
      layer_colors[EPD_DARK] = 0b10;

      default_refresh_delay = 3000;

    } else if (mode == THINKINK_MONO) {
      setColorBuffer(0, true);
      setBlackBuffer(0, true);

      _epd_init_code = ssd1680_fpc7519_init_code;
      _display_update_val = 0xC7;

      layer_colors[EPD_WHITE] = 0b00;
      layer_colors[EPD_BLACK] = 0b01;
      layer_colors[EPD_RED] = 0b01;
      layer_colors[EPD_GRAY] = 0b01;
      layer_colors[EPD_LIGHT] = 0b00;
      layer_colors[EPD_DARK] = 0b01;

      default_refresh_delay = 800;
    }

    powerDown();
  }
};

#endif // _THINKINK_290_GRAYSCALE4_FPC7519_H
