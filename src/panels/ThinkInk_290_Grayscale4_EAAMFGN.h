#ifndef _THINKINK_290_GRAYSCALE4_EAAMFGN_H
#define _THINKINK_290_GRAYSCALE4_EAAMFGN_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

// clang-format off

static const uint8_t ti_290mfgn_monofull_init_code[] {
  SSD1680_SW_RESET, 0, // soft reset
    0xFF, 20,          // busy wait
    SSD1680_DRIVER_CONTROL, 3, 0x27, 0x01, 0x00, // Driver output
    SSD1680_DATA_MODE, 1, 0x03, // Ram data entry mode
    SSD1680_DISP_CTRL1, 2, 0x00, 0x80, // Display update

    SSD1680_SET_RAMXCOUNT, 1, 1,
    SSD1680_SET_RAMYCOUNT, 2, 0, 0,
    SSD1680_WRITE_BORDER, 1, 0x05, // border color

    //0x3F, 1, 0x22, // End of LUT normal
    //SSD1680_GATE_VOLTAGE, 1, 0x17, // Set gate voltage 
    //SSD1680_SOURCE_VOLTAGE, 3, 0x41, 0x00, 0x32,   // Set source voltage
    //SSD1680_WRITE_VCOM, 1, 0x36,   // Vcom Voltage

    SSD1680_DISP_CTRL2, 1, 0x20,  // Load LUT from OTP (default mono)

    0xFE
};

static const uint8_t ti_290mfgn_monofull_lut_code[] = {
  0x32, 153,
  0x80, 0x66, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 
  0x10, 0x66, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 
  0x80, 0x66, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 
  0x10, 0x66, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x20, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x14, 0x8, 0x0, 0x0, 
  0x0, 0x0, 0x1, 0xA, 0xA, 0x0, 0xA, 0xA, 
  0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x14, 0x8, 0x0, 0x1, 
  0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x0, 0x0, 0x0, 
  0xFE
};


// clang-format on

class ThinkInk_290_Grayscale4_EAAMFGN : public Adafruit_SSD1680 {
 public:
 ThinkInk_290_Grayscale4_EAAMFGN(int16_t SID, int16_t SCLK, int16_t DC, 
                                  int16_t RST, int16_t CS, int16_t SRCS, 
                                 int16_t MISO, int16_t BUSY = -1)
   : Adafruit_SSD1680(296, 128, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};
  
 ThinkInk_290_Grayscale4_EAAMFGN(int16_t DC, int16_t RST, int16_t CS, 
                                 int16_t SRCS, int16_t BUSY = -1, 
                                 SPIClass* spi = &SPI)
   : Adafruit_SSD1680(296, 128, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_GRAYSCALE4) {
    Adafruit_SSD1680::begin(true);
    setColorBuffer(0, true); // layer 0 uninverted
    setBlackBuffer(1, true); // layer 1 uninverted

    inkmode = mode; // Preserve ink mode for ImageReader or others
    _xram_offset = 0;

    if (mode == THINKINK_GRAYSCALE4) {
      //_epd_init_code = ti_290t5_gray4_init_code;
      //_epd_lut_code = ti_290t5_gray4_lut_code;

      layer_colors[EPD_WHITE] = 0b00;
      layer_colors[EPD_BLACK] = 0b11;
      layer_colors[EPD_RED] = 0b01;
      layer_colors[EPD_GRAY] = 0b10;
      layer_colors[EPD_LIGHT] = 0b01;
      layer_colors[EPD_DARK] = 0b10;

      default_refresh_delay = 3000;

    } else if (mode == THINKINK_MONO) {
      setColorBuffer(0, true); // layer 0 uninverted
      setBlackBuffer(0, true); // only one

      _epd_init_code = ti_290mfgn_monofull_init_code;
      //_epd_partial_init_code = ti_290t5_monopart_init_code;
      //_epd_partial_lut_code = ti_290t5_monopart_lut_code;

      layer_colors[EPD_WHITE] = 0b00;
      layer_colors[EPD_BLACK] = 0b01;
      layer_colors[EPD_RED] = 0b01;
      layer_colors[EPD_GRAY] = 0b01;
      layer_colors[EPD_LIGHT] = 0b00;
      layer_colors[EPD_DARK] = 0b01;

      default_refresh_delay = 800;
    }

    setRotation(0);

    powerDown();
  }
};

#endif // _THINKINK_290_GRAYSCALE4_EAAMFGN_H
