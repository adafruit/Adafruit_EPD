#ifndef LIB_ADAFRUIT_UC8151D
#define LIB_ADAFRUIT_UC8151D

#include <Arduino.h>

#include "Adafruit_EPD.h"

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

// clang-format off

const uint8_t uc8151d_monofull_init_code[] {
  UC8151D_PON, 0,
    0xFF, 10,
    UC8151D_PSR, 1, 0x1F,
    UC8151D_CDI, 1, 0x97,
    0xFE};

const uint8_t uc8151d_trifull_init_code[] {
  UC8151D_PON, 0,
    0xFF, 10,
    UC8151D_PSR, 2, 0x0F, 0x89,
    0x61, 3, 0x80, 0x01, 0x28,
    UC8151D_CDI, 1, 0x77,
    0xFE};


const uint8_t uc8151d_partial_init_code[] {
  UC8151D_PWR, 5, 0x03, 0x00, 0x2B, 0x2B, 0x03,     //POWER SETTING 
    UC8151D_BTST, 3, 0x17, 0x17, 0x17, //boost soft start
    UC8151D_PON, 0,
    0xFF, 10,
    UC8151D_PSR, 1, 0xBF,  //LUT from OTP 128x296
    UC8151D_PLL, 1, 0x3C,  // 3A 100HZ   29 150Hz 39 200HZ 31 171HZ
    0x61, 3, 0x80, 0x01, 0x28, //resolution setting  
    UC8151D_VDCS, 1, 0x12,    //vcom_DC setting
    UC8151D_CDI, 1, 0x47,
    0xFE};

const uint8_t uc8151d_partialmono_lut[] = {
  // const unsigned char lut_vcom1[]
  0x20, 44,
  0x00  ,0x19 ,0x01 ,0x00 ,0x00 ,0x01,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00
  ,0x00 ,0x00,

  // const unsigned char lut_ww1[]
  0x21, 42,
  0x00  ,0x19 ,0x01 ,0x00 ,0x00 ,0x01,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  // const unsigned char lut_bw1[]
  0x22, 42,
  0x80  ,0x19 ,0x01 ,0x00 ,0x00 ,0x01,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  // const unsigned char lut_wb1[]
  0x23, 42,
  0x40  ,0x19 ,0x01 ,0x00 ,0x00 ,0x01,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
  // const unsigned char lut_bb1[]
  0x24, 42,
  0x00  ,0x19 ,0x01 ,0x00 ,0x00 ,0x01,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,  
  0x00  ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,

  0xFE // EOM
};

// clang-format on

/**************************************************************************/
/*!
    @brief  Class for interfacing with UC8151D EPD drivers
*/
/**************************************************************************/
class Adafruit_UC8151D : public Adafruit_EPD {
 public:
  Adafruit_UC8151D(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                   int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                   int16_t BUSY = -1);
  Adafruit_UC8151D(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                   int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void powerDown();
  void update();

  void displayPartial(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

 protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};

#endif
