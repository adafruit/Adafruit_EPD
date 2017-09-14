#ifndef LIB_ADAFRUIT_EPDX
#define LIB_ADAFRUIT_EPDX

#include "Adafruit_EINK.h"
#include <Arduino.h>

#if defined(EPDX_104_212)
  #define EINK_LCDWIDTH                  212
  #define EINK_LCDHEIGHT                 104
  #define EINK_BUFSIZE					2756
  #define EINK_RAM_BW					 0x24
  #define EINK_RAM_RED					 0x26
  
#elif defined(EPDX_128_296)
  #define EINK_LCDWIDTH                  296
  #define EINK_LCDHEIGHT                 128
  #define EINK_BUFSIZE					4736
  #define EINK_RAM_BW					 0x24
  #define EINK_RAM_RED					 0x26
#endif

#define EPDX_DRIVER_OUTPUT_CONTROL 0x01
#define EPDX_GATE_VOLTAGE_CONTROL 0x03
#define EPDX_SOURCE_VOLTAGE_CONTROL 0x04
#define EPDX_DEEP_SLEEP_MODE 0x10
#define EPDX_DATA_ENTRY_MODE_SETTING 0x11
#define EPDX_SWRST 0x12
#define EPDX_TEMP_SENSOR_CONTROL 0x1A
#define EPDX_MASTER_ACTIVATION 0x20
#define EPDX_DISPLAY_UPDATE_CTRL_1 0x21
#define EPDX_DISPLAY_UPDATE_CTRL_2 0x22
#define EPDX_WRITE_RAM_1 0x24
#define EPDX_WRITE_RAM_2 0x26
#define EPDX_WRITE_VCOM 0x2C
#define EPDX_OTP_REGISTER_READ 0x2D
#define EPDX_STATUS_BIT_READ 0x2F
#define EPDX_WRITE_LUT_REGISTER 0x32
#define EPDX_SET_DUMMY_LINE_PERIOD 0x3A
#define EPDX_SET_GATE_LINE_WIDTH 0x3B
#define EPDX_BORDER_WAVEFORM_CONTROL 0x3C
#define EPDX_SET_RAM_X_START_END 0x44
#define EPDX_SET_RAM_Y_START_END 0x45
#define EPDX_SET_RAM_X_ADDRESS_COUNTER 0x4E
#define EPDX_SET_RAM_Y_ADDRESS_COUNTER 0x4F
#define EPDX_SET_ANALOG_BLOCK_CONTROL 0x74
#define EPDX_SET_DIGITAL_BLOCK_CONTROL 0x7E

class Adafruit_EPDx : public Adafruit_EINK {
public:

#ifdef USE_EXTERNAL_SRAM
	Adafruit_EPDx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO);
	Adafruit_EPDx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS);
#else
	Adafruit_EPDx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
	Adafruit_EPDx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY);
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