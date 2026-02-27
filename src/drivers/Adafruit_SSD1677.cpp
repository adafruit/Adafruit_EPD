#include "Adafruit_SSD1677.h"

#include "Adafruit_EPD.h"

#define EPD_RAM_BW SSD1677_WRITE_RAM_BW
#define EPD_RAM_RED SSD1677_WRITE_RAM_RED

#define BUSY_WAIT 3000

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
    unsigned long start = millis();
    while (digitalRead(_busy_pin)) {
      delay(10);
      if (millis() - start > 15000) {
        return;
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

  _data_entry_mode = THINKINK_UC8179;

  setBlackBuffer(0, true);
  setColorBuffer(1, true);

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update.
    For mono: standard full refresh with bypass RED.
    For grayscale: two-pass process.
      Pass 1 - Preclear: overwrite RED RAM with buffer1 (same as BW),
               then full BW refresh with OTP LUT to drive all pixels
               to a known black/white baseline.
      Pass 2 - Grayscale: reload custom LUT, write buffer1 (LSB) to
               BW and buffer2 (MSB) to RED, then refresh. The LUT
               interprets the transition from the BW baseline to the
               new LSB/MSB combination to produce 4 gray levels.

    Called by base class display() after it has written buffer1 to
    BW RAM (0x24) and buffer2 to RED RAM (0x26).
*/
/**************************************************************************/
void Adafruit_SSD1677::update() {
  uint8_t buf[2];

  if (_grayscale_preclear) {
    setRAMAddress(0, 0);
    if (use_sram) {
      writeSRAMFramebufferToEPD(buffer1_addr, buffer1_size, 1);
    } else {
      writeRAMFramebufferToEPD(buffer1, buffer1_size, 1);
    }

    // Bypass RED, full refresh with OTP LUT
    buf[0] = 0x40;
    buf[1] = 0x00;
    EPD_command(SSD1677_DISPLAY_UPDATE_CTRL1, buf, 2);

    buf[0] = 0xF7;
    EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);
    EPD_command(SSD1677_MASTER_ACTIVATION);

    busy_wait();
    if (_busy_pin <= -1) {
      delay(default_refresh_delay);
    }

    if (_epd_lut_code) {
      EPD_commandList(_epd_lut_code);
    }

    // Write buffer1 (LSB) to BW RAM
    setRAMAddress(0, 0);
    if (use_sram) {
      writeSRAMFramebufferToEPD(buffer1_addr, buffer1_size, 0);
    } else {
      writeRAMFramebufferToEPD(buffer1, buffer1_size, 0);
    }

    // Write buffer2 (MSB) to RED RAM
    setRAMAddress(0, 0);
    if (use_sram) {
      writeSRAMFramebufferToEPD(buffer2_addr, buffer2_size, 1);
    } else {
      writeRAMFramebufferToEPD(buffer2, buffer2_size, 1);
    }

    // NORMAL mode: use both BW and RED for grayscale
    buf[0] = _display_ctrl1_val; // 0x00
    buf[1] = 0x00;
    EPD_command(SSD1677_DISPLAY_UPDATE_CTRL1, buf, 2);

    // Custom LUT refresh
    buf[0] = _display_update_val; // 0xCF
    EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);

    EPD_command(SSD1677_MASTER_ACTIVATION);

    busy_wait();
    if (_busy_pin <= -1) {
      delay(default_refresh_delay);
    }

    return;
  }

  // --- Mono mode: standard full refresh ---
  buf[0] = _display_ctrl1_val; // 0x40 bypass RED
  buf[1] = 0x00;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL1, buf, 2);

  buf[0] = _display_update_val; // 0xF7 full refresh OTP
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);

  EPD_command(SSD1677_MASTER_ACTIVATION);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(default_refresh_delay);
  }
}

