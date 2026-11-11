/*!
 * @file Adafruit_EPD.h
 *
 * This is a library for our EPD displays based on EPD drivers.
 * Designed specifically to work with Adafruit EPD displays.
 *
 * These displays use SPI to communicate, 4 or 5 pins are required to
 * interface
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#ifndef _ADAFRUIT_EPD_H_
#define _ADAFRUIT_EPD_H_

// #define EPD_DEBUG

#define RAMBUFSIZE 64 ///< size of the ram buffer

#define EPD_DEFAULT_SPI_HZ 4000000 ///< SPI clock for normal updates

#include <Adafruit_GFX.h>
#include <Adafruit_SPIDevice.h>

#include "Adafruit_MCPSRAM.h"

/**************************************************************************/
/*!
    @brief available EPD colors
*/
/**************************************************************************/
enum {
  EPD_WHITE,  ///< white color
  EPD_BLACK,  ///< black color
  EPD_RED,    ///< red color
  EPD_GRAY,   ///< gray color ('red' on grayscale)
  EPD_DARK,   ///< darker color
  EPD_LIGHT,  ///< lighter color
  EPD_YELLOW, ///< fourth color on some displays
  EPD_NUM_COLORS
};

/// SRAM data entry mode, selects how the external SRAM is organized
typedef enum {
  THINKINK_STANDARD = 0, ///< 99% of panels use this setup!
  THINKINK_UC8179 = 1,   ///< .... except for UC8179?
} thinkink_sramentrymode_t;

/// Ink mode for ThinkInk displays, passed to begin()
typedef enum {
  THINKINK_MONO,         ///< monochrome (black and white)
  THINKINK_TRICOLOR,     ///< black, white and red/yellow
  THINKINK_GRAYSCALE4,   ///< 4-level grayscale
  THINKINK_MONO_PARTIAL, ///< monochrome with partial refresh
  THINKINK_QUADCOLOR,    ///< black, white, red and yellow
} thinkinkmode_t;

#define EPD_swap(a, b) \
  {                    \
    int16_t t = a;     \
    a = b;             \
    b = t;             \
  } ///< simple swap function

/**************************************************************************/
/*!
    @brief  Class for interfacing with Adafruit EPD display breakouts.
*/
/**************************************************************************/
class Adafruit_EPD : public Adafruit_GFX {
 public:
  Adafruit_EPD(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
               int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
               int16_t BUSY = -1);
  Adafruit_EPD(int width, int height, int16_t DC, int16_t RST, int16_t CS,
               int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);
  ~Adafruit_EPD();

  void begin(bool reset = true);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void clearBuffer();
  void clearDisplay();
  void setBlackBuffer(int8_t index, bool inverted);
  void setColorBuffer(int8_t index, bool inverted);
  void display(bool sleep = false);

  /**************************************************************************/
  /*!
    @brief Get the ink mode the display was started with
    @returns The current thinkinkmode_t ink mode
  */
  /**************************************************************************/
  thinkinkmode_t getMode(void) {
    return inkmode;
  }

  // Fast lag-drive updates (mono only). See Adafruit_EPD_fast.cpp.
  bool fastBegin(uint8_t lag = 6, uint32_t spi_hz = 20000000);
  bool displayFast(void);
  void fastSettle(void);
  void fastEnd(void);

  /**************************************************************************/
  /*!
    @brief Check whether fast mode is active
    @returns true from a successful fastBegin() until fastEnd(), display() or
    begin()
  */
  /**************************************************************************/
  bool fastActive(void) {
    return _fast_ring != NULL;
  }

  /**************************************************************************/
  /*!
    @brief Time the panel spent BUSY on the last fast update
    @returns microseconds
  */
  /**************************************************************************/
  uint32_t fastBusyMicros(void) {
    return _fast_busy_us;
  }

  /**************************************************************************/
  /*!
    @brief Bytes sent to the panel RAM on the last fast update (both planes)
    @returns Byte count, or 0 if the update was skipped
  */
  /**************************************************************************/
  uint32_t fastBytes(void) {
    return _fast_bytes;
  }

 protected:
  /**************************************************************************/
  /*!
    @brief Load the fast waveform and prepare the controller for
    displayFast(). The panel RAM planes hold the image shown on screen.
    Drivers that support fast mode override this and set _fast_stride and
    _fast_rows.
    @returns false if this driver has no fast mode
  */
  /**************************************************************************/
  virtual bool fastModeEnter(void) {
    return false;
  }

