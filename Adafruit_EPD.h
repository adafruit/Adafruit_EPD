/*********************************************************************
This is a library for our EPD displays based on EPD drivers

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
#ifndef _Adafruit_EPD_H_
#define _Adafruit_EPD_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define USE_EXTERNAL_SRAM
#define RAMBUFSIZE 64

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

#include "Adafruit_MCPSRAM.h"

#include <SPI.h>
#include <Adafruit_GFX.h>

enum{
	EPD_BLACK,
	EPD_WHITE,
	EPD_INVERSE,
	EPD_RED,
	EPD_DARK,
	EPD_LIGHT,	
};

#define EPD_swap(a, b) { int16_t t = a; a = b; b = t; }

class Adafruit_EPD : public Adafruit_GFX {
 public:

#ifdef USE_EXTERNAL_SRAM
  Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO);
  Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS);
#else
  Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
  Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
#endif
  ~Adafruit_EPD();

  void begin(bool reset=true);

  void clearDisplay();
  void display();
  void invertDisplay(bool black = true, bool red = true);
  void update();
  void sleep();
  
  virtual void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

 protected:
  int8_t _i2caddr, _vccstate, sid, sclk, dc, rst, cs, busy;
  bool blackInverted, redInverted;
  int bw_bufsize, red_bufsize;

  uint8_t *bw_buf;
  uint8_t *red_buf;
  
#ifdef USE_EXTERNAL_SRAM
  Adafruit_MCPSRAM sram;
#endif
  
  void EPD_command(uint8_t c, const uint8_t *buf, uint16_t len);
  uint8_t EPD_command(uint8_t c, bool end=true);
  void EPD_data(const uint8_t *buf, uint16_t len);
  
  uint8_t fastSPIwrite(uint8_t c);

  boolean hwSPI;
#ifdef HAVE_PORTREG
  PortReg *mosiport, *clkport, *csport, *dcport;
  PortMask mosipinmask, clkpinmask, cspinmask, dcpinmask;
#endif
 void csLow();
 void csHigh();
 void dcHigh();
 void dcLow();
};

#include "Adafruit_IL0376F.h"
#include "Adafruit_IL91874.h"

#endif /* _Adafruit_EPD_H_ */
