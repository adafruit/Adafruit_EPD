#ifndef LIB_ADAFRUIT_IL91874
#define LIB_ADAFRUIT_IL91874

#include "Adafruit_EINK.h"
#include <Arduino.h>

#if defined(IL91874_104_212)
#define EINK_LCDWIDTH                  212
#define EINK_LCDHEIGHT                 104
#define EINK_BUFSIZE				   2756
#define EINK_REDBUFSIZE				   2756
#endif

#define EINK_RAM_BW 0x10
#define EINK_RAM_RED 0x13

#define IL91874_PANEL_SETTING 0x00
#define IL91874_POWER_SETTING 0x01
#define IL91874_POWER_OFF 0x02
#define IL91874_POWER_OFF_SEQUENCE 0x03
#define IL91874_POWER_ON 0x04
#define IL91874_POWER_ON_MEASURE 0x05
#define IL91874_BOOSTER_SOFT_START 0x06
#define IL91874_DEEP_SLEEP 0x07
#define IL91874_DTM1 0x10
#define IL91874_DATA_STOP 0x11
#define IL91874_DISPLAY_REFRESH 0x12
#define IL91874_PDTM1 0x14
#define IL91874_PDTM2 0x15
#define IL91874_PDRF 0x16
#define IL91874_LUT1 0x20
#define IL91874_LUTWW 0x21
#define IL91874_LUTBW 0x22
#define IL91874_LUTWB 0x23
#define IL91874_LUTBB 0x24
#define IL91874_PLL 0x30
#define IL91874_CDI 0x50
#define IL91874_RESOLUTION 0x61
#define IL91874_VCM_DC_SETTING 0x82

class Adafruit_IL91874 : public Adafruit_EINK {
	public:

	#ifdef USE_EXTERNAL_SRAM
	Adafruit_IL91874(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO);
	Adafruit_IL91874(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS);
	#else
	Adafruit_IL91874(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
	Adafruit_IL91874(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
	#endif

	void begin(bool reset=true);
	void powerUp();
	
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	
	void display();
	void invertDisplay(bool black = true, bool red = true);
	void update();
	void sleep();
	
	void clearBuffer();
	void clearDisplay();
};

#endif