  /**************************************************************************/
  /*!
    @brief Check for fast mode support without touching the panel
    @returns true if this driver implements the fast hooks
  */
  /**************************************************************************/
  virtual bool fastSupported(void) {
    return false;
  }

  /**************************************************************************/
  /*!
    @brief Full OTP refresh to white. Leaves both RAM planes white.
  */
  /**************************************************************************/
  virtual void fastFullClean(void) {}

  /**************************************************************************/
  /*!
    @brief Set the RAM window and address counters for a fast update
    @param xb0 first RAM column, in bytes
    @param xb1 last RAM column, in bytes (inclusive)
    @param y0 first RAM row
    @param y1 last RAM row (inclusive)
  */
  /**************************************************************************/
  virtual void fastWindow(uint16_t xb0, uint16_t xb1, uint16_t y0,
                          uint16_t y1) {
    (void)xb0, (void)xb1, (void)y0, (void)y1;
  }

  /**************************************************************************/
  /*!
    @brief Start one fast update. Must not wait for BUSY.
  */
  /**************************************************************************/
  virtual void fastUpdate(void) {}

  /**************************************************************************/
  /*!
    @brief Read the BUSY pin
    @returns true while the controller is busy
  */
  /**************************************************************************/
  bool fastIsBusy(void) {
    return digitalRead(_busy_pin) == _fast_busy_level;
  }

  uint8_t _fast_busy_level = HIGH; ///< BUSY pin level while the panel is busy
  uint16_t _fast_stride = 0;       ///< bytes per RAM row in fast mode
  uint16_t _fast_rows = 0;         ///< RAM rows in fast mode
  uint8_t _fast_frames = 2;        ///< drive frames per fast update
  uint8_t _fast_rate = 0x08;       ///< frame rate code for the fast waveform
  void fastFill(uint8_t ram, uint8_t value);

  thinkink_sramentrymode_t _data_entry_mode =
      THINKINK_STANDARD; ///< how framebuffer data is laid out in SRAM

  void writeRAMFramebufferToEPD(uint8_t* buffer, uint32_t buffer_size,
                                uint8_t EPDlocation, bool invertdata = false);
  void writeSRAMFramebufferToEPD(uint16_t SRAM_buffer_addr,
                                 uint32_t buffer_size, uint8_t EPDlocation,
                                 bool invertdata = false);

  /**************************************************************************/
  /*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color
    displays) Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the
    command
  */
  /**************************************************************************/
  virtual uint8_t writeRAMCommand(uint8_t index) = 0;

  /**************************************************************************/
  /*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
  */
  /**************************************************************************/
  virtual void setRAMAddress(uint16_t x, uint16_t y) = 0;

  /**************************************************************************/
  /*!
    @brief Wait for the display to finish a busy operation
  */
  /**************************************************************************/
  virtual void busy_wait(void) = 0;

  /**************************************************************************/
  /*!
    @brief start up the display
  */
  /**************************************************************************/
  virtual void powerUp() = 0;

  /**************************************************************************/
  /*!
    @brief signal the display to update
  */
  /**************************************************************************/
  virtual void update(void) = 0;

  /**************************************************************************/
  /*!
    @brief wind down the display
  */
  /**************************************************************************/
  virtual void powerDown(void) = 0;
  void hardwareReset(void);

  int16_t _dc_pin,                    ///< data/command pin
      _reset_pin,                     ///< reset pin
      _cs_pin,                        ///< chip select pin
      _busy_pin;                      ///< busy pin
  Adafruit_SPIDevice* spi_dev = NULL; ///< SPI object
  static bool _isInTransaction;       ///< true if SPI bus is in trasnfer state
  bool singleByteTxns; ///< if true CS will go high after every data byte
                       ///< transferred

  const uint8_t* _epd_init_code = NULL; ///< init command list
  const uint8_t* _epd_lut_code = NULL;  ///< waveform LUT command list
  const uint8_t* _epd_partial_init_code =
      NULL; ///< init command list for partial refresh
  const uint8_t* _epd_partial_lut_code =
      NULL; ///< waveform LUT command list for partial refresh

  uint16_t default_refresh_delay = 15000; ///< refresh delay in ms (no busy pin)

