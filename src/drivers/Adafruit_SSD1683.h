#ifndef LIB_ADAFRUIT_SSD1683
#define LIB_ADAFRUIT_SSD1683

#include <Arduino.h>

#include "Adafruit_EPD.h"

#define SSD1683_DRIVER_CONTROL 0x01
#define SSD1683_GATE_VOLTAGE 0x03
#define SSD1683_SOURCE_VOLTAGE 0x04
#define SSD1683_PROGOTP_INITIAL 0x08
#define SSD1683_PROGREG_INITIAL 0x09
#define SSD1683_READREG_INITIAL 0x0A
#define SSD1683_BOOST_SOFTSTART 0x0C
#define SSD1683_DEEP_SLEEP 0x10
#define SSD1683_DATA_MODE 0x11
#define SSD1683_SW_RESET 0x12
#define SSD1683_HV_READY 0x14
#define SSD1683_VCI_DETECT 0x15
#define SSD1683_PROGRAM_WSOTP 0x16
#define SSD1683_PROGRAM_AUTO 0x17
#define SSD1683_TEMP_CONTROL 0x18
#define SSD1683_TEMP_WRITE 0x1A
#define SSD1683_TEMP_READ 0x1B
#define SSD1683_TEMP_CONTROLEXT 0x1C
#define SSD1683_MASTER_ACTIVATE 0x20
#define SSD1683_DISP_CTRL1 0x21
#define SSD1683_DISP_CTRL2 0x22
#define SSD1683_WRITE_RAM1 0x24
#define SSD1683_WRITE_RAM2 0x26
#define SSD1683_READ_RAM1 0x27
#define SSD1683_SENSE_VCOM 0x28
#define SSD1683_SENSEDUR_VCOM 0x29
#define SSD1683_PROGOTP_VCOM 0x2A
#define SSD1683_WRITE_VCOM 0x2C
#define SSD1683_READ_OTP 0x2D
#define SSD1683_READ_USERID 0x2E
#define SSD1683_READ_STATUS 0x2F
#define SSD1683_WRITE_LUT 0x32
#define SSD1683_WRITE_BORDER 0x3C
#define SSD1683_END_OPTION 0x3F
#define SSD1683_SET_RAMXPOS 0x44
#define SSD1683_SET_RAMYPOS 0x45
#define SSD1683_SET_RAMXCOUNT 0x4E
#define SSD1683_SET_RAMYCOUNT 0x4F

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1683 EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1683 : public Adafruit_EPD {
 public:
  Adafruit_SSD1683(int width, int height, int16_t SID, int16_t SCLK, int16_t DC,
                   int16_t RST, int16_t CS, int16_t SRCS, int16_t MISO,
                   int16_t BUSY = -1);
  Adafruit_SSD1683(int width, int height, int16_t DC, int16_t RST, int16_t CS,
                   int16_t SRCS, int16_t BUSY = -1, SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void powerUp();
  void update(void);
  void updatePartial(void);
  void powerDown();
  void displayPartial(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

 protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void setRAMWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void busy_wait();
  uint8_t _display_update_val = 0xF7;
};

#endif
