/*!
 * @file Adafruit_EPD.h
 *
 * This is a library for our EPD displays based on EPD drivers.
 * Designed specifically to work with Adafruit EPD displays.
 *
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef _Adafruit_EPD_H_
#define _Adafruit_EPD_H_

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#define USE_EXTERNAL_SRAM ///< use the external RAM chip on the EPD breakout
#define RAMBUFSIZE 64 ///< size of the ram buffer

#if defined(__SAM3X8E__)
 typedef volatile RwReg PortReg; ///< a port register for fast access
 typedef uint32_t PortMask; ///< a port register mask for your pin
 #define HAVE_PORTREG
#elif defined(ARDUINO_ARCH_SAMD)
// not supported
#elif defined(ESP8266) || defined(ESP32) || defined(ARDUINO_STM32_FEATHER) || defined(__arc__)
  typedef volatile uint32_t PortReg;  ///< a port register for fast access
  typedef uint32_t PortMask; ///< a port register mask for your pin
#elif defined(__AVR__)
  typedef volatile uint8_t PortReg;  ///< a port register for fast access
  typedef uint8_t PortMask; ///< a port register mask for your pin
  #define HAVE_PORTREG
#else
  // chances are its 32 bit so assume that
  typedef volatile uint32_t PortReg;  ///< a port register for fast access
  typedef uint32_t PortMask; ///< a port register mask for your pin
#endif

#include "Adafruit_MCPSRAM.h"

#include <SPI.h>
#include <Adafruit_GFX.h>

 /**************************************************************************/
/*!
    @brief available EPD colors
*/
/**************************************************************************/
enum{
	EPD_BLACK, ///< black color
	EPD_WHITE, ///< white color
	EPD_INVERSE, ///< invert color
	EPD_RED, ///< red color
	EPD_GRAY, ///< gray color ('red' on grayscale)
	EPD_DARK, ///< darker color
	EPD_LIGHT, ///< lighter color
};

#define EPD_swap(a, b) { int16_t t = a; a = b; b = t; } ///< simple swap function

/**************************************************************************/
/*!
    @brief  Class for interfacing with Adafruit EPD display breakouts.
*/
/**************************************************************************/
class Adafruit_EPD : public Adafruit_GFX {
 public:

#ifdef USE_EXTERNAL_SRAM
  Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY = -1);
  Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY = -1);
#else
  Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY = -1);
  Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY = -1);
#endif
  ~Adafruit_EPD();

  void begin(bool reset=true);

 protected:
  int8_t sid, ///< sid pin
   sclk, ///< serial clock pin
   dc, ///< data/command pin
   rst, ///< reset pin
   cs, ///< chip select pin
   busy; ///< busy pin

  bool blackInverted, ///< is black channel inverted
   redInverted; ///< is red channel inverted
  int bw_bufsize, ///< size of the black and white buffer
   red_bufsize; ///< size of the red buffer
  bool singleByteTxns; ///< if true CS will go high after every data byte transferred

#ifndef USE_EXTERNAL_SRAM
  uint8_t *bw_buf; ///< the pointer to the black and white buffer if using on-chip ram
  uint8_t *red_buf; ///< the pointer to the red buffer if using on-chip ram
#else
  Adafruit_MCPSRAM sram; ///< the ram chip object if using off-chip ram
#endif
  
  void EPD_command(uint8_t c, const uint8_t *buf, uint16_t len);
  uint8_t EPD_command(uint8_t c, bool end=true);
  void EPD_data(const uint8_t *buf, uint16_t len);
  void EPD_data(uint8_t data);
  void invertColorLogic(uint8_t colorLayer, bool invert);

  uint8_t fastSPIwrite(uint8_t c);

  boolean hwSPI; ///< true if using hardware SPI
#ifdef HAVE_PORTREG
  PortReg *mosiport, ///< mosi port register
   *clkport, ///< serial clock port register
    *csport, ///< chip select port register
     *dcport; ///< data/command port register
  PortMask mosipinmask, ///< mosi pin mask
   clkpinmask, ///< serial clock pin mask
   cspinmask, ///< chip select pin mask
   dcpinmask; ///< data / command pin mask
#endif
 void csLow();
 void csHigh();
 void dcHigh();
 void dcLow();
};

#include "Adafruit_IL0376F.h"
#include "Adafruit_IL91874.h"
#include "Adafruit_IL0398.h"
#include "Adafruit_IL0371.h"
#include "Adafruit_IL0373.h"
#include "Adafruit_SSD1675.h"

#endif /* _Adafruit_EPD_H_ */
