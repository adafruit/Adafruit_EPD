#include "Adafruit_EPD.h"
#include "Adafruit_IL91874.h"

const unsigned char lut_vcomDC[] =
{
0x00	,0x00,
0x00	,0x1A	,0x1A	,0x00	,0x00	,0x01,		
0x00	,0x0A	,0x0A	,0x00	,0x00	,0x08,		
0x00	,0x0E	,0x01	,0x0E	,0x01	,0x10,		
0x00	,0x0A	,0x0A	,0x00	,0x00	,0x08,		
0x00	,0x04	,0x10	,0x00	,0x00	,0x05,		
0x00	,0x03	,0x0E	,0x00	,0x00	,0x0A,		
0x00	,0x23	,0x00	,0x00	,0x00	,0x01	
};

//R21H
const unsigned char lut_ww[] ={
0x90	,0x1A	,0x1A	,0x00	,0x00	,0x01,
0x40	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x84	,0x0E	,0x01	,0x0E	,0x01	,0x10,
0x80	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x00	,0x04	,0x10	,0x00	,0x00	,0x05,
0x00	,0x03	,0x0E	,0x00	,0x00	,0x0A,
0x00	,0x23	,0x00	,0x00	,0x00	,0x01
					};
//R22H	r
const unsigned char lut_bw[] ={
0xA0	,0x1A	,0x1A	,0x00	,0x00	,0x01,
0x00	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x84	,0x0E	,0x01	,0x0E	,0x01	,0x10,
0x90	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0xB0	,0x04	,0x10	,0x00	,0x00	,0x05,
0xB0	,0x03	,0x0E	,0x00	,0x00	,0x0A,
0xC0	,0x23	,0x00	,0x00	,0x00	,0x01
					};
//R23H	w
const unsigned char lut_bb[] ={
0x90	,0x1A	,0x1A	,0x00	,0x00	,0x01,
0x40	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x84	,0x0E	,0x01	,0x0E	,0x01	,0x10,
0x80	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x00	,0x04	,0x10	,0x00	,0x00	,0x05,
0x00	,0x03	,0x0E	,0x00	,0x00	,0x0A,
0x00	,0x23	,0x00	,0x00	,0x00	,0x01
					};
//R24H	b
const unsigned char lut_wb[] ={
0x90	,0x1A	,0x1A	,0x00	,0x00	,0x01,
0x20	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x84	,0x0E	,0x01	,0x0E	,0x01	,0x10,
0x10	,0x0A	,0x0A	,0x00	,0x00	,0x08,
0x00	,0x04	,0x10	,0x00	,0x00	,0x05,
0x00	,0x03	,0x0E	,0x00	,0x00	,0x0A,
0x00	,0x23	,0x00	,0x00	,0x00	,0x01
					};

