/*********************************************************************
This is a library for our eInk displays based on EINK drivers

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
#ifndef _Adafruit_EINK_H_
#define _Adafruit_EINK_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


//#define USE_EXTERNAL_SRAM

/*=========================================================================

    -----------------------------------------------------------------------*/
   #define IL0376F_200_200
/*=========================================================================*/

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

enum{
	EINK_BLACK,
	EINK_WHITE,
	EINK_INVERSE,
	EINK_RED,
	EINK_DARK,
	EINK_LIGHT,	
};

#define EINK_swap(a, b) { int16_t t = a; a = b; b = t; }

class Adafruit_EINK : public Adafruit_GFX {
 public:

#ifdef USE_EXTERNAL_SRAM
  Adafruit_EINK(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO);
  Adafruit_EINK(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS);
#else
  Adafruit_EINK(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
  Adafruit_EINK(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
#endif

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
  
#ifdef USE_EXTERNAL_SRAM
  Adafruit_23k640 sram;
#endif
  
  void EINK_command(uint8_t c, const uint8_t *buf, uint16_t len);
  void EINK_command(uint8_t c, bool end=true);
  void EINK_data(const uint8_t *buf, uint16_t len);
  
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
};

#include "Adafruit_EPDx.h"
#include "Adafruit_GDEx.h"
#include "Adafruit_IL0376F.h"

extern uint16_t EINK_BUFFER[EINK_BUFSIZE];

#endif /* _Adafruit_EINK_H_ */
