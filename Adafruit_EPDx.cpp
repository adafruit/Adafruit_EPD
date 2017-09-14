#include "Adafruit_EINK.h"
#include "Adafruit_EPDx.h"

#if defined(EPDX_104_212)
const uint8_t init_data[]={
	0xA5,	0x89,	0x10,	0x00,	0x00,	0x00,	0x00,	0xA5,	0x19,	0x80,	0x00,	0x00,	0x00,	0x00,	0xA5,	0xA9,
	0x9B,	0x00,	0x00,	0x00,	0x00,	0xA5,	0xA9,	0x9B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x0F,	0x0F,	0x0F,	0x0F,	0x02,	0x10,	0x10,	0x0A,	0x0A,	0x03,	0x08,	0x08,	0x09,
	0x43,	0x07,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};

#elif defined(EPDX_128_296)

const uint8_t init_data[]={
	0x22,	0x11,	0x10,	0x00,	0x10,	0x00,	0x00,	0x11,	0x88,	0x80,	0x80,	0x80,	0x00,	0x00,	0x6A,	0x9B,
	0x9B,	0x9B,	0x9B,	0x00,	0x00,	0x6A,	0x9B,	0x9B,	0x9B,	0x9B,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x04,	0x18,	0x04,	0x16,	0x01,	0x0A,	0x0A,	0x0A,	0x0A,	0x02,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,	0x04,	0x04,	0x08,	0x3C,	0x07,	0x00,	0x00,	0x00,	0x00,
	0x00,	0x00,	0x00,	0x00,	0x00,	0x00,
};

#else

const uint8_t init_data[] = {};

#endif

#ifdef USE_EXTERNAL_SRAM
Adafruit_EPDx::Adafruit_EPDx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else

extern uint16_t buffer[EINK_BUFSIZE];

