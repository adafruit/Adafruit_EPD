/*********************************************************************
This is a library for our eInk displays based on SSD1675 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/TODO

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Dean Miller for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/
#ifndef _Adafruit_SSD1675_H_
#define _Adafruit_SSD1675_H_

#if ARDUINO >= 100
 #include "Arduino.h"
 #define WIRE_WRITE Wire.write
#else
 #include "WProgram.h"
  #define WIRE_WRITE Wire.send
#endif

#if defined(__SAM3X8E__)
 typedef volatile RwReg PortReg;
 typedef uint32_t PortMask;
 #define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#elif defined(__AVR__)
  typedef volatile uint8_t PortReg;
  typedef uint8_t PortMask;
  #define HAVE_PORTREG
#else
  // chances are its 32 bit so assume that
  typedef volatile uint32_t PortReg;
  typedef uint32_t PortMask;
#endif

#include "Adafruit_23k640.h"

#include <SPI.h>
#include <Adafruit_GFX.h>

#define SSD1675_BLACK 0
#define SSD1675_WHITE 1
#define SSD1675_INVERSE 2
#define SSD1675_RED 3

#define USE_EXTERNAL_SRAM

/*=========================================================================

    -----------------------------------------------------------------------*/
   #define SSD1675_104_212
/*=========================================================================*/

#if defined SSD1675_104_212
  #define SSD1675_LCDWIDTH                  212
  #define SSD1675_LCDHEIGHT                 104
  #define SSD1675_BUFSIZE					2756
#endif

#define SSD1675_DRIVER_OUTPUT_CONTROL 0x01
#define SSD1675_GATE_VOLTAGE_CONTROL 0x03
#define SSD1675_SOURCE_VOLTAGE_CONTROL 0x04
#define SSD1675_DEEP_SLEEP_MODE 0x10
#define SSD1675_DATA_ENTRY_MODE_SETTING 0x11
#define SSD1675_SWRST 0x12
#define SSD1675_TEMP_SENSOR_CONTROL 0x1A
#define SSD1675_MASTER_ACTIVATION 0x20
#define SSD1675_DISPLAY_UPDATE_CTRL_1 0x21
#define SSD1675_DISPLAY_UPDATE_CTRL_2 0x22
#define SSD1675_WRITE_RAM_1 0x24
#define SSD1675_WRITE_RAM_2 0x26
#define SSD1675_WRITE_VCOM 0x2C
#define SSD1675_OTP_REGISTER_READ 0x2D
#define SSD1675_STATUS_BIT_READ 0x2F
#define SSD1675_WRITE_LUT_REGISTER 0x32
#define SSD1675_SET_DUMMY_LINE_PERIOD 0x3A
#define SSD1675_SET_GATE_LINE_WIDTH 0x3B
#define SSD1675_BORDER_WAVEFORM_CONTROL 0x3C
#define SSD1675_SET_RAM_X_START_END 0x44
#define SSD1675_SET_RAM_Y_START_END 0x45
#define SSD1675_SET_RAM_X_ADDRESS_COUNTER 0x4E
#define SSD1675_SET_RAM_Y_ADDRESS_COUNTER 0x4F
#define SSD1675_SET_ANALOG_BLOCK_CONTROL 0x74
#define SSD1675_SET_DIGITAL_BLOCK_CONTROL 0x7E

class Adafruit_SSD1675 : public Adafruit_GFX {
 public:

#ifdef USE_EXTERNAL_SRAM
  Adafruit_SSD1675(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO);
  Adafruit_SSD1675(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS);
#else
  Adafruit_SSD1675(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
  Adafruit_SSD1675(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
#endif

  void begin(bool reset=true);
  void SSD1675_command(uint8_t c, const uint8_t *buf, uint16_t len);
  void SSD1675_command(uint8_t c, bool end=true);
  void SSD1675_data(const uint8_t *buf, uint16_t len);

  void clearDisplay();
  void display();
  void invertDisplay(bool black = true, bool red = true);
  void update();
  void sleep();

  void dim(boolean dim);

  void drawPixel(int16_t x, int16_t y, uint16_t color);

  virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);

 private:
  int8_t _i2caddr, _vccstate, sid, sclk, dc, rst, cs, busy;
  bool blackInverted, redInverted;
  
#ifdef USE_EXTERNAL_SRAM
  Adafruit_23k640 sram;
#endif
  
  void fastSPIwrite(uint8_t c);

  boolean hwSPI;
#ifdef HAVE_PORTREG
  PortReg *mosiport, *clkport, *csport, *dcport;
  PortMask mosipinmask, clkpinmask, cspinmask, dcpinmask;
#endif
  inline void csLow();
  inline void csHigh();
  inline void dcHigh();
  inline void dcLow();

  inline void drawFastVLineInternal(int16_t x, int16_t y, int16_t h, uint16_t color) __attribute__((always_inline));
  inline void drawFastHLineInternal(int16_t x, int16_t y, int16_t w, uint16_t color) __attribute__((always_inline));
};

#endif /* _Adafruit_SSD1675_H_ */
