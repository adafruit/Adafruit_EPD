#include "Adafruit_SSD1677.h"

#include "Adafruit_EPD.h"

#define EPD_RAM_BW SSD1677_WRITE_RAM_BW
#define EPD_RAM_RED SSD1677_WRITE_RAM_RED

#define BUSY_WAIT 500

// clang-format off

const uint8_t ssd1677_default_init_code[] {
  SSD1677_SOFT_RESET, 0,
    0xFF, 200,                                        // busy wait + delay
    SSD1677_TEMP_SENSOR_CONTROL, 1, 0x80,             // internal temp sensor
    SSD1677_BOOSTER_SOFT_START, 5,
      0xAE, 0xC7, 0xC3, 0xC0, 0x40,                  // GDEQ0426T82 values
    SSD1677_DRIVER_OUTPUT_CONTROL, 3,
      0xDF, 0x01, 0x02,                               // 480 gates, SM=1 TB=0
    SSD1677_BORDER_WAVEFORM, 1, 0x01,
    SSD1677_DATA_ENTRY_MODE, 1, 0x03,                 // X inc, Y inc
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
Adafruit_SSD1677::Adafruit_SSD1677(int width, int height, int16_t SID,
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
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }
}

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
    @param spi the SPI bus to use
*/
/**************************************************************************/
Adafruit_SSD1677::Adafruit_SSD1677(int width, int height, int16_t DC,
                                   int16_t RST, int16_t CS, int16_t SRCS,
                                   int16_t BUSY, SPIClass *spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
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
void Adafruit_SSD1677::busy_wait(void) {
  if (_busy_pin >= 0) {
    // SSD1677 busy is active HIGH — wait while pin is HIGH
    unsigned long start = millis();
    while (digitalRead(_busy_pin)) {
      delay(10);
      if (millis() - start > 15000) {
        return; // timeout
      }
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
void Adafruit_SSD1677::begin(bool reset) {
  Adafruit_EPD::begin(reset);

  // SSD1677 uses horizontal scan layout like UC8179
  _data_entry_mode = THINKINK_UC8179;

  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, true);  // second buffer inverted

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1677::update() {
  uint8_t buf[2];

  // Display Update Control 1: bypass RED RAM for full refresh
  buf[0] = 0x40; // CTRL1_BYPASS_RED
  buf[1] = 0x00; // single chip
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL1, buf, 2);

  // Display Update Control 2: full power cycle
  // 0xF7 = CLOCK_ON | ANALOG_ON | TEMP_LOAD | LUT_LOAD |
  //        DISPLAY_START | ANALOG_OFF | CLOCK_OFF
  buf[0] = 0xF7;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);

  // Master activation — triggers the refresh
  EPD_command(SSD1677_MASTER_ACTIVATION);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(default_refresh_delay);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_SSD1677::powerUp() {
  hardwareReset();

  const uint8_t *init_code = ssd1677_default_init_code;

  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  if (_epd_lut_code) {
    EPD_commandList(_epd_lut_code);
  }

  // Set full-screen RAM window
  setRAMAddress(0, 0);
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_SSD1677::powerDown() {
  uint8_t buf[1];

  buf[0] = 0x01; // enter deep sleep
  EPD_command(SSD1677_DEEP_SLEEP, buf, 1);
  delay(100);
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 for tri-color
   displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Adafruit_SSD1677::writeRAMCommand(uint8_t index) {
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
    @brief Set the RAM address pointer and window for the full display.
    @param x X address counter value (unused, full screen)
    @param y Y address counter value (unused, full screen)
*/
/**************************************************************************/
void Adafruit_SSD1677::setRAMAddress(uint16_t x, uint16_t y) {
  (void)x;
  (void)y;

  // SSD1677 uses pixel-based X addresses with 2-byte values.
  // Gate direction is reversed on GDEQ0426T82, so Y starts high
  // and decrements (data entry mode 0x01 = X inc, Y dec).

  uint8_t buf[4];

  // RAM X range: 0 to WIDTH-1 (in pixels)
  buf[0] = 0x00;                       // X start low
  buf[1] = 0x00;                       // X start high
  buf[2] = (WIDTH - 1) & 0xFF;         // X end low
  buf[3] = ((WIDTH - 1) >> 8) & 0xFF;  // X end high
  EPD_command(SSD1677_SET_RAM_X_RANGE, buf, 4);

  // RAM Y range: 0 to HEIGHT-1 (Y increment mode)
  buf[0] = 0x00;                         // Y start low
  buf[1] = 0x00;                         // Y start high
  buf[2] = (HEIGHT - 1) & 0xFF;          // Y end low
  buf[3] = ((HEIGHT - 1) >> 8) & 0xFF;   // Y end high
  EPD_command(SSD1677_SET_RAM_Y_RANGE, buf, 4);

  // RAM X counter: start at 0
  buf[0] = 0x00;
  buf[1] = 0x00;
  EPD_command(SSD1677_SET_RAM_X_COUNTER, buf, 2);

  // RAM Y counter: start at 0
  buf[0] = 0x00;
  buf[1] = 0x00;
  EPD_command(SSD1677_SET_RAM_Y_COUNTER, buf, 2);
}