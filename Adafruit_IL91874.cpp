#include "Adafruit_EINK.h"
#include "Adafruit_IL91874.h"

#ifdef USE_EXTERNAL_SRAM
#define RAMBUFSIZE 64
Adafruit_IL91874::Adafruit_IL91874(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else

extern uint8_t EINK_BUFFER[EINK_BUFSIZE];
extern uint8_t EINK_REDBUFFER[EINK_REDBUFSIZE];

Adafruit_IL91874::Adafruit_IL91874(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY) {
#endif
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_IL91874::Adafruit_IL91874(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EINK(DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_IL91874::Adafruit_IL91874(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(DC, RST, CS, BUSY) {
	#endif
}

void Adafruit_IL91874::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EINK::begin(reset);
			
	buf[0] = 0x07;
	buf[1] = 0x00;
	buf[2] = 0x0A;
	buf[3] = 0x00;
	EINK_command(IL91874_POWER_SETTING, buf, 4);
		
	buf[0] = 0x07;
	buf[1] = 0x07;
	buf[2] = 0x07;
	EINK_command(IL91874_BOOSTER_SOFT_START, buf, 3);
	
	EINK_command(IL91874_POWER_ON);
	while(digitalRead(busy)); //wait for busy low
	delay(200);
	
	buf[0] = 0xCF;
	EINK_command(IL91874_PANEL_SETTING, buf, 1);
	
	buf[0] = 0x37;
	EINK_command(IL91874_CDI, buf, 1);
	
	buf[0] = 0x29;
	EINK_command(IL91874_PLL, buf, 1);
			
	buf[0] = 0x68;
	buf[1] = 0x00;
	buf[2] = 0xD4;
	EINK_command(IL91874_RESOLUTION, buf, 3);
			
	buf[0] = 0x0A;
	EINK_command(IL91874_VCM_DC_SETTING, buf, 1);
	delay(20);
}

void Adafruit_IL91874::update()
{
	EINK_command(IL91874_DISPLAY_REFRESH);
			
	while(digitalRead(busy)); //wait for busy low
	
	delay(10000);
	
	//power off
	uint8_t buf[4];
	
	buf[0] = 0x17;
	EINK_command(IL91874_CDI, buf, 1);
	
	buf[0] = 0x00;
	EINK_command(IL91874_VCM_DC_SETTING, buf, 0);
	
	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	EINK_command(IL91874_POWER_SETTING);
	
	EINK_command(IL91874_POWER_OFF);
}

void Adafruit_IL91874::display()
{
	Adafruit_EINK::display();
	update();
}
		
void Adafruit_IL91874::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
	return;
	
	uint8_t *pBuf;

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
	//make our buffer happy
	x = (x == 0 ? 1 : x);

	uint16_t addr = ( (EINK_LCDWIDTH - x) * EINK_LCDHEIGHT + y)/8;

#ifdef USE_EXTERNAL_SRAM
	if(color == EINK_RED){
			//red is written after bw
			addr = addr + EINK_BUFSIZE;
		}
		uint8_t c = sram.read8(addr);
		pBuf = &c;
#else
	if(color == EINK_RED){
		pBuf = EINK_REDBUFFER + addr;
	}
	else{
		pBuf = EINK_BUFFER + addr;
	}
#endif
	// x is which column
	switch (color)
	{
		case EINK_WHITE:   *pBuf |= (1 << (7 - y%8)); break;
		case EINK_RED:
		case EINK_BLACK:   *pBuf &= ~(1 << (7 - y%8)); break;
		case EINK_INVERSE: *pBuf ^= (1 << (7 - y%8)); break;
	}
#ifdef USE_EXTERNAL_SRAM
	sram.write8(addr, *pBuf);
#endif
	
}

void Adafruit_IL91874::clearBuffer()
{
#ifdef USE_EXTERNAL_SRAM
	sram.erase(0x00, EINK_BUFSIZE + EINK_REDBUFSIZE, 0xFF);
#else
	memset(EINK_BUFFER, 0xFF, EINK_BUFSIZE);
	memset(EINK_REDBUFFER, 0xFF, EINK_REDBUFSIZE);
#endif
}

void Adafruit_IL91874::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}