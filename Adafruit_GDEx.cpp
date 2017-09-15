#include "Adafruit_EINK.h"
#include "Adafruit_GDEx.h"

#if defined(GDEX_72_172)

const uint8_t init_data[]={
	0x00,0x00,0x00,0x55,0x00,0x00,0x55,0x55,
	0x00,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
	0x55,0xAA,0x55,0x55,0xAA,0xAA,0xAA,0xAA,
	0x05,0x05,0x05,0x05,0x15,0x15,0x15,0x15,
	0x01,0x01,0x01,0x01,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x34,0x32,0xF1,0x74,0x14,0x00,0x00,0x00,
	0x00,0x00,};
	
#else

const uint8_t init_data[]={};

#endif

#ifdef USE_EXTERNAL_SRAM
Adafruit_GDEx::Adafruit_GDEx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS, int8_t MISO) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY, SRCS, MISO){
#else

extern uint16_t EINK_BUFFER[EINK_BUFSIZE];

Adafruit_GDEx::Adafruit_GDEx(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(SID, SCLK, DC, RST, CS, BUSY) {
#endif
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM
Adafruit_GDEx::Adafruit_GDEx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY, int8_t SRCS) : Adafruit_EINK(DC, RST, CS, BUSY, SRCS) {
#else
Adafruit_GDEx::Adafruit_GDEx(int8_t DC, int8_t RST, int8_t CS, int8_t BUSY) : Adafruit_EINK(DC, RST, CS, BUSY) {
	#endif
}

void Adafruit_GDEx::begin(bool reset)
{
	uint8_t buf[5];
	Adafruit_EINK::begin(reset);
			
	// Init sequence
	if(reset){
		EINK_command(GDEX_SWRESET);
		while(digitalRead(busy)); //wait for busy low
	}
		
	buf[0] = 0x00;
	EINK_command(GDEX_DEEP_SLEEP_MODE, buf, 1);
		
	//ram data entry mode
	buf[0] = 0x03;
	EINK_command(GDEX_DATA_ENTRY_MODE, buf, 1);

	buf[0] = 0xB0; //20V
	EINK_command(GDEX_GATE_DRIVING_VOLTAGE_CONTROL, buf, 1);
			
	buf[0] = 0x2; //11V
	EINK_command(GDEX_SOURCE_DRIVING_VOLTAGE_CONTROL, buf, 1);
		
#if defined(GDEX_72_172)
	//ram x address
	buf[0] = 0x00;
	buf[1] = 0x11;
	EINK_command(GDEX_RAM_X_START_END, buf, 2);
				
	//ram y address
	buf[0] = 0x00;
	buf[1] = 0xAB;
	EINK_command(GDEX_RAM_Y_START_END, buf, 2);
#endif
		
	buf[0] = 0x03;
	EINK_command(GDEX_DISPLAY_UPDATE_CONTROL_1, buf, 1);
		
	buf[0] = 0x1F;
	EINK_command(GDEX_BOOSTER_FEEDBACK, buf, 1);
			
	buf[0] = 0x3C; //-1.5V
	EINK_command(GDEX_WRITE_VCOM_REGISTER, buf, 1);
			
	//set border
	buf[0] = 0x63;
	EINK_command(GDEX_BORDER_WAVEFORM, buf, 1);
			
	buf[0] = 0xC4;
	EINK_command(GDEX_DISPLAY_UPDATE_CONTROL_2, buf, 1);
			
	//write LUT
	EINK_command(GDEX_WRITE_LUT, init_data, 90);
}

void Adafruit_GDEx::update()
{
	uint8_t c = 0xc4;
	EINK_command(GDEX_DISPLAY_UPDATE_CONTROL_2, &c, 1);
		
	c = 0x03;
	EINK_command(GDEX_MASTER_ACTIVATION, &c, 1);
			
	while(digitalRead(busy)); //wait for busy low
}

void Adafruit_GDEx::display()
{
	uint8_t cmdbuf[2];

	cmdbuf[0] = 0x00;
	EINK_command(GDEX_RAM_X_ADDRESS_COUNTER, cmdbuf, 1);

	cmdbuf[0] = 0x00;
	cmdbuf[1] = 0x00;
	EINK_command(GDEX_RAM_Y_ADDRESS_COUNTER, cmdbuf, 2);
			
	Adafruit_EINK::display();
	
	EINK_command(0xFF);
			
	update();
}

void Adafruit_GDEx::sleep()
{
	//enter deep sleep. MUST HW RESET TO EXIT DEEP SLEEP
	uint8_t c = 0x01;
	EINK_command(GDEX_DEEP_SLEEP_MODE, &c, 1);
}
	
void Adafruit_GDEx::drawPixel(int16_t x, int16_t y, uint16_t color) {
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

	uint16_t addr = ( (EINK_LCDWIDTH - x) * EINK_LCDHEIGHT + y)/4;

#ifdef USE_EXTERNAL_SRAM
	addr = addr * 2; //2 bytes in sram
	uint16_t c = sram.read16(addr);
	pBuf = &c;
#else
	pBuf = EINK_BUFFER + addr;
#endif
  // x is which column
  uint8_t bits = (6 - y%4 * 2);
  *pBuf &= ~(0x3 << bits);
    switch (color)
    {
      case EINK_BLACK:   break;
	  case EINK_DARK:	*pBuf |= (0x1 << bits); break;
	  case EINK_LIGHT:	*pBuf |= (0x2 << bits); break;
      case EINK_WHITE:   *pBuf |= (0x3 << bits); break;
    }
#ifdef USE_EXTERNAL_SRAM
	sram.write16(addr, *pBuf);
#endif
	
}

void Adafruit_GDEx::clearBuffer()
{
#ifdef USE_EXTERNAL_SRAM
  sram.erase(0xFF, EINK_BUFSIZE * 2);
#else
  memset(EINK_BUFFER, 0xFF, EINK_BUFSIZE * 2);
#endif
}

void Adafruit_GDEx::clearDisplay() {
	clearBuffer();
	display();
	delay(100);
	display();
}