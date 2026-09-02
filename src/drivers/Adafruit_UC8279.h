#ifndef LIB_ADAFRUIT_UC8279
#define LIB_ADAFRUIT_UC8279

#include <Arduino.h>

#include "Adafruit_EPD.h"

// registers via https://github.com/Free-Ink/freeink-sdk/blob/main/docs/display-driver-references.md
#define UC8279_PANELSETTING 0x00     ///< PSR (2 data bytes)
#define UC8279_POWEROFF 0x02         ///< POF
#define UC8279_PFS 0x03              ///< power-off sequence
#define UC8279_POWERON 0x04          ///< PON
#define UC8279_DEEPSLEEP 0x07        ///< DSLP (check code 0xA5)
#define UC8279_WRITE_RAM1 0x10       ///< DTM1 — OLD plane in KW mode
#define UC8279_DISPLAYREFRESH 0x12   ///< DRF
#define UC8279_WRITE_RAM2 0x13       ///< DTM2 — NEW plane in KW mode
#define UC8279_PLL 0x30              ///< PLL frame rate
#define UC8279_VCOM_DATA_INTERVAL 0x50 ///< CDI (1 data byte on this part)
#define UC8279_TRES 0x61             ///< resolution
#define UC8279_GATE_SOURCE_START 0x65 ///< GSST (4 data bytes)
#define UC8279_GET_STATUS 0x71       ///< status readback
#define UC8279_PARTIAL_WINDOW 0x90   ///< PTL
#define UC8279_PARTIAL_IN 0x91       ///< PTIN
#define UC8279_PARTIAL_OUT 0x92      ///< PTOUT
#define UC8279_CCSET 0xE0            ///< cascade / output enable
#define UC8279_GATE_SCAN 0xE1        ///< gate-scan selection
#define UC8279_TSSET 0xE5            ///< forced temperature

/**************************************************************************/
/*!
    @brief  Class for interfacing with UC8279 EPD drivers.
*/
/**************************************************************************/
class Adafruit_UC8279 : public Adafruit_EPD {
 public:
  Adafruit_UC8279(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                  int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                  int16_t BUSY = -1);
  Adafruit_UC8279(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                  int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void update(void);
  void powerDown();

 protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void busy_wait();

  void setGateGeometry(uint16_t addressedGates, uint16_t visibleGateOffset);
  void writeBlankGates(uint16_t count);
  uint16_t _addressed_gates = 600;  ///< Gates the controller scans (TRES height)
  uint16_t _gate_offset = 120;      ///< Blank gates before the visible window

  void setScanDirection(bool mirrorX, bool flipY);
  uint8_t _psr0 = 0x33;      ///< PSR byte 0 at init
  uint8_t _psr1 = 0x4D;      ///< PSR byte 1
  uint8_t _pfs = 0x20;       ///< power-off sequence
  uint8_t _pll = 0x0E;       ///< PLL frame rate
  uint8_t _gate_scan = 0x02; ///< gate-scan selection
  uint8_t _ccset = 0x02;     ///< cascade setting
  uint8_t _tsset = 0x1E;     ///< forced temperature, full refresh
  uint8_t _cdi = 0x97;       ///< VCOM/data interval, full refresh
};

#endif
