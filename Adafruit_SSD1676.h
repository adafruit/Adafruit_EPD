#ifndef LIB_ADAFRUIT_SSD1676
#define LIB_ADAFRUIT_SSD1676

#include "Adafruit_EPD.h"
#include <Arduino.h>

#define EPD_RAM_BW 0x10
#define EPD_RAM_RED 0x13

#define SSD1676_DRIVER_CONTROL 0x01
#define SSD1676_GATE_VOLTAGE 0x03
#define SSD1676_SOURCE_VOLTAGE 0x04
#define SSD1676_PROGOTP_INITIAL 0x08
#define SSD1676_PROGREG_INITIAL 0x09
#define SSD1676_READREG_INITIAL 0x0A
#define SSD1676_BOOST_SOFTSTART 0x0C
#define SSD1676_DEEP_SLEEP 0x10
#define SSD1676_DATA_MODE 0x11
#define SSD1676_SW_RESET 0x12
//#define SSD1676_HV_READY 0x14
//#define SSD1676_VCI_READY 0x15
#define SSD1676_TEMP_CONTROL 0x18
#define SSD1676_TEMP_WRITE 0x1A
#define SSD1676_MASTER_ACTIVATE 0x20
#define SSD1676_DISP_CTRL1 0x21
#define SSD1676_DISP_CTRL2 0x22
#define SSD1676_WRITE_RAM1 0x24
#define SSD1676_WRITE_RAM2 0x26
#define SSD1676_WRITE_VCOM 0x2C
#define SSD1676_READ_OTP 0x2D
#define SSD1676_READ_STATUS 0x2F
#define SSD1676_WRITE_LUT 0x32
//#define SSD1676_WRITE_DUMMY 0x3A
//#define SSD1676_WRITE_GATELINE 0x3B
#define SSD1676_WRITE_BORDER 0x3C
#define SSD1676_SET_RAMXPOS 0x44
#define SSD1676_SET_RAMYPOS 0x45
#define SSD1676_SET_RAMXCOUNT 0x4E
#define SSD1676_SET_RAMYCOUNT 0x4F
//#define SSD1676_SET_ANALOGBLOCK 0x74
//#define SSD1676_SET_DIGITALBLOCK 0x7E

/**************************************************************************/
/*!
    @brief  Class for interfacing with SSD1676 EPD drivers
*/
/**************************************************************************/
class Adafruit_SSD1676 : public Adafruit_EPD {
public:
  Adafruit_SSD1676(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                   int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO,
                   int8_t BUSY = -1);
  Adafruit_SSD1676(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                   int8_t SRCS, int8_t BUSY = -1);

  void begin(bool reset = true);
  void powerUp();
  void update();
  void powerDown();

void Epaper_Write_Command(uint8_t cmd);
void Epaper_Write_CommandR(uint8_t cmd);
void Epaper_Write_Data(uint8_t data);
 void Epaper_Spi_WriteByte(uint8_t TxData);
void Epaper_Update_and_Deepsleep(void);
void Epaper_Init(void);
void Display_All_Red(void);
void Display_All_Black(void);
void Display_All_White(void);
 void EpaperIO_Init(void);

protected:
  uint8_t writeRAMCommand(uint8_t index);
  void setRAMAddress(uint16_t x, uint16_t y);
  void busy_wait();
};


/*******************************************************************************/
#define MAX_LINE_BYTES     16
#define MAX_COLUMN_BYTES   296
#define ALLSCREEN_BYTES   4736
#define SPI_OUT 0
#define SPI_IN 1
#define MONO 0
#define RED 1


#endif

