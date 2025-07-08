#ifndef LIB_ADAFRUIT_UC8179
#define LIB_ADAFRUIT_UC8179

#include <Arduino.h>

#include "Adafruit_EPD.h"

#define UC8179_PANELSETTING 0x00
#define UC8179_POWERSETTING 0x01
#define UC8179_POWEROFF 0x02
#define UC8179_POWERON 0x04
#define UC8179_DEEPSLEEP 0x07
#define UC8179_WRITE_RAM1 0x10
#define UC8179_DATASTOP 0x11
#define UC8179_DISPLAYREFRESH 0x12
#define UC8179_WRITE_RAM2 0x13
#define UC8179_DUALSPI 0x15

#define UC8179_WRITE_VCOM 0x50
#define UC8179_TCON 0x60
#define UC8179_TRES 0x61
#define UC8179_GET_STATUS 0x71

/**************************************************************************/
/*!
    @brief  Class for interfacing with UC8179 EPD drivers
*/
/**************************************************************************/
class Adafruit_UC8179 : public Adafruit_EPD {
 public:
  Adafruit_UC8179(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                  int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                  int16_t BUSY = -1);
  Adafruit_UC8179(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                  int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void update(void);
  void powerDown();

 protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void busy_wait();
};

#endif
