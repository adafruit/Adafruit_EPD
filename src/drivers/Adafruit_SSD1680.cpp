#include "Adafruit_SSD1680.h"

#include "Adafruit_EPD.h"

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define BUSY_WAIT 500

// clang-format off

// Default init for FPC-A005 and unrecognized SSD1680 panels.
// VCOM, source voltage, and border corrected to match CircuitPython reference.
// Includes full 153-byte waveform LUT for grayscale operation.
const uint8_t ssd1680_default_init_code[] {
    SSD1680_SW_RESET, 0,                         // soft reset
    0xFF, 20,                                    // busy wait + 20 ms settle
    SSD1680_DATA_MODE, 1, 0x03,                  // RAM data entry mode
    SSD1680_WRITE_BORDER, 1, 0x03,               // border color
    SSD1680_WRITE_VCOM, 1, 0x28,                 // VCOM voltage (-2.0 V)
    SSD1680_GATE_VOLTAGE, 1, 0x17,               // gate voltage
    SSD1680_SOURCE_VOLTAGE, 3, 0x41, 0xae, 0x32, // source voltage
    SSD1680_SET_RAMXCOUNT, 1, 1,
    SSD1680_SET_RAMYCOUNT, 2, 0, 0,

    // Full waveform LUT — 153 bytes (5×12 VS + 12×7 timing + 9 FR/XON)
    SSD1680_WRITE_LUT, 153,
    // VS section: 5 rows × 12 bytes
    0x2a, 0x60, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L0
    0x20, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L1
    0x28, 0x60, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L2
    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L3
    0x00, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L4
    // Timing section: 12 groups × 7 bytes
    0x00, 0x02, 0x00, 0x05, 0x14, 0x00, 0x00,   // G0
    0x1e, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x01,   // G1
    0x00, 0x02, 0x00, 0x05, 0x14, 0x00, 0x00,   // G2
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G11
    // FR / XON section: 9 bytes
    0x24, 0x22, 0x22, 0x22, 0x23, 0x32, 0x00, 0x00, 0x00,

    0xFE // end
};

// Init for MagTag FPC-7519rev.b panels (User ID first byte 0x44 or 0xca).
// Uses GxEPD2_4G (GDEM029T94) LUT: L0/L3 swapped, FS byte 0x60→0x48 for DC
// balance, VCOM raised to 0x24 to reduce over-drive on these panels.
const uint8_t ssd1680_fpc7519_init_code[] {
    SSD1680_SW_RESET, 0,                         // soft reset
    0xFF, 20,                                    // busy wait + 20 ms settle
    SSD1680_DATA_MODE, 1, 0x03,                  // RAM data entry mode
    SSD1680_WRITE_BORDER, 1, 0x03,               // border color
    SSD1680_WRITE_VCOM, 1, 0x24,                 // VCOM voltage (tuned for FPC-7519rev.b)
    SSD1680_GATE_VOLTAGE, 1, 0x17,               // gate voltage
    SSD1680_SOURCE_VOLTAGE, 3, 0x41, 0xae, 0x32, // source voltage
    SSD1680_SET_RAMXCOUNT, 1, 1,                 // colstart = 8 (1 byte = 8 pixels)
    SSD1680_SET_RAMYCOUNT, 2, 0, 0,

    // Full waveform LUT — 153 bytes (5×12 VS + 12×7 timing + 9 FR/XON)
    SSD1680_WRITE_LUT, 153,
    // VS section: 5 rows × 12 bytes
    0x20, 0x48, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L0 black
    0x08, 0x48, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L1 lighter gray
    0x02, 0x48, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L2 darker gray
    0x40, 0x48, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L3 white
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // L4
    // Timing section: 12 groups × 7 bytes
    0x0a, 0x19, 0x00, 0x03, 0x08, 0x00, 0x00,   // G0
    0x14, 0x01, 0x00, 0x14, 0x01, 0x00, 0x03,   // G1
    0x0a, 0x03, 0x00, 0x08, 0x19, 0x00, 0x00,   // G2
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,   // G3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G5
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G6
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G7
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G8
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G9
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G10
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // G11
    // FR / XON section: 9 bytes
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00,

    0xFE // end
};

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
Adafruit_SSD1680::Adafruit_SSD1680(int width, int height, int16_t SID,
                                   int16_t SCLK, int16_t DC, int16_t RST,
                                   int16_t CS, int16_t SRCS, int16_t MISO,
                                   int16_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

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
Adafruit_SSD1680::Adafruit_SSD1680(int width, int height, int16_t DC,
                                   int16_t RST, int16_t CS, int16_t SRCS,
                                   int16_t BUSY, SPIClass* spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

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

  singleByteTxns = true;
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1680::busy_wait(void) {
  if (_busy_pin >= 0) {
    while (digitalRead(_busy_pin)) { // wait for busy low
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
void Adafruit_SSD1680::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true);  // black defaults to inverted
  setColorBuffer(1, false); // red defaults to un inverted
  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1680::update() {
  uint8_t buf[1];

  buf[0] = _display_update_val; // varies for mono vs gray4 mode
  EPD_command(SSD1680_DISP_CTRL2, buf, 1);
  EPD_command(SSD1680_MASTER_ACTIVATE);
  busy_wait();

  if (_busy_pin <= -1) {
    delay(1000);
  }
}

/**************************************************************************/
/*!
    @brief start up the display
*/
/**************************************************************************/
void Adafruit_SSD1680::powerUp() {
  uint8_t buf[5];

  hardwareReset();
  delay(100);
  busy_wait();

  const uint8_t* init_code;

  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  } else {
    init_code = ssd1680_default_init_code;
    // Default init embeds a full LUT; requires Display Mode 1.
    _display_update_val = 0xc7;
  }

  EPD_commandList(init_code);

  uint8_t height = HEIGHT;
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  // Set ram X start/end postion
  buf[0] = _xram_offset;
  buf[1] = height / 8 - 1 + _xram_offset;
  EPD_command(SSD1680_SET_RAMXPOS, buf, 2);

  // Set ram Y start/end postion
  buf[0] = 0x00;
  buf[1] = 0x00;
  buf[2] = (WIDTH - 1);
  buf[3] = (WIDTH - 1) >> 8;
  EPD_command(SSD1680_SET_RAMYPOS, buf, 4);

  // Apply caller-supplied LUT override if set (takes precedence over embedded
  // LUT)
  if (_epd_lut_code) {
    EPD_commandList(_epd_lut_code);
  }

  // Set display size and driver output control
  buf[0] = (WIDTH - 1);
  buf[1] = (WIDTH - 1) >> 8;
  buf[2] = 0x00;
  EPD_command(SSD1680_DRIVER_CONTROL, buf, 3);
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_SSD1680::powerDown() {
  uint8_t buf[1];
  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    // deep sleep
    buf[0] = 0x01;
    EPD_command(SSD1680_DEEP_SLEEP, buf, 1);
    delay(100);
  } else {
    EPD_command(SSD1680_SW_RESET);
    busy_wait();
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
uint8_t Adafruit_SSD1680::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(SSD1680_WRITE_RAM1, false);
  }
  if (index == 1) {
    return EPD_command(SSD1680_WRITE_RAM2, false);
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
void Adafruit_SSD1680::setRAMAddress(uint16_t x, uint16_t y) {
  (void)x;
  (void)y;

  uint8_t buf[2];

  // set RAM x address count
  buf[0] = _xram_offset;
  EPD_command(SSD1680_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count
  buf[0] = 0;
  buf[1] = 0;
  EPD_command(SSD1680_SET_RAMYCOUNT, buf, 2);
}
