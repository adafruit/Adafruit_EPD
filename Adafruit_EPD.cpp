/*!
 * @file Adafruit_EPD.cpp
 *
 * @mainpage Adafruit EPD driver
 *
 * @section intro_sec Introduction
 *
 * This is the documentation for Adafruit's EPD driver for the
 * Arduino platform.  It is designed specifically to work with the
 * Adafruit EPD breakouts.
 *
 * These displays use SPI to communicate, 6 pins are requiEPD_RED to
 * interface
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section dependencies Dependencies
 *
 * This library depends on <a href="https://github.com/adafruit/Adafruit-GFX-Library">
 * Adafruit_GFX</a> being present on your system. Please make sure you have
 * installed the latest version before using this library.
 *
 * @section author Author
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * @section license License
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr)) ///< read bytes from program memory
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
 #include <util/delay.h>
#endif

#include <stdlib.h>

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_EPD.h"

#ifdef USE_EXTERNAL_SRAM

/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param MISO the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY) : Adafruit_GFX(width, height),
sram(SID, MISO, SCLK, SRCS) {
#else
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_GFX(width, height) {
#endif
  cs = CS;
  rst = RST;
  dc = DC;
  sclk = SCLK;
  sid = SID;
  busy = BUSY;
  hwSPI = false;
  singleByteTxns = false;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY) : Adafruit_GFX(width, height),
sram(SRCS) {
#else
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_GFX(width, height) {
#endif
  dc = DC;
  rst = RST;
  cs = CS;
  busy = BUSY;
  hwSPI = true;
  singleByteTxns = false;
}

/**************************************************************************/
/*!
    @brief default destructor
*/
/**************************************************************************/
Adafruit_EPD::~Adafruit_EPD()
{
#ifndef USE_EXTERNAL_SRAM
  free(bw_buf);
  free(red_buf);
#endif
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_EPD::begin(bool reset) {
  blackInverted = true;
  redInverted = false;
  
#ifdef USE_EXTERNAL_SRAM
	sram.begin();
	sram.write8(0, K640_SEQUENTIAL_MODE, K640_WRSR);
#endif
  
  // set pin directions
    pinMode(dc, OUTPUT);
    pinMode(cs, OUTPUT);
#ifdef HAVE_PORTREG
    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);
    dcport      = portOutputRegister(digitalPinToPort(dc));
    dcpinmask   = digitalPinToBitMask(dc);
#endif

	csHigh();

    if (!hwSPI){
      // set pins for software-SPI
      pinMode(sid, OUTPUT);
      pinMode(sclk, OUTPUT);
#ifdef HAVE_PORTREG
      clkport     = portOutputRegister(digitalPinToPort(sclk));
      clkpinmask  = digitalPinToBitMask(sclk);
      mosiport    = portOutputRegister(digitalPinToPort(sid));
      mosipinmask = digitalPinToBitMask(sid);
#endif
      }
    if (hwSPI){
      SPI.begin();
#ifndef SPI_HAS_TRANSACTION
      SPI.setClockDivider (4);
#endif
    }

    pinMode(rst, OUTPUT);
  if ((reset) && (rst >= 0)) {
    // Setup reset pin direction
    digitalWrite(rst, HIGH);
    // VDD (3.3V) goes high at start, lets just chill for a ms
    delay(1);
    // bring reset low
    digitalWrite(rst, LOW);
    // wait 10ms
    delay(10);
    // bring out of reset
    digitalWrite(rst, HIGH);
  }
  if(busy > -1)
    pinMode(busy, INPUT);
}

/**************************************************************************/
/*!
    @brief send an EPD command followed by data
    @param c the command to send
    @param buf the buffer of data to send
    @param len the length of the data buffer
*/
/**************************************************************************/
void Adafruit_EPD::EPD_command(uint8_t c, const uint8_t *buf, uint16_t len)
{
	EPD_command(c, false);
	EPD_data(buf, len);
}

/**************************************************************************/
/*!
    @brief send an EPD command with no data
    @param c the command to send
    @param end if true the cs pin will be pulled high following the transaction. If false the cs pin will remain low.
    @returns the data byte read over the SPI bus
*/
/**************************************************************************/
uint8_t Adafruit_EPD::EPD_command(uint8_t c, bool end) {
    // SPI
	csHigh();
	dcLow();
	csLow();
	
	uint8_t data = fastSPIwrite(c);

	if(end){
		csHigh();
	}

	return data;
}


/**************************************************************************/
/*!
    @brief send data to the display
    @param buf the data buffer to send
    @param len the length of the data buffer
*/
/**************************************************************************/
void Adafruit_EPD::EPD_data(const uint8_t *buf, uint16_t len)
{
	// SPI
	dcHigh();

	for (uint16_t i=0; i<len; i++) {
		fastSPIwrite(buf[i]);
	}
	csHigh();
}

/**************************************************************************/
/*!
    @brief transfer a single byte over SPI.
    @param d the data to send
    @returns the data byte read
*/
/**************************************************************************/
inline uint8_t Adafruit_EPD::fastSPIwrite(uint8_t d) {
  if(hwSPI) {
    if(singleByteTxns){
      uint8_t b;
      csLow();
      b = SPI.transfer(d);
      csHigh();
      return b;
    }
    else return SPI.transfer(d);
  } else {
	  //TODO: return read data for software SPI
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
      *clkport &= ~clkpinmask;
      if(d & bit) *mosiport |=  mosipinmask;
      else        *mosiport &= ~mosipinmask;
      *clkport |=  clkpinmask;
#else
      digitalWrite(sclk, LOW);
      if(d & bit) digitalWrite(sid, HIGH);
      else        digitalWrite(sid, LOW);
      digitalWrite(sclk, HIGH);
#endif
    }
  }
}

/**************************************************************************/
/*!
    @brief set chip select pin high
*/
/**************************************************************************/
void Adafruit_EPD::csHigh()
{
#ifdef SPI_HAS_TRANSACTION
      SPI.endTransaction();
#endif
#ifdef HAVE_PORTREG
	*csport |= cspinmask;
#else
	digitalWrite(cs, HIGH);
#endif
}

/**************************************************************************/
/*!
    @brief set chip select pin low
*/
/**************************************************************************/
void Adafruit_EPD::csLow()
{
#ifdef SPI_HAS_TRANSACTION
      SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
#endif
#ifdef HAVE_PORTREG
	*csport &= ~cspinmask;
#else
	digitalWrite(cs, LOW);
#endif
}

/**************************************************************************/
/*!
    @brief set data/command pin high
*/
/**************************************************************************/
void Adafruit_EPD::dcHigh()
{
#ifdef HAVE_PORTREG
	*dcport |= dcpinmask;
#else
	digitalWrite(dc, HIGH);
#endif
}

/**************************************************************************/
/*!
    @brief set data/command pin low
*/
/**************************************************************************/
void Adafruit_EPD::dcLow()
{
#ifdef HAVE_PORTREG
	*dcport &= ~dcpinmask;
#else
	digitalWrite(dc, LOW);
#endif
}
