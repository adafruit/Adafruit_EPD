#include "Adafruit_JD79667.h"

#include "Adafruit_EPD.h"

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define BUSY_WAIT 500

// clang-format off

const uint8_t jd79667_default_init_code[] {
    0xFF, 10, // wait a lil bit
    0x4D, 1, 0x78,
    JD79667_PANEL_SETTING, 2, 0x0F, 0x29, // PSR, Display resolution is 180x384
    JD79667_POWER_SETTING, 2, 0x07, 0x00, // PWR
    0x03, 3, 0x10, 0x54, 0x44, // POFS
    JD79667_BOOSTER_SOFTSTART, 7, 0x05, 0x00, 0x3F, 0x0A, 0x25, 0x12, 0x1A,
    JD79667_CDI, 1, 0x37, // CDI
    0x60, 2, 0x02, 0x02, // TCON
    JD79667_RESOLUTION, 4, 0, 180, 1, 128, // TRES 180x384
    0xE7, 1, 0x1C,
    0xE3, 1, 0x22,
    0xB4, 1, 0xD0,
    0xB5, 1, 0x03,
    0xE9, 1, 0x01,
    JD79667_PLL_CONTROL, 1, 0x08,
    JD79667_POWER_ON, 0,
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
Adafruit_JD79667::Adafruit_JD79667(int width, int height, int16_t SID,
                                   int16_t SCLK, int16_t DC, int16_t RST,
                                   int16_t CS, int16_t SRCS, int16_t MISO,
                                   int16_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  if ((width % 8) != 0) {
    width += 8 - (width % 8);
  }
  buffer1_size = width * height / 4;
  buffer2_size = 0;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
  } else {
    buffer1 = (uint8_t*)malloc(buffer1_size);
    buffer2 = buffer1;
  }
  singleByteTxns = true;
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
Adafruit_JD79667::Adafruit_JD79667(int width, int height, int16_t DC,
                                   int16_t RST, int16_t CS, int16_t SRCS,
                                   int16_t BUSY, SPIClass* spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
  if ((width % 8) != 0) {
    width += 8 - (width % 8);
  }
  buffer1_size = width * height / 4;
  buffer2_size = 0;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
  } else {
    buffer1 = (uint8_t*)malloc(buffer1_size);
    buffer2 = buffer1;
  }

  singleByteTxns = true;
}

/**************************************************************************/
/*!
    @brief clear all data buffers
*/
/**************************************************************************/
void Adafruit_JD79667::clearBuffer() {
  if (use_sram) {
    sram.erase(colorbuffer_addr, buffer1_size, 0x55);
  } else {
    memset(buffer1, 0x55, buffer1_size);
  }
}

/**************************************************************************/
/*!
    @brief draw a single pixel on the screen
        @param x the x axis position
        @param y the y axis position
        @param color the color of the pixel
*/
/**************************************************************************/
void Adafruit_JD79667::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  uint8_t* pBuf;

  // deal with non-4-bit heights
  uint16_t _WIDTH = WIDTH;
  if (_WIDTH % 4 != 0) {
    _WIDTH += 4 - (_WIDTH % 4);
  }

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
    case 1:
      EPD_swap(x, y);
      x = _WIDTH - x - 1;
      // remove the offset
      x -= _WIDTH - WIDTH;
      break;
    case 2:
      x = _WIDTH - x - 1;
      y = HEIGHT - y - 1;
      // re-add the offset
      x += _WIDTH - WIDTH;
      break;
    case 3:
      EPD_swap(x, y);
      y = HEIGHT - y - 1;
      break;
  }
  uint32_t addr = ((uint32_t)x + (uint32_t)y * _WIDTH) / 4;
  uint8_t color_c;

  if (use_sram) {
    color_c = sram.read8(colorbuffer_addr + addr);
    pBuf = &color_c;
  } else {
    pBuf = color_buffer + addr;
  }

  if (color == EPD_BLACK) {
    color = JD79667_BLACK;
  } else if (color == EPD_RED) {
    color = JD79667_RED;
  } else if (color == EPD_YELLOW) {
    color = JD79667_YELLOW;
  } else if (color == EPD_WHITE) {
    color = JD79667_WHITE;
  }

  uint8_t byte_offset_mask = 0x3 << (3 - (x % 4)) * 2;
  uint8_t byte_offset_value = (color & 0x3) << (3 - (x % 4)) * 2;

  *pBuf &= ~byte_offset_mask; // save reverse mask
  *pBuf |= byte_offset_value; // now add in the new color

  if (use_sram) {
    sram.write8(colorbuffer_addr + addr, *pBuf);
  }
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_JD79667::busy_wait(void) {
  if (_busy_pin >= 0) {
    while (!digitalRead(_busy_pin)) { // wait for busy HIGH!
      delay(10);
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
void Adafruit_JD79667::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  delay(100);
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_JD79667::update() {
  uint8_t buf[1];

  // display update sequence
  buf[0] = 0x00;
  EPD_command(JD79667_DISPLAY_REFRESH, buf, 1);

  busy_wait();

  if (_busy_pin <= -1) {
    delay(1000);
  }
}

void Adafruit_JD79667::hardwareReset(void) {
  if (_reset_pin >= 0) {
    // Setup reset pin direction
    pinMode(_reset_pin, OUTPUT);
    // VDD (3.3V) goes high at start, lets just chill for a ms
    digitalWrite(_reset_pin, HIGH);
    delay(20);
    // bring reset low
    digitalWrite(_reset_pin, LOW);
    // wait 40ms
    delay(40);
    // bring out of reset
    digitalWrite(_reset_pin, HIGH);
    delay(50);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/

void Adafruit_JD79667::powerUp() {
  hardwareReset();
  busy_wait();

  const uint8_t* init_code = jd79667_default_init_code;
  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  busy_wait();
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_JD79667::powerDown() {
  uint8_t buf[1];
  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    // deep sleep
    buf[0] = 0x00;
    EPD_command(JD79667_POWER_OFF, buf, 1);
    busy_wait();
    delay(100);
    buf[0] = 0xA5;
    EPD_command(JD79667_DEEP_SLEEP, buf, 1);
  }
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
uint8_t Adafruit_JD79667::writeRAMCommand(uint8_t index) {
  (void)index;
  EPD_command(JD79667_DATA_START_XMIT);
  return true;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_JD79667::setRAMAddress(uint16_t x, uint16_t y) {
  (void)x;
  (void)y;
}
