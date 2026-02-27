#ifndef LIB_ADAFRUIT_SSD1677
#define LIB_ADAFRUIT_SSD1677

#include <Arduino.h>

#include "Adafruit_EPD.h"

// Initialization and reset
#define SSD1677_SOFT_RESET 0x12
#define SSD1677_BOOSTER_SOFT_START 0x0C
#define SSD1677_DRIVER_OUTPUT_CONTROL 0x01
#define SSD1677_BORDER_WAVEFORM 0x3C
#define SSD1677_TEMP_SENSOR_CONTROL 0x18

// RAM and buffer management
#define SSD1677_DATA_ENTRY_MODE 0x11
#define SSD1677_SET_RAM_X_RANGE 0x44
#define SSD1677_SET_RAM_Y_RANGE 0x45
#define SSD1677_SET_RAM_X_COUNTER 0x4E
#define SSD1677_SET_RAM_Y_COUNTER 0x4F
#define SSD1677_WRITE_RAM_BW 0x24
#define SSD1677_WRITE_RAM_RED 0x26
#define SSD1677_AUTO_WRITE_BW_RAM 0x46
#define SSD1677_AUTO_WRITE_RED_RAM 0x47

// Display update and refresh
#define SSD1677_DISPLAY_UPDATE_CTRL1 0x21
#define SSD1677_DISPLAY_UPDATE_CTRL2 0x22
#define SSD1677_MASTER_ACTIVATION 0x20

// LUT and voltage settings
#define SSD1677_WRITE_LUT 0x32
#define SSD1677_GATE_VOLTAGE 0x03
#define SSD1677_SOURCE_VOLTAGE 0x04
#define SSD1677_WRITE_VCOM 0x2C
#define SSD1677_WRITE_TEMP 0x1A

// Power management
#define SSD1677_DEEP_SLEEP 0x10

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1677 EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1677 : public Adafruit_EPD {
 public:
  Adafruit_SSD1677(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                   int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                   int16_t BUSY = -1);
  Adafruit_SSD1677(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                   int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void powerDown();
  void update();

  void displayPartial(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void displayHalf();

 protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();

  void softReset();

  uint8_t _display_ctrl1_val = 0x40;  // default: bypass RED
  uint8_t _display_update_val = 0xF7; // default: full refresh
  bool _grayscale_preclear = false;   // if true, do BW clear before grayscale

  void setCustomLUT(const uint8_t* lutData);
  void clearCustomLUT();
  bool _customLutActive = false;
};

#endif