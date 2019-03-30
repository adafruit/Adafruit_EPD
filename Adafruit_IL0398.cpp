#include "Adafruit_EPD.h"
#include "Adafruit_IL0398.h"

#define BUSY_WAIT 500

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
Adafruit_IL0398::Adafruit_IL0398(int width, int height, 
				 int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, 
				 int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY) : 
  Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {

  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
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
Adafruit_IL0398::Adafruit_IL0398(int width, int height, 
				 int8_t DC, int8_t RST, 
				 int8_t CS, int8_t SRCS, int8_t BUSY) : 
  Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY) {

  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_IL0398::busy_wait(void)
{
  if(busy > -1)
    while (digitalRead(busy)); //wait for busy low
  else
    delay(BUSY_WAIT);
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_IL0398::begin(bool reset)
{
  uint8_t buf[5];
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, true);  // red defaults to inverted  

  setRotation(1);

  buf[0] = 0x07;
  buf[1] = 0x07;
  buf[2] = 0x07;
  EPD_command(IL0398_BOOSTER_SOFT_START, buf, 3);
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_IL0398::update()
{
  EPD_command(IL0398_DISPLAY_REFRESH);
			
  busy_wait();
  
  delay(10000);
  
  //power off
  uint8_t buf[4];
  
  buf[0] = 0xF7;
  EPD_command(IL0398_CDI, buf, 1);
  
  EPD_command(IL0398_POWER_OFF);
  
  delay(10000);
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_IL0398::powerUp()
{
  uint8_t buf[4];
  
  EPD_command(IL0398_POWER_ON);
  busy_wait();
  delay(200);
  
  buf[0] = 0x0F;
  EPD_command(IL0398_PANEL_SETTING, buf, 1);
  
  buf[0] = (HEIGHT >> 8) & 0xFF;
  buf[1] = HEIGHT & 0xFF;
  buf[2] = (WIDTH >> 8) & 0xFF;
  buf[3] = WIDTH & 0xFF;
  EPD_command(IL0398_RESOLUTION, buf, 4);
  
  delay(20);
}



uint8_t Adafruit_IL0398::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(EPD_RAM_BW, false);
  }
  if (index == 1) {
    return EPD_command(EPD_RAM_RED, false);
  }
}

void Adafruit_IL0398::setRAMAddress(uint16_t x, uint16_t y) {
  // on this chip we do nothing
}