#ifdef USE_EXTERNAL_SRAM
Adafruit_IL91874::Adafruit_IL91874(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else
Adafruit_IL91874::Adafruit_IL91874(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY) {
	bw_buf = (uint8_t *)malloc(width * height / 8);
	red_buf = (uint8_t *)malloc(width * height / 8);
#endif
	bw_bufsize = width * height / 8;
	red_bufsize = bw_bufsize;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_IL91874::Adafruit_IL91874(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EPD(width, height, DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_IL91874::Adafruit_IL91874(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(width, height, DC, RST, CS, BUSY) {
	bw_buf = (uint8_t *)malloc(width * height / 8);
	red_buf = (uint8_t *)malloc(width * height / 8);
#endif
	bw_bufsize = width * height / 8;
	red_bufsize = bw_bufsize;
}

void Adafruit_IL91874::begin(bool reset)
{
	uint8_t buf[5];
	singleByteTxns = true;
	Adafruit_EPD::begin(reset);
}

void Adafruit_IL91874::update()
{
	EPD_command(IL91874_DISPLAY_REFRESH);
			
	while(!digitalRead(busy)) delay(1); //wait for busy low
	
	//power off
	uint8_t buf[4];

	EPD_command(IL91874_POWER_OFF);
	while(!digitalRead(busy)) delay(1); //wait for busy low

	buf[0] = 0xA5;
	EPD_command(IL91874_DEEP_SLEEP, buf, 1);
}

void Adafruit_IL91874::powerUp()
{
	uint8_t buf[5];

	digitalWrite(rst, LOW);
    // wait 10ms
    delay(10);
    // bring out of reset
    digitalWrite(rst, HIGH);
    delay(10);

	buf[0] = 0x03;
	buf[1] = 0x00;
	buf[2] = 0x2b;
	buf[3] = 0x2b;
	buf[4] = 0x09;
	EPD_command(IL91874_POWER_SETTING, buf, 5);
		
	buf[0] = 0x07;
	buf[1] = 0x07;
	buf[2] = 0x017;
	EPD_command(IL91874_BOOSTER_SOFT_START, buf, 3);

	buf[0] = 0x60;
	buf[1] = 0xA5;
	EPD_command(0xF8, buf, 2);

	buf[0] = 0x89;
	buf[1] = 0xA5;
	EPD_command(0xF8, buf, 2);

	buf[0] = 0x90;
	buf[1] = 0x00;
	EPD_command(0xF8, buf, 2);

	buf[0] = 0x93;
	buf[1] = 0x2A;
	EPD_command(0xF8, buf, 2);

	buf[0] = 0x73;
	buf[1] = 0x41;
	EPD_command(0xF8, buf, 2);

	buf[0] = 0x00;
	EPD_command(IL91874_PDRF, buf, 1);

	EPD_command(IL91874_POWER_ON);
	while(!digitalRead(busy)) delay(1); //wait for busy low
	
	buf[0] = 0xAF;
	EPD_command(IL91874_PANEL_SETTING, buf, 1);
	
	buf[0] = 0x3a;
	EPD_command(IL91874_PLL, buf, 1);

	buf[0] = (height() >> 8) & 0xFF;
	buf[1] = height() & 0xFF;
	buf[2] = (width() >> 8) & 0xFF;
	buf[3] = width() & 0xFF;
	EPD_command(IL91874_RESOLUTION, buf, 4);
			
	buf[0] = 0x12;
	EPD_command(IL91874_VCM_DC_SETTING, buf, 1);

	buf[0] = 0x87;
	EPD_command(IL91874_CDI, buf, 1);

	EPD_command(IL91874_LUT1, lut_vcomDC, 44);
	EPD_command(IL91874_LUTWW, lut_ww, 42);
	EPD_command(IL91874_LUTBW, lut_bw, 42);
	EPD_command(IL91874_LUTWB, lut_wb, 42);
	EPD_command(IL91874_LUTBB, lut_bb, 42);
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
	delay(2);
	
	dcHigh();
	
	for(uint16_t i=0; i<bw_bufsize; i++){
		c = fastSPIwrite(c);
	}
	sram.csHigh();
	
	delay(2);
	
	sram.csLow();
	//send write command
	fastSPIwrite(K640_READ);
	
	uint8_t b[2];
	b[0] = (bw_bufsize >> 8);
	b[1] = (bw_bufsize & 0xFF);
	//send address
	fastSPIwrite(b[0]);
	fastSPIwrite(b[1]);
	
	//first data byte from SRAM will be transfered in at the same time as the EPD command is transferred out
	c = EPD_command(EPD_RAM_RED, false);
	delay(2);
	
	dcHigh();
	
	for(uint16_t i=0; i<red_bufsize; i++){
		c = fastSPIwrite(c);
	}
	sram.csHigh();
	delay(2);
	
#else
	//write image
	EPD_command(IL91874_DTM1);
	dcHigh();

	for(uint16_t i=0; i<bw_bufsize; i++){
		csLow();
		fastSPIwrite(bw_buf[i]);
		csHigh();
	}
	
	EPD_command(0x13);
	dcHigh();
		
	for(uint16_t i=0; i<red_bufsize; i++){
		csLow();
		fastSPIwrite(red_buf[i]);
		csHigh();
	}

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

	uint16_t addr = ( (width() - x) * height() + y)/8;

#ifdef USE_EXTERNAL_SRAM
	if(color == EPD_RED){
		//red is written after bw
		addr = addr + bw_bufsize;
	}
	uint8_t c = sram.read8(addr);
	pBuf = &c;
#else
	if(color == EPD_RED){
		pBuf = red_buf + addr;
	}
	else{
		pBuf = bw_buf + addr;
	}
#endif
	// x is which column
	switch (color)
	{
		case EPD_RED:
		case EPD_WHITE:   *pBuf |= (1 << (7 - y%8)); break;
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
	sram.erase(0x00, bw_bufsize, 0xFF);
	sram.erase(bw_bufsize, red_bufsize, 0x00);
#else
	memset(bw_buf, 0xFF, bw_bufsize);
	memset(red_buf, 0x00, red_bufsize);
#endif
}

void Adafruit_IL91874::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}