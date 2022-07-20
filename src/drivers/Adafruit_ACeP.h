#ifndef LIB_ADAFRUIT_ACEP
#define LIB_ADAFRUIT_ACEP

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define ACEP_PANEL_SETTING 0x00
#define ACEP_POWER_SETTING 0x01
#define ACEP_POWER_OFF 0x02
#define ACEP_POWER_OFF_SEQUENCE 0x03
#define ACEP_POWER_ON 0x04
#define ACEP_BOOSTER_SOFT_START 0x06
#define ACEP_DEEP_SLEEP 0x07
#define ACEP_DTM 0x10
#define ACEP_DISPLAY_REFRESH 0x12
#define ACEP_PLL 0x30
#define ACEP_TSE 0x40
#define ACEP_CDI 0x50
#define ACEP_TCON 0x60
#define ACEP_RESOLUTION 0x61
#define ACEP_PWS 0xE3

#define ACEP_COLOR_BLACK 0x0  /// 000
#define ACEP_COLOR_WHITE 0x1  ///	001
#define ACEP_COLOR_GREEN 0x2  ///	010
#define ACEP_COLOR_BLUE 0x3   ///	011
#define ACEP_COLOR_RED 0x4    ///	100
#define ACEP_COLOR_YELLOW 0x5 ///	101
#define ACEP_COLOR_ORANGE 0x6 ///	110

/**************************************************************************/
/*!
    @brief  Class for interfacing with ACEP EPD drivers
*/
/**************************************************************************/
class Adafruit_ACEP : public Adafruit_EPD {
public:
  Adafruit_ACEP(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                int16_t BUSY = -1);
  Adafruit_ACEP(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                int16_t SRCS, int16_t BUSY = -1, SPIClass *spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void powerDown();
  void update();
  void display(bool sleep = true);

  void clearBuffer();
  void clearDisplay();
  void deGhost();
  void drawPixel(int16_t x, int16_t y, uint16_t color);

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};

#endif
