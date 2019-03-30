#include "Adafruit_EPD.h"
#include "Adafruit_SSD1608.h"

#define BUSY_WAIT 500

const unsigned char LUT_DATA[30]= {
  0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69, 
  0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00, 
  0xF8, 0xB4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00
};

#ifdef USE_EXTERNAL_SRAM

/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param MISO the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_SSD1608::Adafruit_SSD1608(int width, int height, 
				   int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, 
				   int8_t SRCS, int8_t MISO, int8_t BUSY)
  : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){
#else

/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_SSD1608::Adafruit_SSD1608(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY)   : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, BUSY) {
  
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }
  bw_buf = (uint8_t *)malloc(width * height / 8);
#endif
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }
  bw_bufsize = width * height / 8;
  setRotation(3);

}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
#ifdef USE_EXTERNAL_SRAM

/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_SSD1608::Adafruit_SSD1608(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY)
  : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY) {
#else

/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_SSD1608::Adafruit_SSD1608(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t BUSY)
  : Adafruit_EPD(width, height, DC, RST, CS, BUSY) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }
  bw_buf = (uint8_t *)malloc(width * height / 8);
#endif
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }
  bw_bufsize = width * height / 8;
  setRotation(3);
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1608::busy_wait(void)
{
  if (busy >= 0) {
    while(digitalRead(busy)) { //wait for busy low
      //Serial.print("."); 
      delay(100);
    }
  } else {
    delay(BUSY_WAIT);
  }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_SSD1608::begin(bool reset)
{
  uint8_t buf[5];
  Adafruit_EPD::begin(reset);
  
  delay(100);

  busy_wait();

  // soft reset
  EPD_command(SSD1608_SW_RESET);

  busy_wait();

  // driver output control
  buf[0] = HEIGHT - 1;
  buf[1] = (HEIGHT - 1) >> 8;
  buf[2] = 0x00;
  EPD_command(SSD1608_DRIVER_CONTROL, buf, 3);
    
  // Set dummy line period
  buf[0] = 0x1B;
  EPD_command(SSD1608_WRITE_DUMMY, buf, 1);

  // Set gate line width
  buf[0] = 0x0B;
  EPD_command(SSD1608_WRITE_GATELINE, buf, 1);

  // Data entry sequence
  buf[0] = 0x03;
  EPD_command(SSD1608_DATA_MODE, buf, 1);

  // Set ram X start/end postion
  buf[0] = 0x00;
  buf[1] = WIDTH/8 - 1;
  EPD_command(SSD1608_SET_RAMXPOS, buf, 2);

  // Set ram Y start/end postion
  buf[2] = HEIGHT - 1;
  buf[3] = (HEIGHT - 1) >> 8;
  buf[0] = 0x00;
  buf[1] = 0x00;
  EPD_command(SSD1608_SET_RAMYPOS, buf, 4);

  // Vcom Voltage
  buf[0] = 0x70;
  EPD_command(SSD1608_WRITE_VCOM, buf, 1);

  EPD_command(SSD1608_WRITE_LUT, LUT_DATA, 30);


  /*
  // border color
  buf[0] = 0x03;
  EPD_command(SSD1608_WRITE_BORDER, buf, 1);

  // Set gate voltage
  buf[0] = LUT_DATA[70];
  EPD_command(SSD1608_GATE_VOLTAGE, buf, 1);

  // Set source voltage
  buf[0] = LUT_DATA[71];
  buf[1] = LUT_DATA[72];
  buf[2] = LUT_DATA[73];
  EPD_command(SSD1608_SOURCE_VOLTAGE, buf, 3);
  */
  /*

  // set RAM x address count ;
  buf[0] = 0;
  EPD_command(SSD1608_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count;   
  buf[0] = HEIGHT - 1;
  buf[1] = (HEIGHT - 1) >> 8;
  EPD_command(SSD1608_SET_RAMYCOUNT, buf, 2);
  */
  busy_wait();
}


/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1608::update()
{
  uint8_t buf[1];

  // display update sequence
  buf[0] = 0xC7;
  EPD_command(SSD1608_DISP_CTRL2, buf, 1);

  EPD_command(SSD1608_MASTER_ACTIVATE);

  busy_wait();
    
  // deep sleep
  buf[0] = 0x01;
  EPD_command(SSD1608_DEEP_SLEEP, buf, 1);
  delay(100);
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_SSD1608::powerUp()
{
  begin();
}

/**************************************************************************/
/*!
    @brief show the data stored in the buffer on the display
*/
/**************************************************************************/
void Adafruit_SSD1608::display()
{
  powerUp();
  
  uint8_t buf[2];

#ifdef USE_EXTERNAL_SRAM
  uint8_t c;
  
  // Set RAM X address counter
  buf[0] = 0x0;
  EPD_command(SSD1608_SET_RAMXCOUNT, buf, 1);

  // Set RAM Y address counter
  buf[0] = 0x00;
  buf[1] = 0x00;
  EPD_command(SSD1608_SET_RAMYCOUNT, buf, 2);

  sram.csLow();
  //send read command
  SPItransfer(MCPSRAM_READ);
  
  //send address
  SPItransfer(0x00);
  SPItransfer(0x00);
  
  //first data byte from SRAM will be transfered in at the same time as the EPD command is transferred out
  c = EPD_command(SSD1608_WRITE_RAM, false);
  
  dcHigh();
  
  for(uint16_t i=0; i<bw_bufsize; i++){
    c = SPItransfer(c);
    //Serial.print("0x"); Serial.print((byte)c, HEX); Serial.print(", ");
    //if (i % 32 == 31) Serial.println();
  }
  csHigh();
  sram.csHigh();
  
#else
  //write image
  // set RAM x address count to 0
  buf[0] = 0;
  EPD_command(SSD1608_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count to 0   
  buf[0] = 0x00;
  buf[1] = 0x00;
  EPD_command(SSD1608_SET_RAMYCOUNT, buf, 2);

  EPD_command(SSD1608_WRITE_RAM, false);
  dcHigh();
  for(uint16_t i=0; i<bw_bufsize; i++){
    SPItransfer(bw_buf[i]);
    //Serial.print("0x"); Serial.print(bw_buf[i], HEX); Serial.print(", ");
    //if (i % 32 == 31) Serial.println();
  }
  csHigh(); 
#endif

  update();
}

/**************************************************************************/
/*!
    @brief draw a single pixel on the screen
	@param x the x axis position
	@param y the y axis position
	@param color the color of the pixel
*/
/**************************************************************************/
void Adafruit_SSD1608::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;
	
  uint8_t *pBuf;

  // deal with non-8-bit heights
  uint16_t _HEIGHT = HEIGHT;
  if (_HEIGHT % 8 != 0) {
    _HEIGHT += 8 - (_HEIGHT % 8);
  }

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
    EPD_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = _HEIGHT - y - 1;
    break;
  case 3:
    EPD_swap(x, y);
    y = _HEIGHT - y - 1;
    break;
  }
  //make our buffer happy
  x = (x == 0 ? 1 : x);
  
  uint16_t addr = ( (WIDTH - 1 - x) * _HEIGHT + y)/8;
  Serial.print("X: "); Serial.print(x); Serial.print(" Y: "); Serial.print(y); 
  Serial.print(" Addr: "); Serial.println(addr, HEX);

#ifdef USE_EXTERNAL_SRAM
  uint8_t c = sram.read8(addr);
  pBuf = &c;
#else
  pBuf = bw_buf + addr;
#endif
  if ((color == EPD_BLACK) && blackInverted) {
    *pBuf &= ~(1 << (7 - y%8));
  } else if ((color == EPD_BLACK) && !blackInverted) {
    *pBuf |= (1 << (7 - y%8));
  } else if (color == EPD_INVERSE) {
    *pBuf ^= (1 << (7 - y%8));
  }
#ifdef USE_EXTERNAL_SRAM
  sram.write8(addr, *pBuf);
#endif
}

/**************************************************************************/
/*!
    @brief clear all data buffers
*/
/**************************************************************************/
void Adafruit_SSD1608::clearBuffer()
{
#ifdef USE_EXTERNAL_SRAM
  sram.erase(0x00, bw_bufsize, 0xFF);
#else
  memset(bw_buf, 0xFF, bw_bufsize);
#endif
}

/**************************************************************************/
/*!
    @brief clear the display twice to remove any spooky ghost images
*/
/**************************************************************************/
void Adafruit_SSD1608::clearDisplay() {
  clearBuffer();
  display();
  delay(100);
  display();
}
