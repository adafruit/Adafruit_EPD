#include "Adafruit_EPD.h"
#include "Adafruit_IL91874.h"

#ifdef USE_EXTERNAL_SRAM
#define RAMBUFSIZE 64
Adafruit_IL91874::Adafruit_IL91874(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EPD(SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else

extern uint8_t EPD_BUFFER[EPD_BUFSIZE];
extern uint8_t EPD_REDBUFFER[EPD_REDBUFSIZE];

Adafruit_IL91874::Adafruit_IL91874(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(SID, SCLK, DC, RST, CS, BUSY) {
#endif
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_IL91874::Adafruit_IL91874(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EPD(DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_IL91874::Adafruit_IL91874(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(DC, RST, CS, BUSY) {
	#endif
}

void Adafruit_IL91874::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EPD::begin(reset);
			
	buf[0] = 0x07;
	buf[1] = 0x00;
	buf[2] = 0x0A;
	buf[3] = 0x00;
	EPD_command(IL91874_POWER_SETTING, buf, 4);
		
	buf[0] = 0x07;
	buf[1] = 0x07;
	buf[2] = 0x07;
	EPD_command(IL91874_BOOSTER_SOFT_START, buf, 3);
}

void Adafruit_IL91874::update()
{
	EPD_command(IL91874_DISPLAY_REFRESH);
			
	while(digitalRead(busy)); //wait for busy low
	
	delay(10000);
	
	//power off
	uint8_t buf[4];
	
	buf[0] = 0x17;
	EPD_command(IL91874_CDI, buf, 1);
	
	buf[0] = 0x00;
	EPD_command(IL91874_VCM_DC_SETTING, buf, 0);
	
	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	EPD_command(IL91874_POWER_SETTING);
	
	EPD_command(IL91874_POWER_OFF);
	
	delay(10000);
}

void Adafruit_IL91874::powerUp()
{
	uint8_t buf[3];
	 
	EPD_command(IL91874_POWER_ON);
	while(digitalRead(busy)); //wait for busy low
	delay(200);
	
	buf[0] = 0xCF;
	EPD_command(IL91874_PANEL_SETTING, buf, 1);
	
	buf[0] = 0x37;
	EPD_command(IL91874_CDI, buf, 1);
	
	buf[0] = 0x29;
	EPD_command(IL91874_PLL, buf, 1);
			
	buf[0] = 0x68;
	buf[1] = 0x00;
	buf[2] = 0xD4;
	EPD_command(IL91874_RESOLUTION, buf, 3);
			
	buf[0] = 0x0A;
	EPD_command(IL91874_VCM_DC_SETTING, buf, 1);
	delay(20);
}

void Adafruit_IL91874::display()
{
	powerUp();
	
#ifdef USE_EXTERNAL_SRAM
	uint8_t c;
	
	sram.csLow();
	//send read command
	fastSPIwrite(K640_READ);
	
	//send address
	fastSPIwrite(0x00);
	fastSPIwrite(0x00);
	
	//first data byte from SRAM will be transfered in at the same time as the EPD command is transferred out
	c = EPD_command(EPD_RAM_BW, false);
	
	dcHigh();
	
	for(uint16_t i=0; i<EPD_BUFSIZE; i++){
		c = fastSPIwrite(c);
	}
	csHigh();
	sram.csHigh();
	
	delay(2);
	
	sram.csLow();
	//send write command
	fastSPIwrite(K640_READ);
	
	uint8_t b[2];
	b[0] = (EPD_BUFSIZE >> 8);
	b[1] = (EPD_BUFSIZE & 0xFF);
	//send address
	fastSPIwrite(b[0]);
	fastSPIwrite(b[1]);
	
	//first data byte from SRAM will be transfered in at the same time as the EPD command is transferred out
	c = EPD_command(EPD_RAM_RED, false);
	
	dcHigh();
	
	for(uint16_t i=0; i<EPD_REDBUFSIZE; i++){
		c = fastSPIwrite(c);
	}
	csHigh();
	sram.csHigh();
	
#else
	//write image
	EPD_command(EPD_RAM_BW, false);
	dcHigh();

	for(uint16_t i=0; i<EPD_BUFSIZE; i++){
		fastSPIwrite(EPD_BUFFER[i]);
	}
	csHigh();
	
	EPD_command(EPD_RAM_RED, false);
	dcHigh();
		
	for(uint16_t i=0; i<EPD_REDBUFSIZE; i++){
		fastSPIwrite(EPD_REDBUFFER[i]);
	}
	csHigh();

#endif
	update();
}
		
void Adafruit_IL91874::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
	return;
	
	uint8_t *pBuf;

	// check rotation, move pixel around if necessary
	switch (getRotation()) {
		case 1:
		EPD_swap(x, y);
		x = WIDTH - x - 1;
		break;
		case 2:
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		break;
		case 3:
		EPD_swap(x, y);
		y = HEIGHT - y - 1;
		break;
	}
	//make our buffer happy
	x = (x == 0 ? 1 : x);

	uint16_t addr = ( (EPD_LCDWIDTH - x) * EPD_LCDHEIGHT + y)/8;

#ifdef USE_EXTERNAL_SRAM
	if(color == EPD_RED){
		//red is written after bw
		addr = addr + EPD_BUFSIZE;
	}
	uint8_t c = sram.read8(addr);
	pBuf = &c;
#else
	if(color == EPD_RED){
		pBuf = EPD_REDBUFFER + addr;
	}
	else{
		pBuf = EPD_BUFFER + addr;
	}
#endif
	// x is which column
	switch (color)
	{
		case EPD_WHITE:   *pBuf |= (1 << (7 - y%8)); break;
		case EPD_RED:
		case EPD_BLACK:   *pBuf &= ~(1 << (7 - y%8)); break;
		case EPD_INVERSE: *pBuf ^= (1 << (7 - y%8)); break;
	}
#ifdef USE_EXTERNAL_SRAM
	sram.write8(addr, *pBuf);
#endif
	
}

void Adafruit_IL91874::clearBuffer()
{
#ifdef USE_EXTERNAL_SRAM
	sram.erase(0x00, EPD_BUFSIZE + EPD_REDBUFSIZE, 0xFF);
#else
	memset(EPD_BUFFER, 0xFF, EPD_BUFSIZE);
	memset(EPD_REDBUFFER, 0xFF, EPD_REDBUFSIZE);
#endif
}

void Adafruit_IL91874::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}