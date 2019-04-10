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

//#define EPD_DEBUG

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
enum {
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

  Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY = -1);
  Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY = -1);
  ~Adafruit_EPD();

  void begin(bool reset=true);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void clearBuffer();
  void clearDisplay();
  void setBlackBuffer(int8_t index, bool inverted);
  void setColorBuffer(int8_t index, bool inverted);
  void display(void);

 protected:
  /**************************************************************************/
  /*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color displays)
    Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the command
  */
  /**************************************************************************/
  virtual uint8_t writeRAMCommand(uint8_t index) = 0;

  /**************************************************************************/
  /*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
  */
  /**************************************************************************/
  virtual void setRAMAddress(uint16_t x, uint16_t y) = 0;

  /**************************************************************************/
  /*!
    @brief start up the display
  */
  /**************************************************************************/
  virtual void powerUp(void) = 0;

  /**************************************************************************/
  /*!
    @brief signal the display to update
  */
  /**************************************************************************/
  virtual void update(void) = 0;

  /**************************************************************************/
  /*!
    @brief wind down the display
  */
  /**************************************************************************/
  virtual void powerDown(void) = 0;
  void hardwareReset(void);

  int8_t _sid_pin, ///< sid pin
    _sclk_pin, ///< serial clock pin
    _dc_pin, ///< data/command pin
    _reset_pin, ///< reset pin
    _cs_pin, ///< chip select pin
    _busy_pin; ///< busy pin
  static bool _isInTransaction; ///< true if SPI bus is in trasnfer state
  bool singleByteTxns; ///< if true CS will go high after every data byte transferred
  Adafruit_MCPSRAM sram; ///< the ram chip object if using off-chip ram

  bool blackInverted; ///< is black channel inverted
  bool colorInverted; ///< is red channel inverted
  uint16_t buffer1_size; ///< size of the primary buffer
  uint16_t buffer2_size; ///< size of the secondary buffer
  uint8_t *buffer1; ///< the pointer to the primary buffer if using on-chip ram
  uint8_t *buffer2; ///< the pointer to the secondary buffer if using on-chip ram
  uint8_t *color_buffer;  ///< the pointer to the color buffer if using on-chip ram
  uint8_t *black_buffer;  ///< the pointer to the black buffer if using on-chip ram
  uint16_t buffer1_addr;  ///< The SRAM address offsets for the primary buffer
  uint16_t buffer2_addr;  ///< The SRAM address offsets for the secondary buffer
  uint16_t colorbuffer_addr;  ///< The SRAM address offsets for the color buffer
  uint16_t blackbuffer_addr;  ///< The SRAM address offsets for the black buffer

  void EPD_command(uint8_t c, const uint8_t *buf, uint16_t len);
  uint8_t EPD_command(uint8_t c, bool end=true);
  void EPD_data(const uint8_t *buf, uint16_t len);
  void EPD_data(uint8_t data);

  uint8_t SPItransfer(uint8_t c);

  bool use_sram; ///< true if we are using an SRAM chip as a framebuffer
  bool hwSPI; ///< true if using hardware SPI
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

/*
#include "Adafruit_IL0376F.h"
#include "Adafruit_IL0371.h"
*/
#include "Adafruit_IL0398.h"
#include "Adafruit_IL91874.h"
#include "Adafruit_IL0373.h"
#include "Adafruit_SSD1608.h"
#include "Adafruit_SSD1675.h"
#endif /* _Adafruit_EPD_H_ */
