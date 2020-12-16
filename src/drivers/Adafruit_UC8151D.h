#ifndef LIB_ADAFRUIT_UC8151D
#define LIB_ADAFRUIT_UC8151D

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10

#define UC8151D_PSR 0x00
#define UC8151D_PWR 0x01
#define UC8151D_POF 0x02
#define UC8151D_PFS 0x03
#define UC8151D_PON 0x04
#define UC8151D_PMEAS 0x05
#define UC8151D_BTST 0x06
#define UC8151D_DSLP 0x07
#define UC8151D_DTM1 0x10
#define UC8151D_DSP 0x11
#define UC8151D_DRF 0x12
#define UC8151D_DTM2 0x13
#define UC8151D_AUTO 0x17
#define UC8151D_LUTOPT 0x2A
#define UC8151D_PLL 0x30
#define UC8151D_TSC 0x40
#define UC8151D_TSE 0x41
#define UC8151D_TSW 0x42
#define UC8151D_TSR 0x43
#define UC8151D_PBC 0x44
#define UC8151D_CDI 0x50
#define UC8151D_LPD 0x51
#define UC8151D_TRES 0x65
#define UC8151D_GSST 0x70
#define UC8151D_REV 0x70
#define UC8151D_FLG 0x71
#define UC8151D_AMV 0x80
#define UC8151D_VV 0x81
#define UC8151D_VDCS 0x82
#define UC8151D_PTL 0x90
#define UC8151D_PTIN 0x91
#define UC8151D_PTOUT 0x92
#define UC8151D_PGM 0xA0
#define UC8151D_APG 0xA1
#define UC8151D_ROTP 0xA2
#define UC8151D_CCSET 0xE0
#define UC8151D_PWS 0xE3
#define UC8151D_LVSEL 0xE4
#define UC8151D_TSSET 0xE5

/**************************************************************************/
/*!
    @brief  Class for interfacing with UC8151D EPD drivers
*/
/**************************************************************************/
class Adafruit_UC8151D : public Adafruit_EPD {
public:
  Adafruit_UC8151D(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                   int8_t BUSY = -1);
  Adafruit_UC8151D(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void powerDown();
  void update();

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};

#endif