/**************************************************************************/
/*!
    @brief Perform a partial (fast differential) display update.
*/
/**************************************************************************/
void Adafruit_SSD1677::displayPartial(uint16_t x1, uint16_t y1,
                                      uint16_t x2, uint16_t y2) {
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;

  softReset();
  setRAMAddress(0, 0);

  // Write NEW frame to BW RAM
  if (use_sram) {
    writeSRAMFramebufferToEPD(buffer1_addr, buffer1_size, 0);
  } else {
    writeRAMFramebufferToEPD(buffer1, buffer1_size, 0);
  }

  uint8_t buf[2];

  // NORMAL mode: compare BW vs RED for differential
  buf[0] = 0x00;
  buf[1] = 0x00;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL1, buf, 2);

  // Partial refresh using built-in LUT:
  // 0xFC = CLOCK_ON | ANALOG_ON | TEMP_LOAD | LUT_LOAD |
  //        MODE_SELECT | DISPLAY_START
  buf[0] = 0xFC;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);

  EPD_command(SSD1677_MASTER_ACTIVATION);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(2000);
  }

  // Sync RED RAM with new frame for next differential update
  setRAMAddress(0, 0);
  if (use_sram) {
    writeSRAMFramebufferToEPD(buffer1_addr, buffer1_size, 1);
  } else {
    writeRAMFramebufferToEPD(buffer1, buffer1_size, 1);
  }

  // Power off analog + clock (not deep sleep)
  buf[0] = 0x03;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);
  EPD_command(SSD1677_MASTER_ACTIVATION);
  busy_wait();
}

/**************************************************************************/
/*!
    @brief Perform a "half" refresh — faster than full, cleaner than partial.
*/
/**************************************************************************/
void Adafruit_SSD1677::displayHalf() {
  softReset();
  setRAMAddress(0, 0);

  // Write to BW RAM
  if (use_sram) {
    writeSRAMFramebufferToEPD(buffer1_addr, buffer1_size, 0);
  } else {
    writeRAMFramebufferToEPD(buffer1, buffer1_size, 0);
  }

  // Also write to RED RAM
  setRAMAddress(0, 0);
  if (use_sram) {
    writeSRAMFramebufferToEPD(buffer1_addr, buffer1_size, 1);
  } else {
    writeRAMFramebufferToEPD(buffer1, buffer1_size, 1);
  }

  uint8_t buf[2];

  // Bypass RED
  buf[0] = 0x40;
  buf[1] = 0x00;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL1, buf, 2);

  // Write high temp for faster refresh
  buf[0] = 0x5A;
  EPD_command(SSD1677_WRITE_TEMP, buf, 1);

  // Half refresh: 0xD4 = CLOCK_ON | ANALOG_ON | LUT_LOAD |
  //               MODE_SELECT | DISPLAY_START
  buf[0] = 0xD4;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);

  EPD_command(SSD1677_MASTER_ACTIVATION);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(5000);
  }

  // Power off (not deep sleep)
  buf[0] = 0x03;
  EPD_command(SSD1677_DISPLAY_UPDATE_CTRL2, buf, 1);
  EPD_command(SSD1677_MASTER_ACTIVATION);
  busy_wait();
}

/**************************************************************************/
/*!
    @brief Perform a soft reset and re-send init commands without
    toggling the hardware reset pin.
*/
/**************************************************************************/
void Adafruit_SSD1677::softReset() {
  const uint8_t *init_code = ssd1677_default_init_code;

  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }

  EPD_commandList(init_code);

  if (_epd_lut_code) {
    EPD_commandList(_epd_lut_code);
  }
}

/**************************************************************************/
/*!
    @brief start up the display (full init with hardware reset)
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

  setRAMAddress(0, 0);
}

/**************************************************************************/
/*!
    @brief wind down the display into deep sleep
*/
/**************************************************************************/
void Adafruit_SSD1677::powerDown() {
  uint8_t buf[1];

  buf[0] = 0x01;
  EPD_command(SSD1677_DEEP_SLEEP, buf, 1);
  delay(100);
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index 0 for BW RAM, 1 for RED RAM
    @returns The byte read from SPI
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

  uint8_t buf[4];

  // RAM X range: 0 to WIDTH-1 (in pixels)
  buf[0] = 0x00;
  buf[1] = 0x00;
  buf[2] = (WIDTH - 1) & 0xFF;
  buf[3] = ((WIDTH - 1) >> 8) & 0xFF;
  EPD_command(SSD1677_SET_RAM_X_RANGE, buf, 4);

  // RAM Y range: 0 to HEIGHT-1
  buf[0] = 0x00;
  buf[1] = 0x00;
  buf[2] = (HEIGHT - 1) & 0xFF;
  buf[3] = ((HEIGHT - 1) >> 8) & 0xFF;
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