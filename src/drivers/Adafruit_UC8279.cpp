#include "Adafruit_UC8279.h"

#include "Adafruit_EPD.h"

#define BUSY_WAIT 500

// clang-format off

const uint8_t uc8279_default_init_code[] {
    UC8279_GATE_SOURCE_START, 4,
	0x00, 0x00, 0x00, 0x00,

    UC8279_PFS, 1, 0x20,
    UC8279_PLL, 1, 0x0E,
    UC8279_GATE_SCAN, 1, 0x02,

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
Adafruit_UC8279::Adafruit_UC8279(int width, int height, int16_t SID,
                                 int16_t SCLK, int16_t DC, int16_t RST,
                                 int16_t CS, int16_t SRCS, int16_t MISO,
                                 int16_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = (uint32_t)width * (uint32_t)height / 8;
  buffer2_size = 0;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t*)malloc(buffer1_size);
    buffer2 = NULL;
  }

  singleByteTxns = true;
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
Adafruit_UC8279::Adafruit_UC8279(int width, int height, int16_t DC, int16_t RST,
                                 int16_t CS, int16_t SRCS, int16_t BUSY,
                                 SPIClass* spi)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = (uint32_t)width * (uint32_t)height / 8;
  buffer2_size = 0;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t*)malloc(buffer1_size);
    buffer2 = NULL;
  }

  singleByteTxns = true;
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_UC8279::busy_wait(void) {
  if (_busy_pin >= 0) {
    while (!digitalRead(_busy_pin)) { // wait for busy HIGH
      EPD_command(UC8279_GET_STATUS);
      delay(10);
    }
  } else {
    delay(BUSY_WAIT);
  }
  delay(200);
}

/**************************************************************************/
/*!
    @brief push blank (white) gate rows into the plane currently being written
    @param count number of gate rows to emit
*/
/**************************************************************************/
void Adafruit_UC8279::writeBlankGates(uint16_t count) {
  const uint16_t rowbytes = WIDTH / 8;
  for (uint16_t y = 0; y < count; y++) {
    for (uint16_t i = 0; i < rowbytes; i++) {
      SPItransfer(0xFF);
    }
  }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_UC8279::begin(bool reset) {
  // Same SRAM layout quirk as UC8179 sibling.
  _data_entry_mode = THINKINK_UC8179;

  // Set Gate Geometry and Scan Direction to defaults for 800x480 displays.
  setGateGeometry(_addressed_gates, _gate_offset);

  Adafruit_EPD::begin(reset);

  // Invert both for monochrome defaults
  setBlackBuffer(0, true);
  setColorBuffer(0, true);

  layer_colors[EPD_WHITE] = 0b00;
  layer_colors[EPD_BLACK] = 0b01;
  layer_colors[EPD_RED] = 0b01;
  layer_colors[EPD_GRAY] = 0b01;
  layer_colors[EPD_DARK] = 0b01;
  layer_colors[EPD_LIGHT] = 0b00;

  default_refresh_delay = 5000;

  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_UC8279::update() {
  EPD_command(UC8279_VCOM_DATA_INTERVAL, &_cdi, 1);
  EPD_command(UC8279_CCSET, &_ccset, 1);
  EPD_command(UC8279_TSSET, &_tsset, 1);

  EPD_command(UC8279_POWERON);
  busy_wait();

  // PON refreshes the panel's defaults, so PSR only latches when written after
  // it
  uint8_t psr[2] = {(uint8_t)(_psr0 & 0xDF), _psr1};
  EPD_command(UC8279_PANELSETTING, psr, 2);

  // mirrors UC8179 behavior
  EPD_command(UC8279_DISPLAYREFRESH);
  delay(100);
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
void Adafruit_UC8279::powerUp() {
  hardwareReset();

  uint8_t buf[4];

  // PSR with REG=1 selects the external LUT bank for init.
  buf[0] = _psr0;
  buf[1] = _psr1;
  EPD_command(UC8279_PANELSETTING, buf, 2);

  // Resolution is the ADDRESSED scan, not the visible area.
  buf[0] = WIDTH >> 8;
  buf[1] = WIDTH & 0xFF;
  buf[2] = _addressed_gates >> 8;
  buf[3] = _addressed_gates & 0xFF;
  EPD_command(UC8279_TRES, buf, 4);

  const uint8_t* init_code = uc8279_default_init_code;
  if (_epd_init_code != NULL) {
    init_code = _epd_init_code;
  }
  EPD_commandList(init_code);

  // Write the OLD plane with white pixels first, then the NEW plane with the
  // actual framebuffer
  EPD_command(UC8279_WRITE_RAM1, false);
  dcHigh();
  writeBlankGates(_addressed_gates);
  csHigh();
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_UC8279::powerDown() {
  uint8_t buf[1];

  EPD_command(UC8279_POWEROFF);
  busy_wait();

  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    buf[0] = 0xA5;
    EPD_command(UC8279_DEEPSLEEP, buf, 1);
  }
}

/***********************************************************************************/
/*!
    @brief Sets the controller's gate layout.
    @param addressedGates Number of gates the controller scans.
    @param visibleGateOffset Number of blank gates before the visible window
   starts.
*/
/***********************************************************************************/
void Adafruit_UC8279::setGateGeometry(uint16_t addressedGates,
                                      uint16_t visibleGateOffset) {
  _addressed_gates = addressedGates;
  _gate_offset = visibleGateOffset;
}

/*****************************************************************************/
/*!
    @brief Sets the controller's scan directions
    @param mirrorX True to reverse the horizontal source order, false otherwise.
    @param flipY True to reverse the gate scan order, false otherwise.
*/
/*****************************************************************************/
void Adafruit_UC8279::setScanDirection(bool mirrorX, bool flipY) {
  _psr0 &= ~0x0C;
  if (mirrorX) {
    _psr0 |= 0x04; // SHL
  }
  if (flipY) {
    _psr0 |= 0x08; // UD
  }
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write. Only 0 is meaningful on
   this monochrome driver; it selects the NEW plane.
    @returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Adafruit_UC8279::writeRAMCommand(uint8_t index) {
  if (index != 0) {
    return 0;
  }

  const uint8_t rv = EPD_command(UC8279_WRITE_RAM2, false);

  // Controller scans the entire area - guards for where gate geometry !=
  // visible geometry
  dcHigh();
  writeBlankGates(_gate_offset);

  return rv;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void Adafruit_UC8279::setRAMAddress(uint16_t x, uint16_t y) {
  // not used in this chip!
  (void)x;
  (void)y;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM window
    @param x1 start x
    @param y1 start y
    @param x2 end x
    @param y2 end y
*/
/**************************************************************************/
void Adafruit_UC8279::setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2,
                                   uint16_t y2) {
  // not used in this chip!
  (void)x1;
  (void)y1;
  (void)x2;
  (void)y2;
}
