#ifndef LIB_ADAFRUIT_SSD1675
#define LIB_ADAFRUIT_SSD1675

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define SSD1675_DRIVER_CONTROL 0x01
#define SSD1675_GATE_VOLTAGE 0x03
#define SSD1675_SOURCE_VOLTAGE 0x04
#define SSD1675_DEEP_SLEEP 0x10
#define SSD1675_DATA_MODE 0x11
#define SSD1675_SW_RESET 0x12
#define SSD1675_HV_READY 0x14
#define SSD1675_VCI_READY 0x15
#define SSD1675_TEMP_WRITE 0x1A
#define SSD1675_MASTER_ACTIVATE 0x20
#define SSD1675_DISP_CTRL1 0x21
#define SSD1675_DISP_CTRL2 0x22
#define SSD1675_WRITE_RAM1 0x24
#define SSD1675_WRITE_RAM2 0x26
#define SSD1675_WRITE_VCOM 0x2C
#define SSD1675_READ_OTP 0x2D
#define SSD1675_READ_STATUS 0x2F
#define SSD1675_WRITE_LUT 0x32
#define SSD1675_WRITE_DUMMY 0x3A
#define SSD1675_WRITE_GATELINE 0x3B
#define SSD1675_WRITE_BORDER 0x3C
#define SSD1675_SET_RAMXPOS 0x44
#define SSD1675_SET_RAMYPOS 0x45
#define SSD1675_SET_RAMXCOUNT 0x4E
#define SSD1675_SET_RAMYCOUNT 0x4F
#define SSD1675_SET_ANALOGBLOCK 0x74
#define SSD1675_SET_DIGITALBLOCK 0x7E


/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1675 EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1675 : public Adafruit_EPD {
 public:  
  Adafruit_SSD1675(int width, int height, int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY = -1);
  Adafruit_SSD1675(int width, int height, int8_t DC, int8_t RST, int8_t CS, int8_t SRCS, int8_t BUSY = -1);

  void begin(bool reset=true);
  void powerUp();
  void update();
  void powerDown();

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};

#endif
