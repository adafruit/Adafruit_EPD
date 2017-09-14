#ifndef LIB_ADAFRUIT_GDEX
#define LIB_ADAFRUIT_GDEX

#include "Adafruit_EINK.h"
#include <Arduino.h>

#if defined(GDEX_72_172)
	#define EINK_LCDWIDTH                  172
	#define EINK_LCDHEIGHT                 72
	#define EINK_BUFSIZE				   3096
	#define EINK_RAM_BW						0x24
#endif

#define GDEX_DRIVER_OUTPUT_CONTROL 0x01
#define GDEX_GATE_DRIVING_VOLTAGE_CONTROL 0x03
#define GDEX_SOURCE_DRIVING_VOLTAGE_CONTROL 0x04
#define GDEX_DISPLAY_CONTROL 0x07

#define GDEX_GATE_SCAN_START 0x0F
#define GDEX_DEEP_SLEEP_MODE 0x10
#define GDEX_DATA_ENTRY_MODE 0x11
#define GDEX_SWRESET 0x12

#define GDEX_MASTER_ACTIVATION 0x20
#define GDEX_DISPLAY_UPDATE_CONTROL_1 0x21
#define GDEX_DISPLAY_UPDATE_CONTROL_2 0x22
#define GDEX_WRITE_RAM 0x24
#define GDEX_VCOM_SENSE 0x28
#define GDEX_VCOM_SENSE_DURATION 0x29
#define GDEX_WRITE_VCOM_REGISTER 0x2C
#define GDEX_WRITE_LUT 0x32
#define GDEX_DUMMY_LINE_PERIOD 0x3A
#define GDEX_GATE_LINE_WIDTH 0x3B
#define GDEX_BORDER_WAVEFORM 0x3C

#define GDEX_RAM_X_START_END 0x44
#define GDEX_RAM_Y_START_END 0x45

#define GDEX_RAM_X_ADDRESS_COUNTER 0x4E
#define GDEX_RAM_Y_ADDRESS_COUNTER 0x4F
#define GDEX_BOOSTER_FEEDBACK 0xF0

class Adafruit_GDEx : public Adafruit_EINK {
	public:

	#ifdef USE_EXTERNAL_SRAM
	Adafruit_GDEx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO);
	Adafruit_GDEx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS);
	#else
	Adafruit_GDEx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
	Adafruit_GDEx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
	#endif

	void begin(bool reset=true);
	
	void drawPixel(int16_t x, int16_t y, uint16_t color);
	
	void display();
	void invertDisplay(bool black = true, bool red = true);
	void update();
	void sleep();
	
	void clearBuffer();
	void clearDisplay();
};

#endif