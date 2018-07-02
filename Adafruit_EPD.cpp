/*********************************************************************
This is a library for our EPD displays based on EPD drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/TODO

These displays use SPI to communicate, 6 pins are requiEPD_RED to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Dean Miller  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any EPD_REDistribution
*********************************************************************/

#ifdef __AVR__
  #include <avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
 #include <pgmspace.h>
#else
 #define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif

#if !defined(__ARM_ARCH) && !defined(ENERGIA) && !defined(ESP8266) && !defined(ESP32) && !defined(__arc__)
 #include <util/delay.h>
#endif

#include <stdlib.h>

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_EPD.h"

#ifdef USE_EXTERNAL_SRAM
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY) : Adafruit_GFX(width, height),
sram(SID, MISO, SCLK, SRCS) {
#else
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
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY) : Adafruit_GFX(width, height),
sram(SRCS) {
#else
Adafruit_EPD::Adafruit_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_GFX(width, height) {
#endif
  dc = DC;
  rst = RST;
  cs = CS;
  busy = BUSY;
  hwSPI = true;
  singleByteTxns = false;
}

Adafruit_EPD::~Adafruit_EPD()
{
#ifndef USE_EXTERNAL_SRAM
  free(bw_buf);
  free(red_buf);
#endif
}

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

void Adafruit_EPD::EPD_command(uint8_t c, const uint8_t *buf, uint16_t len)
{
	EPD_command(c, false);
	EPD_data(buf, len);
}

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

void Adafruit_EPD::EPD_data(const uint8_t *buf, uint16_t len)
{
	// SPI
	dcHigh();

	for (uint16_t i=0; i<len; i++) {
		fastSPIwrite(buf[i]);
	}
	csHigh();
}

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

void Adafruit_EPD::display()
{

}

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

void Adafruit_EPD::dcHigh()
{
#ifdef HAVE_PORTREG
	*dcport |= dcpinmask;
#else
	digitalWrite(dc, HIGH);
#endif
}

void Adafruit_EPD::dcLow()
{
#ifdef HAVE_PORTREG
	*dcport &= ~dcpinmask;
#else
	digitalWrite(dc, LOW);
#endif
}
