#ifndef _THINKINK_426_GRAYSCALE4_GDEQ_H
#define _THINKINK_426_GRAYSCALE4_GDEQ_H

// This file is #included by Adafruit_ThinkInk.h and does not need to
// #include anything else to pick up the EPD header or ink mode enum.

// clang-format off

static const uint8_t ti_426_gray4_init_code[] {
  SSD1677_SOFT_RESET, 0,
    0xFF, 200,
    SSD1677_TEMP_SENSOR_CONTROL, 1, 0x80,
    SSD1677_BOOSTER_SOFT_START, 5,
      0xAE, 0xC7, 0xC3, 0xC0, 0x40,
    SSD1677_DRIVER_OUTPUT_CONTROL, 3,
      0xDF, 0x01, 0x02,
    SSD1677_BORDER_WAVEFORM, 1, 0x01,
    SSD1677_DATA_ENTRY_MODE, 1, 0x03,
    0xFE};

// Grayscale LUT code — includes the 105-byte LUT AND voltage commands.
// Both must be reloaded together after any OTP LUT load (0xF7) since
// that overwrites the LUT register and may reset voltage settings.
static const uint8_t ti_426_gray4_lut_code[] {
  SSD1677_WRITE_LUT, 105,
    // VS waveforms (5 groups x 10 bytes)
    // 00 white
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // 01 light gray
    0x54, 0x54, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // 10 dark gray
    0xAA, 0xA0, 0xA8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // 11 black
    0xA2, 0x22, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // L4 (VCOM)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // TP/RP groups (10 groups x 5 bytes)
    0x01, 0x01, 0x01, 0x01, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    // Frame rate
    0x8F, 0x8F, 0x8F, 0x8F, 0x8F,

  // Voltage commands — must be reloaded with LUT
  SSD1677_GATE_VOLTAGE, 1, 0x17,
  SSD1677_SOURCE_VOLTAGE, 3, 0x41, 0xA8, 0x32,
  SSD1677_WRITE_VCOM, 1, 0x30,

  0xFE};

static const uint8_t ti_426_monofull_init_code[] {
  SSD1677_SOFT_RESET, 0,
    0xFF, 200,
    SSD1677_TEMP_SENSOR_CONTROL, 1, 0x80,
    SSD1677_BOOSTER_SOFT_START, 5,
      0xAE, 0xC7, 0xC3, 0xC0, 0x40,
    SSD1677_DRIVER_OUTPUT_CONTROL, 3,
      0xDF, 0x01, 0x02,
    SSD1677_BORDER_WAVEFORM, 1, 0x01,
    SSD1677_DATA_ENTRY_MODE, 1, 0x03,
    0xFE};

// clang-format on

class ThinkInk_426_Grayscale4_GDEQ : public Adafruit_SSD1677 {
 public:
  ThinkInk_426_Grayscale4_GDEQ(int16_t SID, int16_t SCLK, int16_t DC,
                                      int16_t RST, int16_t CS, int16_t SRCS,
                                      int16_t MISO, int16_t BUSY = -1)
      : Adafruit_SSD1677(800, 480, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_426_Grayscale4_GDEQ(int16_t DC, int16_t RST, int16_t CS,
                                      int16_t SRCS, int16_t BUSY = -1,
                                      SPIClass* spi = &SPI)
      : Adafruit_SSD1677(800, 480, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_MONO) {
    Adafruit_SSD1677::begin(true);

    inkmode = mode;

    if (mode == THINKINK_GRAYSCALE4) {
      setBlackBuffer(0, true); // LSB inverted
      setColorBuffer(1, true); // MSB inverted

      _epd_init_code = ti_426_gray4_init_code;
      _epd_lut_code = ti_426_gray4_lut_code;

      // Inverted buffers: clearBuffer fills 0xFF = all 1s in RAM = white
      // drawPixel flips bits: layer_colors bit 1 → RAM 0, bit 0 → RAM 1
      // OTP BW: RAM 0=black, RAM 1=white
      // Grayscale LUT groups by {RED_bit, BW_bit}:
      //   {0,0}=black  {0,1}=light gray  {1,0}=dark gray  {1,1}=white
      layer_colors[EPD_WHITE] = 0b00; // RAM {1,1} = white
      layer_colors[EPD_BLACK] = 0b11; // RAM {0,0} = black
      layer_colors[EPD_LIGHT] = 0b10; // RAM {0,1} = light gray
      layer_colors[EPD_DARK] = 0b01;  // RAM {1,0} = dark gray
      layer_colors[EPD_RED] = 0b11;
      layer_colors[EPD_GRAY] = 0b10;

      _display_ctrl1_val = 0x00;  // NORMAL: use both BW and RED
      _display_update_val = 0xCF; // custom LUT, full power cycle
      _grayscale_preclear = true;

    } else {
      // THINKINK_MONO
      setColorBuffer(0, true);
      setBlackBuffer(0, true);

      _epd_init_code = ti_426_monofull_init_code;
      _epd_lut_code = NULL;

      layer_colors[EPD_WHITE] = 0b00;
      layer_colors[EPD_BLACK] = 0b01;
      layer_colors[EPD_RED] = 0b01;
      layer_colors[EPD_GRAY] = 0b01;
      layer_colors[EPD_LIGHT] = 0b00;
      layer_colors[EPD_DARK] = 0b01;

      _display_ctrl1_val = 0x40;  // bypass RED
      _display_update_val = 0xF7; // full refresh with OTP LUT
      _grayscale_preclear = false;
    }

    default_refresh_delay = 15000;

    powerDown();
  }
};

#endif // _THINKINK_426_GRAYSCALE4_GDEQ_H
