#ifndef _THINKINK_290_TRICOLOR_Z94_H
#define _THINKINK_290_TRICOLOR_Z94_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

class ThinkInk_290_Tricolor_Z94 : public Adafruit_SSD1680 {
 public:
  // colstart: left RAM-X offset in pixels (multiple of 8). Default 0 keeps the
  // shipped behavior; pass 8 for panels that show an 8px speckled strip along
  // one long edge.
  ThinkInk_290_Tricolor_Z94(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST,
                            int8_t CS, int8_t SRCS, int8_t MISO,
                            int8_t BUSY = -1, int16_t colstart = 0)
      : Adafruit_SSD1680(296, 128, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY),
        _colstart(colstart) {};

  ThinkInk_290_Tricolor_Z94(int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
                            int8_t BUSY = -1, SPIClass* spi = &SPI,
                            int16_t colstart = 0)
      : Adafruit_SSD1680(296, 128, DC, RST, CS, SRCS, BUSY, spi),
        _colstart(colstart) {};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    // RAM-X byte offset derived from colstart (pixels). Default 0 keeps the
    // shipped behavior; an 8px colstart (= 1 byte) removes the unwritten strip.
    _xram_offset = _colstart / 8;

    Adafruit_SSD1680::begin(true);
    setBlackBuffer(0, true);
    setColorBuffer(1, false);

    inkmode = mode; // Preserve ink mode for ImageReader or others

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b01;
    layer_colors[EPD_RED] = 0b10;
    layer_colors[EPD_GRAY] = 0b10;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b01;

    default_refresh_delay = 13000;
    setRotation(0);
    powerDown();
  }

 private:
  int16_t _colstart = 0; // left RAM-X offset in pixels (0 default, or 8)
};

#endif // _THINKINK_290 Z94