  Adafruit_MCPSRAM sram; ///< the ram chip object if using off-chip ram

  bool blackInverted; ///< is black channel inverted
  bool colorInverted; ///< is red channel inverted

  uint8_t layer_colors[EPD_NUM_COLORS]; ///< buffer layer bits for each color

  uint32_t buffer1_size; ///< size of the primary buffer
  uint32_t buffer2_size; ///< size of the secondary buffer
  uint8_t* buffer1; ///< the pointer to the primary buffer if using on-chip ram
  uint8_t*
      buffer2; ///< the pointer to the secondary buffer if using on-chip ram
  uint8_t*
      color_buffer; ///< the pointer to the color buffer if using on-chip ram
  uint8_t*
      black_buffer; ///< the pointer to the black buffer if using on-chip ram
  uint16_t buffer1_addr; ///< The SRAM address offsets for the primary buffer
  uint16_t buffer2_addr; ///< The SRAM address offsets for the secondary buffer
  uint16_t colorbuffer_addr; ///< The SRAM address offsets for the color buffer
  uint16_t blackbuffer_addr; ///< The SRAM address offsets for the black buffer

  void EPD_commandList(const uint8_t* init_code);
  void EPD_command(uint8_t c, const uint8_t* buf, uint16_t len);
  uint8_t EPD_command(uint8_t c, bool end = true);
  void EPD_data(const uint8_t* buf, uint16_t len);
  void EPD_data(uint8_t data);

  uint8_t SPItransfer(uint8_t c);

  bool use_sram; ///< true if we are using an SRAM chip as a framebuffer

  thinkinkmode_t inkmode; ///< Ink mode passed to begin()

  uint8_t partialsSinceLastFullUpdate =
      0; ///< partial refreshes since the last full refresh

#if defined(BUSIO_USE_FAST_PINIO)
  BusIO_PortReg *csPort, *dcPort;
  BusIO_PortMask csPinMask, dcPinMask;
#endif

  void csLow();
  void csHigh();
  void dcHigh();
  void dcLow();

 private:
  /**************************************************************************/
  /*!
    @brief Bytes in one full RAM plane in fast mode
    @returns _fast_stride * _fast_rows
  */
  /**************************************************************************/
  uint32_t fastFrameBytes(void) {
    return (uint32_t)_fast_stride * _fast_rows;
  }

  bool fastStep(const uint8_t* frame);
  void fastStart(void);
  void fastWaitIdle(void);
  void fastWriteBox(uint8_t ram, const uint8_t* src, const int16_t* box);
  void fastSetSPI(uint32_t hz);
  void fastRelease(void);
  const uint8_t* fastHeadFrame(void);

  uint8_t* _fast_ring = NULL; ///< lag + 1 frames, newest at _fast_head
  uint8_t _fast_lag = 0;
  uint8_t _fast_head = 0;
  int16_t _fast_prev[4] = {0, -1, 0, -1}; ///< previous step box xb0,xb1,y0,y1
  bool _fast_prev_valid = false;
  bool _fast_pending = false; ///< an update may still be running
  uint32_t _fast_busy_us = 0;
  uint32_t _fast_bytes = 0;
  uint32_t _fast_t0 = 0;
  SPIClass* _spi_bus = NULL; ///< hardware SPI bus, NULL for software SPI
};

#include "drivers/Adafruit_ACeP.h"
#include "drivers/Adafruit_EK79686.h"
#include "drivers/Adafruit_IL0373.h"
#include "drivers/Adafruit_IL0398.h"
#include "drivers/Adafruit_IL91874.h"
#include "drivers/Adafruit_JD79661.h"
#include "drivers/Adafruit_JD79667.h"
#include "drivers/Adafruit_SSD1608.h"
#include "drivers/Adafruit_SSD1619.h"
#include "drivers/Adafruit_SSD1675.h"
#include "drivers/Adafruit_SSD1675B.h"
#include "drivers/Adafruit_SSD1677.h"
#include "drivers/Adafruit_SSD1680.h"
#include "drivers/Adafruit_SSD1681.h"
#include "drivers/Adafruit_SSD1683.h"
#include "drivers/Adafruit_UC8151D.h"
#include "drivers/Adafruit_UC8179.h"
#include "drivers/Adafruit_UC8253.h"
#include "drivers/Adafruit_UC8276.h"

#endif /* _ADAFRUIT_EPD_H_ */
