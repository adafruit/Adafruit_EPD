#include "Adafruit_IL0398.h"

#include "Adafruit_EPD.h"

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define BUSY_WAIT 500

// clang-format off

const uint8_t il0398_default_init_code[] {
    0xFF, 20,          // busy wait
    IL0398_BOOSTER_SOFT_START, 3, 0x17, 0x17, 0x17,
    IL0398_POWER_ON, 0,
    0xFF, 20,          // busy wait
    IL0398_PANEL_SETTING, 2, 0x1F, 0x0D, // lut from OTP & vcom = 0v
    IL0398_VCOM, 1, 0x97,
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
Adafruit_IL0398::Adafruit_IL0398(int width, int height, int16_t SID,
                                 int16_t SCLK, int16_t DC, int16_t RST,
                                 int16_t CS, int16_t SRCS, int16_t MISO,
                                 int16_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t*)malloc(buffer1_size);
    buffer2 = (uint8_t*)malloc(buffer2_size);
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
Adafruit_IL0398::Adafruit_IL0398(int width, int height, int16_t DC, int16_t RST,
                                 int16_t CS, int16_t SRCS, int16_t BUSY,
                                 SPIClass* spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t*)malloc(buffer1_size);
    buffer2 = (uint8_t*)malloc(buffer2_size);
  }
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_IL0398::busy_wait(void) {
  if (_busy_pin > -1) {
    do {
      EPD_command(IL0398_GETSTATUS);
      delay(10);
    } while (!digitalRead(_busy_pin)); // wait for busy HIGH
    delay(200);
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
void Adafruit_IL0398::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true); // black defaults to inverted
  setColorBuffer(1, true); // red defaults to inverted

  setRotation(1);
  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_IL0398::update() {
  EPD_command(IL0398_DISPLAY_REFRESH);
  delay(100);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(15000);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_IL0398::powerUp() {
  uint8_t buf[4];

  hardwareReset();

  const uint8_t* init_code = il0398_default_init_code;
  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  if (_epd_lut_code) {
    EPD_commandList(_epd_lut_code);
  }

  buf[0] = (HEIGHT >> 8) & 0xFF;
  buf[1] = HEIGHT & 0xFF;
  buf[2] = (WIDTH >> 8) & 0xFF;
  buf[3] = WIDTH & 0xFF;
  EPD_command(IL0398_RESOLUTION, buf, 4);

  delay(20);
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_IL0398::powerDown() {
  uint8_t buf[4];

  // power off
  buf[0] = 0xF7; // border floating
  EPD_command(IL0398_VCOM, buf, 1);
  EPD_command(IL0398_POWER_OFF);
  busy_wait();
  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    buf[0] = 0xA5; // deep sleep
    EPD_command(UC8276_DEEPSLEEP, buf, 1);
  }
  delay(100);
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
uint8_t Adafruit_IL0398::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(EPD_RAM_BW, false);
  }
  if (index == 1) {
    return EPD_command(EPD_RAM_RED, false);
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
void Adafruit_IL0398::setRAMAddress(uint16_t x, uint16_t y) {
  // on this chip we do nothing
  (void)x;
  (void)y;
}
