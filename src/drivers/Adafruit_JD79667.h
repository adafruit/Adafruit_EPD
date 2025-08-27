#ifndef LIB_ADAFRUIT_JD79667
#define LIB_ADAFRUIT_JD79667

#include <Arduino.h>

#include "Adafruit_EPD.h"

#define JD79667_PANEL_SETTING 0x00
#define JD79667_POWER_SETTING 0x01
#define JD79667_POWER_OFF 0x02
#define JD79667_POWER_ON 0x04
#define JD79667_BOOSTER_SOFTSTART 0x06
#define JD79667_DATA_START_XMIT 0x10
#define JD79667_DISPLAY_REFRESH 0x12
#define JD79667_DEEP_SLEEP 0x07
#define JD79667_PLL_CONTROL 0x30
#define JD79667_CDI 0x50
#define JD79667_RESOLUTION 0x61

#define JD79667_BLACK 0b00
#define JD79667_WHITE 0b01
#define JD79667_YELLOW 0b10
#define JD79667_RED 0b11

/**************************************************************************/
/*!
    @brief  Class for interfacing with JD79667 EPD drivers
*/
/**************************************************************************/
class Adafruit_JD79667 : public Adafruit_EPD {
 public:
  Adafruit_JD79667(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                   int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                   int16_t BUSY = -1);
  Adafruit_JD79667(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                   int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void update(void);
  void powerDown();

  void clearBuffer();
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void hardwareReset(void);

 protected:
  void busy_wait();
  void setRAMAddress(uint16_t x, uint16_t y);
  uint8_t writeRAMCommand(uint8_t index);
};

#endif
