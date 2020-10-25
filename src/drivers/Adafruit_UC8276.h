#ifndef LIB_ADAFRUIT_UC8276
#define LIB_ADAFRUIT_UC8276

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define UC8276_PANELSETTING 0x00
#define UC8276_POWEROFF 0x02
#define UC8276_POWERON 0x04
#define UC8276_DEEPSLEEP 0x07
#define UC8276_DISPLAYREFRESH 0x12
#define UC8276_WRITE_RAM1 0x10
#define UC8276_WRITE_RAM2 0x13
#define UC8276_WRITE_VCOM 0x50
#define UC8276_GET_STATUS 0x71

/*
#define UC8276_GATE_VOLTAGE 0x03
#define UC8276_PROGOTP_INITIAL 0x08
#define UC8276_PROGREG_INITIAL 0x09
#define UC8276_READREG_INITIAL 0x0A
#define UC8276_BOOST_SOFTSTART 0x0C
#define UC8276_DATA_MODE 0x11
#define UC8276_SW_RESET 0x12
#define UC8276_TEMP_CONTROL 0x18
#define UC8276_TEMP_WRITE 0x1A

#define UC8276_DISP_CTRL1 0x21
#define UC8276_DISP_CTRL2 0x22

#define UC8276_READ_OTP 0x2D
#define UC8276_READ_STATUS 0x2F
#define UC8276_WRITE_LUT 0x32
#define UC8276_WRITE_BORDER 0x3C
#define UC8276_SET_RAMXPOS 0x44
#define UC8276_SET_RAMYPOS 0x45
#define UC8276_SET_RAMXCOUNT 0x4E
#define UC8276_SET_RAMYCOUNT 0x4F
#define UC8276_SET_ANALOGBLOCK 0x74
#define UC8276_SET_DIGITALBLOCK 0x7E
*/

/**************************************************************************/
/*!
    @brief  Class for interfacing with UC8276 EPD drivers
*/
/**************************************************************************/
class Adafruit_UC8276 : public Adafruit_EPD {
public:
  Adafruit_UC8276(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                   int8_t BUSY = -1);
  Adafruit_UC8276(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);

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
