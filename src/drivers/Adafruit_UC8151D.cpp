#include "Adafruit_UC8151D.h"
#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

const unsigned char LUT_DATA[30] = {
    0x02, 0x02, 0x01, 0x11, 0x12, 0x12, 0x22, 0x22, 0x66, 0x69,
    0x69, 0x59, 0x58, 0x99, 0x99, 0x88, 0x00, 0x00, 0x00, 0x00,
    0xF8, 0xB4, 0x13, 0x51, 0x35, 0x51, 0x51, 0x19, 0x01, 0x00};

// clang-format off

const uint8_t uc8151d_default_init_code[] {
  UC8151D_PON, 0,
    0xFF, 10,
    UC8151D_PSR, 1, 0x1F,
    UC8151D_CDI, 1, 0x97,
    0xFE};
    
// clang-format on

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
Adafruit_UC8151D::Adafruit_UC8151D(int width, int height, int8_t SID,
                                   int8_t SCLK, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t MISO,
                                   int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {

  if ((width % 8) != 0) {
    width += 8 - (width % 8);
  }
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
Adafruit_UC8151D::Adafruit_UC8151D(int width, int height, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t BUSY,
                                   SPIClass *spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {

  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }
  buffer1_size = (uint16_t)width * (uint16_t)height / 8;
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
void Adafruit_UC8151D::busy_wait(void) {
  Serial.print("Waiting...");
  if (_busy_pin >= 0) {
    EPD_command(UC8151D_FLG);

    if (! digitalRead(_busy_pin)) { // wait for busy high
      delay(10);
    }
  } else {
    delay(BUSY_WAIT);
  }
  Serial.println("OK!");
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_UC8151D::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(1, true); // black defaults to inverted
  setColorBuffer(0, true); // red defaults to inverted

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_UC8151D::update() {
  EPD_command(UC8151D_DRF);
  delay(100);
  busy_wait();
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_UC8151D::powerUp() {
  uint8_t buf[5];

  // Demo code resets 3 times!
  hardwareReset();
  delay(10);
  hardwareReset();
  delay(10);
  hardwareReset();
  delay(10);

  const uint8_t *init_code = uc8151d_default_init_code;

  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  if (_epd_lut_code) {
    EPD_commandList(_epd_lut_code);
  }
  busy_wait();
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/

void Adafruit_UC8151D::powerDown(void) {
  uint8_t buf[1];

  buf[0] = 0xF7;
  EPD_command(UC8151D_CDI, buf, 1);

  EPD_command(UC8151D_POF);   //power off
  busy_wait();

  buf[0] = 0xA5;
  EPD_command(UC8151D_DSLP, buf, 1);       
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color
   displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Adafruit_UC8151D::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(UC8151D_DTM1, false);
  }
  if (index == 1) {
    return EPD_command(UC8151D_DTM2, false);
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_UC8151D::setRAMAddress(uint16_t x, uint16_t y) {
}
