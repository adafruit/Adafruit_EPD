#include "Adafruit_EINK.h"
#include "Adafruit_IL0376F.h"

#if defined(IL0376F_200_200)

const uint8_t lut_vcom0[] ={	0x0E	,0x14	,0x01	,0x0A	,0x06	,0x04	,0x0A	,0x0A	,0x0F	,0x03	,0x03	,0x0C	,0x06	,0x0A	,0x00	};
const uint8_t lut_w[] ={	0x0E	,0x14	,0x01	,0x0A	,0x46	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x86	,0x0A	,0x04	};
const uint8_t lut_b[] ={	0x0E	,0x14	,0x01	,0x8A	,0x06	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x06	,0x4A	,0x04	};
const uint8_t lut_g1[] ={	0x8E	,0x94	,0x01	,0x8A	,0x06	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x06	,0x0A	,0x04	};
const uint8_t lut_g2[] ={	0x8E	,0x94	,0x01	,0x8A	,0x06	,0x04	,0x8A	,0x4A	,0x0F	,0x83	,0x43	,0x0C	,0x06	,0x0A	,0x04	};
const uint8_t lut_vcom1[] ={	0x03	,0x1D	,0x01	,0x01	,0x08	,0x23	,0x37	,0x37	,0x01	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	};
const uint8_t lut_red0[] ={	0x83	,0x5D	,0x01	,0x81	,0x48	,0x23	,0x77	,0x77	,0x01	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	};
const uint8_t lut_red1[] ={	0x03	,0x1D	,0x01	,0x01	,0x08	,0x23	,0x37	,0x37	,0x01	,0x00	,0x00	,0x00	,0x00	,0x00	,0x00	};

#else

const uint8_t lut_vcom0[] ={};
const uint8_t lut_w[] ={};
const uint8_t lut_b[] ={};
const uint8_t lut_g1[] ={};
const uint8_t lut_g2[] ={};
const uint8_t lut_vcom1[] ={};
const uint8_t lut_red0[] ={};
const uint8_t lut_red1[] ={};


#endif

#ifdef USE_EXTERNAL_SRAM
Adafruit_IL0376F::Adafruit_IL0376F(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else

extern uint16_t EINK_BUFFER[EINK_BUFSIZE];

Adafruit_IL0376F::Adafruit_IL0376F(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY) {
#endif
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_IL0376F::Adafruit_IL0376F(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EINK(DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_IL0376F::Adafruit_IL0376F(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(DC, RST, CS, BUSY) {
#endif
}

void Adafruit_IL0376F::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EINK::begin(reset);
		
	buf[0] = 0x07;
	buf[1] = 0x00;
	buf[2] = 0x0D;
	buf[3] = 0x00;
	EINK_command(IL0376F_POWER_SETTING, buf, 4);
		
	buf[0] = 0x07;
	buf[1] = 0x07;
	buf[2] = 0x07;
	EINK_command(IL0376F_BOOSTER_SOFT_START, buf, 3);
	
	EINK_command(IL0376F_POWER_ON);
	while(digitalRead(busy)); //wait for busy low
	delay(200);
	
	buf[0] = 0xCF;
	EINK_command(IL0376F_PANEL_SETTING, buf, 1);
	
	buf[0] = 0x37;
	EINK_command(IL0376F_CDI, buf, 1);
	
	buf[0] = 0x39;
	EINK_command(IL0376F_PLL, buf, 1);

	buf[0] = 0xC8;
	buf[1] = 0x00;
	buf[2] = 0xC8;
	EINK_command(IL0376F_RESOLUTION, buf, 3);
	
	buf[0] = 0x0E;
	EINK_command(IL0376F_VCM_DC_SETTING, buf, 1);
	
	//write LUTs
	EINK_command(IL0376F_VCOM1_LUT, lut_vcom0, 15);
	EINK_command(IL0376F_WHITE_LUT, lut_w, 15);
	EINK_command(IL0376F_BLACK_LUT, lut_b, 15);
	EINK_command(IL0376F_GRAY1_LUT, lut_g1, 15);
	EINK_command(IL0376F_GRAY2_LUT, lut_g2, 15);
	EINK_command(IL0376F_VCOM2_LUT, lut_vcom1, 15);
	EINK_command(IL0376F_RED0_LUT, lut_red0, 15);
	EINK_command(IL0376F_RED1_LUT, lut_red1, 15);
}

void Adafruit_IL0376F::update()
{
	EINK_command(IL0376F_DISPLAY_REFRESH);
			
	while(digitalRead(busy)); //wait for busy low
	
	delay(10000);
	
	//power off
	uint8_t buf[4];
	
	buf[0] = 0x17;
	EINK_command(IL0376F_CDI, buf, 1);
	
	buf[0] = 0x00;
	EINK_command(IL0376F_VCM_DC_SETTING, buf, 0);
	
	buf[0] = 0x02;
	buf[1] = 0x00;
	buf[2] = 0x00;
	buf[3] = 0x00;
	EINK_command(IL0376F_POWER_SETTING);
	
	EINK_command(IL0376F_POWER_OFF);
}

void Adafruit_IL0376F::display()
{
	Adafruit_EINK::display();
	update();
}
		
void Adafruit_IL0376F::drawPixel(int16_t x, int16_t y, uint16_t color) {
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
			
	//make our buffer happy
	x = (x == 0 ? 1 : x);
	uint16_t addr;

	if(color == EINK_RED){
		addr = ( (EINK_LCDWIDTH - x) * EINK_LCDHEIGHT + y)/8;
	}
	else{
		addr = ( (EINK_LCDWIDTH - x) * EINK_LCDHEIGHT + y)/4;
	}

	#ifdef USE_EXTERNAL_SRAM
		addr = addr * 2; //2 bytes in sram
		uint16_t c = sram.read16(addr);
		pBuf = &c;
	#else
		pBuf = EINK_BUFFER + addr;
	#endif
	
	if(color == EINK_RED){
		*pBuf &= ~(1 << (15 - (y%8)));
	}
	else{
		uint8_t bits = (6 - y%4 * 2);
		*pBuf &= ~(0x3 << bits);
		switch (color)
		{
			case EINK_BLACK:   break;
			case EINK_DARK:	*pBuf |= (0x1 << bits); break;
			case EINK_LIGHT:	*pBuf |= (0x2 << bits); break;
			case EINK_WHITE:   *pBuf |= (0x3 << bits); break;
		}
	}
	
	#ifdef USE_EXTERNAL_SRAM
	sram.write16(addr, *pBuf);
	#endif
			
}

void Adafruit_IL0376F::clearBuffer()
{
	#ifdef USE_EXTERNAL_SRAM
	sram.erase(0xFF, EINK_BUFSIZE * 2);
	#else
	memset(EINK_BUFFER, 0xFF, EINK_BUFSIZE * 2);
	#endif
}

void Adafruit_IL0376F::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}