Adafruit_EPDx::Adafruit_EPDx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY) {
#endif
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_EPDx::Adafruit_EPDx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EINK(DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_EPDx::Adafruit_EPDx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(DC, RST, CS, BUSY) {
#endif
}

void Adafruit_EPDx::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EINK::begin(reset);
	
	  // Init sequence
	  if(reset){
		  EINK_command(EPDX_SWRST);
		  while(digitalRead(busy)); //wait for busy low
	  }
	  
	  buf[0] = 0x54;
	  EINK_command(EPDX_SET_ANALOG_BLOCK_CONTROL, buf, 1);
	  
	  buf[0] = 0x3B;
	  EINK_command(EPDX_SET_DIGITAL_BLOCK_CONTROL, buf, 1);
	  
	  //set display size and driver output control
#if defined(EPDX_104_212)
	  buf[0] = 0xD3;
	  buf[1] = 0x00;
	  buf[2] = 0x00;
#elif defined(EPDX_128_296)
	  buf[0] = 0x27;
	  buf[1] = 0x01;
	  buf[2] = 0x00;
#endif
	  EINK_command(EPDX_DRIVER_OUTPUT_CONTROL, buf, 3);

#if defined(EPDX_104_212)
	  buf[0] = 0x18;
#elif defined(EPDX_128_296)
	  buf[0] = 0x35;
#endif
	  EINK_command(EPDX_SET_DUMMY_LINE_PERIOD, buf, 1);
	  
#if defined(EPDX_104_212)
	  buf[0] = 0x05;
#elif defined(EPDX_128_296)
	  buf[0] = 0x04;
#endif
	  EINK_command(EPDX_SET_GATE_LINE_WIDTH, buf, 1);
	  
	  //ram data entry mode
	  buf[0] = 0x03;
	  EINK_command(EPDX_DATA_ENTRY_MODE_SETTING, buf, 1);

#if defined(EPDX_104_212)
	  //ram x address
	  buf[0] = 0x00;
	  buf[1] = 0x0c;
	  EINK_command(EPDX_SET_RAM_X_START_END, buf, 2);
	  
	  //ram y address
	  buf[0] = 0x00;
	  buf[1] = 0x00;
	  buf[2] = 0xD3;
	  buf[3] = 0x00;
	  buf[4] = 0x00;
	  EINK_command(EPDX_SET_RAM_Y_START_END, buf, 5);

#elif defined(EPDX_128_296)
	  //ram x address
	  buf[0] = 0x00;
	  buf[1] = 0x0F;
	  EINK_command(EPDX_SET_RAM_X_START_END, buf, 2);
	  
	  //ram y address
	  buf[0] = 0x00;
	  buf[1] = 0x00;
	  buf[2] = 0x27;
	  buf[3] = 0x01;
	  buf[4] = 0x00;
	  EINK_command(EPDX_SET_RAM_Y_START_END, buf, 5);
#endif
	  
	  buf[0] = 0x17; //20V
	  EINK_command(EPDX_GATE_VOLTAGE_CONTROL, buf, 1);
	  
	  buf[0] = 0x2D; //11V
	  buf[1] = 0xA8; //5V
	  buf[2] = 0x32; //-15V
	  EINK_command(EPDX_SOURCE_VOLTAGE_CONTROL, buf, 1);
	  
	  buf[0] = 0x3C; //-1.5V
	  EINK_command(EPDX_WRITE_VCOM, buf, 1);
	  
	  //set border
	  buf[0] = 0x33;
	  EINK_command(EPDX_BORDER_WAVEFORM_CONTROL, buf, 1);
	  
	  buf[0] = 0x08;
	  EINK_command(EPDX_DISPLAY_UPDATE_CTRL_1, buf, 1);
	  
	  //write LUT
	  EINK_command(EPDX_WRITE_LUT_REGISTER, init_data, 70);
}

void Adafruit_EPDx::update()
{
	uint8_t c = 0xc7;
	EINK_command(EPDX_DISPLAY_UPDATE_CTRL_2, &c, 1);
	
	EINK_command(EPDX_MASTER_ACTIVATION);
	
	while(digitalRead(busy)); //wait for busy low
}

void Adafruit_EPDx::display()
{

	uint8_t cmdbuf[2];

	cmdbuf[0] = 0x00;
	EINK_command(EPDX_SET_RAM_X_ADDRESS_COUNTER, cmdbuf, 1);

	cmdbuf[0] = 0x00;
	cmdbuf[1] = 0x00;
	EINK_command(EPDX_SET_RAM_Y_ADDRESS_COUNTER, cmdbuf, 2);
	
	Adafruit_EINK::display();
	
	update();
}

void Adafruit_EPDx::sleep()
{
	//enter deep sleep. MUST HW RESET TO EXIT DEEP SLEEP
	uint8_t c = 0x01;
	EINK_command(EPDX_DEEP_SLEEP_MODE, &c, 1);
}

void Adafruit_EPDx::invertDisplay(bool black, bool red)
{
	uint8_t c = (blackInverted << 3) | (redInverted << 7);
	if(EINK_BLACK){
		blackInverted = blackInverted;
		c ^= (blackInverted << 3);
	}
	if(EINK_RED){
		redInverted = !redInverted;
		c ^= (redInverted << 7);
	}
	EINK_command(EPDX_DISPLAY_UPDATE_CTRL_1, &c, 1);
}

void Adafruit_EPDx::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;
	
  uint16_t *pBuf;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    EINK_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
    EINK_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }

	uint16_t addr = (x * EINK_LCDHEIGHT / 8) + y/8;

#ifdef USE_EXTERNAL_SRAM
	addr = addr * 2; //2 bytes in sram
	uint16_t c = sram.read16(addr);
	pBuf = &c;
#else
	pBuf = buffer + addr;
#endif
  // x is which column
    switch (color)
    {
      case EINK_BLACK:   *pBuf |= (1 << (7 - y%8)); break;
      case EINK_WHITE:   *pBuf &= ~(1 << (7 - y%8)); break;
      case EINK_INVERSE: *pBuf ^= (1 << (7 - y%8)); break;
	  case EINK_RED:   *pBuf |= (1 << (15 - (y%8))); break;
    }
#ifdef USE_EXTERNAL_SRAM
	sram.write16(addr, *pBuf);
#endif
	
}

void Adafruit_EPDx::clearBuffer() {
#ifdef USE_EXTERNAL_SRAM
  sram.erase(0x00, EINK_BUFSIZE * 2);
#else
  memset(buffer, 0x00, EINK_BUFSIZE * 2);
#endif
}

void Adafruit_EPDx::clearDisplay() {
	clearBuffer();
	display();
}