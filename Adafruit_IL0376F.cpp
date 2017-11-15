#include "Adafruit_EPD.h"
#include "Adafruit_IL0376F.h"

const uint8_t lut_vcom0[] ={	0x0E	,0x14	,0x01	,0x0A	,0x06	,0x04	,0x0A	,0x0A	,0x0F	,0x03	,0x03	,0x0C	,0x06	,0x0A	,0x00	};
const uint8_t lut_w[] ={	0x0E	,0x14	,0x01	,0x0A	,0x46	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x86	,0x0A	,0x04	};
const uint8_t lut_b[] ={	0x0E	,0x14	,0x01	,0x8A	,0x06	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x06	,0x4A	,0x04	};
const uint8_t lut_g1[] ={	0x8E	,0x94	,0x01	,0x8A	,0x06	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x06	,0x0A	,0x04	};
const uint8_t lut_g2[] ={	0x8E	,0x94	,0x01	,0x8A	,0x06	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x06	,0x0A	,0x04	};
const uint8_t lut_vcom1[] ={	0x03	,0x1D	,0x01	,0x01	,0x08	,0x23	,0x37	,0x37	,0x01	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	};
const uint8_t lut_red0[] ={	0x83	,0x5D	,0x01	,0x81	,0x48	,0x23	,0x77	,0x77	,0x01	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	};
const uint8_t lut_red1[] ={	0x03	,0x1D	,0x01	,0x01	,0x08	,0x23	,0x37	,0x37	,0x01	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	};

#ifdef USE_EXTERNAL_SRAM
Adafruit_IL0376F::Adafruit_IL0376F(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else
Adafruit_IL0376F::Adafruit_IL0376F(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY) {
	bw_buf = (uint8_t *)malloc(width * height / 4);
	red_buf = (uint8_t *)malloc(width * height / 8);
#endif
	bw_bufsize = width * height / 4;
	red_bufsize = width * height / 8;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_IL0376F::Adafruit_IL0376F(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EPD(width, height, DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_IL0376F::Adafruit_IL0376F(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EPD(width, height, DC, RST, CS, BUSY) {
	bw_buf = (uint8_t *)malloc(width * height / 4);
	red_buf = (uint8_t *)malloc(width * height / 8);
#endif
	bw_bufsize = width * height / 4;
	red_bufsize = width * height / 8;
}

void Adafruit_IL0376F::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EPD::begin(reset);
  	while(digitalRead(busy)); //wait for busy low
		
	buf[0] = 0x07;
	buf[1] = 0x00;
	buf[2] = 0x0D;
	buf[3] = 0x00;
	EPD_command(IL0376F_POWER_SETTING, buf, 4);
		
	buf[0] = 0x07;
	buf[1] = 0x07;
	buf[2] = 0x07;
	EPD_command(IL0376F_BOOSTER_SOFT_START, buf, 3);
}

void Adafruit_IL0376F::update()
{
	EPD_command(IL0376F_DISPLAY_REFRESH);
			
	while(digitalRead(busy)); //wait for busy low
	
	delay(10000);
	
	//power off
	uint8_t buf[4];
	
	buf[0] = 0x17;
	EPD_command(IL0376F_CDI, buf, 1);
	
	buf[0] = 0x00;
	EPD_command(IL0376F_VCM_DC_SETTING, buf, 0);
	
	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	EPD_command(IL0376F_POWER_SETTING);
	
	EPD_command(IL0376F_POWER_OFF);
}

void Adafruit_IL0376F::powerUp()
{
	uint8_t buf[5];
	EPD_command(IL0376F_POWER_ON);
	while(digitalRead(busy)); //wait for busy low
	delay(200);
	
	buf[0] = 0xCF;
	EPD_command(IL0376F_PANEL_SETTING, buf, 1);
	
	buf[0] = 0x37;
	EPD_command(IL0376F_CDI, buf, 1);
	
	buf[0] = 0x39;
	EPD_command(IL0376F_PLL, buf, 1);

	buf[0] = height() & 0xFF;
	buf[1] = (height() >> 8) & 0xFF;
	buf[2] = width() & 0xFF;
	buf[3] = (width() >> 8) & 0xFF;
	EPD_command(IL0376F_RESOLUTION, buf, 4);
	
	buf[0] = 0x0E;
	EPD_command(IL0376F_VCM_DC_SETTING, buf, 1);
	
	//write LUTs
	EPD_command(IL0376F_VCOM1_LUT, lut_vcom0, 15);
	EPD_command(IL0376F_WHITE_LUT, lut_w, 15);
	EPD_command(IL0376F_BLACK_LUT, lut_b, 15);
	EPD_command(IL0376F_GRAY1_LUT, lut_g1, 15);
	EPD_command(IL0376F_GRAY2_LUT, lut_g2, 15);
	EPD_command(IL0376F_VCOM2_LUT, lut_vcom1, 15);
	EPD_command(IL0376F_RED0_LUT, lut_red0, 15);
	EPD_command(IL0376F_RED1_LUT, lut_red1, 15);
}

void Adafruit_IL0376F::display()
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
	
	for(uint16_t i=0; i<bw_bufsize; i++){
		c = fastSPIwrite(c);
	}
	csHigh();
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
	
	dcHigh();
	
	for(uint16_t i=0; i<red_bufsize; i++){
		c = fastSPIwrite(c);
	}
	csHigh();
	sram.csHigh();
	
#else
	//write image
	EPD_command(EPD_RAM_BW, false);
	dcHigh();

	for(uint16_t i=0; i<bw_bufsize; i++){
		fastSPIwrite(bw_buf[i]);
	}
	csHigh();
	
	EPD_command(EPD_RAM_RED, false);
	dcHigh();
		
	for(uint16_t i=0; i<red_bufsize; i++){
		fastSPIwrite(red_buf[i]);
	}
	csHigh();

#endif
	update();
}
		
void Adafruit_IL0376F::drawPixel(int16_t x, int16_t y, uint16_t color) {
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
	uint16_t addr;

	if(color == EPD_RED){
		addr = ( (width() - x) * height() + y)/8;
	}
	else{
		addr = ( (width() - x) * height() + y)/4;
	}

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
	
	if(color == EPD_RED){
		*pBuf &= ~(1 << (7 - (y%8)));
	}
	else{
		uint8_t bits = (6 - y%4 * 2);
		*pBuf &= ~(0x3 << bits);
		switch (color)
		{
			case EPD_BLACK:   break;
			case EPD_DARK:	*pBuf |= (0x1 << bits); break;
			case EPD_LIGHT:	*pBuf |= (0x2 << bits); break;
			case EPD_WHITE:   *pBuf |= (0x3 << bits); break;
		}
	}
	
#ifdef USE_EXTERNAL_SRAM
	sram.write8(addr, *pBuf);
#endif
			
}

void Adafruit_IL0376F::clearBuffer()
{
	#ifdef USE_EXTERNAL_SRAM
	sram.erase(0x00, bw_bufsize + red_bufsize, 0xFF);
	#else
	memset(bw_buf, 0xFF, bw_bufsize);
	memset(red_buf, 0xFF, red_bufsize);
	#endif
}

void Adafruit_IL0376F::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}