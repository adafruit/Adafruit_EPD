#include "Adafruit_SSD1676.h"
#include "Adafruit_EPD.h"


#define EPD_CS     10
#define EPD_DC      9
#define EPD_RESET   5 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    6 // can set to -1 to not use a pin (will wait a fixed delay)
#define BUSY_WAIT 500

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
Adafruit_SSD1676::Adafruit_SSD1676(int width, int height, int8_t SID,
                                   int8_t SCLK, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t MISO,
                                   int8_t BUSY)
    : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = width * height / 8;
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
Adafruit_SSD1676::Adafruit_SSD1676(int width, int height, int8_t DC, int8_t RST,
                                   int8_t CS, int8_t SRCS, int8_t BUSY)
    : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY) {
  if ((height % 8) != 0) {
    height += 8 - (height % 8);
  }

  buffer1_size = width * height / 8;
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

  singleByteTxns = true;
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void Adafruit_SSD1676::busy_wait(void) {
  if (_busy_pin >= 0) {
    while (digitalRead(_busy_pin)) { // wait for busy low
      Serial.print(".");
      delay(10);
    }
  } else {
    delay(BUSY_WAIT);
  }
  Serial.println();
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_SSD1676::begin(bool reset) {
  Adafruit_EPD::begin(reset);
  //setBlackBuffer(0, true); // black defaults to inverted
  //setColorBuffer(0, true); // no secondary buffer, so we'll just reuse index 0
  Serial.println("power down");
  //powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_SSD1676::update() {
  uint8_t buf[1];

  // display update sequence
  //buf[0] = 0xC7;
  //EPD_command(SSD1676_DISP_CTRL2, buf, 1);

  EPD_command(SSD1676_MASTER_ACTIVATE);
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
void Adafruit_SSD1676::powerUp() {
  uint8_t buf[5];

  hardwareReset();
  delay(100);
  busy_wait();

  // soft reset
  EPD_command(SSD1676_SW_RESET);
  busy_wait();

  // Set display size and driver output control
  buf[0] = 0x27;
  buf[1] = 0x01;
  buf[2] = 0x00;
  EPD_command(SSD1676_DRIVER_CONTROL, buf, 3);

  // Ram data entry mode
  buf[0] = 0x01;
  EPD_command(SSD1676_DATA_MODE, buf, 1);

  // Set ram X start/end postion
  buf[0] = 0x01;
  buf[1] = 0x10;
  EPD_command(SSD1676_SET_RAMXPOS, buf, 2);

  // Set ram Y start/end postion
  buf[0] = 0x27;
  buf[1] = 0x01;
  buf[2] = 0x00;
  buf[3] = 0x00;
  EPD_command(SSD1676_SET_RAMYPOS, buf, 4);

  // border color
  buf[0] = 0x05;
  EPD_command(SSD1676_WRITE_BORDER, buf, 1);

  // Vcom Voltage
  buf[0] = 0x36;
  EPD_command(SSD1676_WRITE_VCOM, buf, 1);

  // Set gate voltage
  buf[0] = 0x17;
  EPD_command(SSD1676_GATE_VOLTAGE, buf, 1);

  // Set source voltage
  buf[0] = 0x41;
  buf[1] = 0x00;
  buf[2] = 0x32;
  EPD_command(SSD1676_SOURCE_VOLTAGE, buf, 3);

  // Set LUT
  /*
  buf[0] = LUT_DATA[74];
  EPD_command(SSD1676_WRITE_LUT, buf, 1);
  EPD_command(SSD1676_WRITE_LUT, LUT_DATA, 70);
  */

  // set RAM x address count
  buf[0] = 1;
  EPD_command(SSD1676_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count
  buf[0] = 0x27;
  buf[1] = 0x01;
  EPD_command(SSD1676_SET_RAMYCOUNT, buf, 2);

}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void Adafruit_SSD1676::powerDown() {
  uint8_t buf[1];
  // Only deep sleep if we can get out of it
  if (_reset_pin >= 0) {
    // deep sleep
    buf[0] = 0x01;
    EPD_command(SSD1676_DEEP_SLEEP, buf, 1);
    delay(100);
  } else {
    EPD_command(SSD1676_SW_RESET);
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
uint8_t Adafruit_SSD1676::writeRAMCommand(uint8_t index) {
  if (index == 0) {
    return EPD_command(SSD1676_WRITE_RAM1, false);
  }
  if (index == 1) {
    return EPD_command(SSD1676_WRITE_RAM2, false);
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
void Adafruit_SSD1676::setRAMAddress(uint16_t x, uint16_t y) {
  uint8_t buf[2];

  // set RAM x address count
  buf[0] = 1;
  EPD_command(SSD1676_SET_RAMXCOUNT, buf, 1);

  // set RAM y address count
  buf[0] = 0x27;
  buf[1] = 0x01;
  EPD_command(SSD1676_SET_RAMYCOUNT, buf, 2);
}



  
void Adafruit_SSD1676::EpaperIO_Init(void)
{
  pinMode(EPD_RESET, OUTPUT);
  digitalWrite(EPD_RESET, LOW);
  pinMode(EPD_CS, OUTPUT);
  pinMode(EPD_DC, OUTPUT);
  pinMode(EPD_BUSY, INPUT);
  pinMode(SCK, OUTPUT);
  pinMode(MOSI, OUTPUT);
}


void Adafruit_SSD1676::Epaper_Spi_WriteByte(uint8_t TxData)
{                
  uint8_t TempData;
  uint8_t scnt;
  TempData=TxData;
  digitalWrite(SCK, LOW); 
  for(scnt=0; scnt<8; scnt++) { 
    if (TempData&0x80)
      digitalWrite(MOSI, HIGH);
    else
      digitalWrite(MOSI, LOW);
    digitalWrite(SCK, HIGH);
    delayMicroseconds(5) ; 
    digitalWrite(SCK, LOW);
    TempData=TempData<<1;
    delayMicroseconds(5) ; 
  }
}



/*
 * º¯ÊýÃû£ºEpaper_Write_Command
 * ÃèÊö  £ºÐ´ÃüÁî
 * ÊäÈë  £ºcmd
 * Êä³ö  £ºÎÞ
 */ 
void Adafruit_SSD1676::Epaper_Write_Command (uint8_t cmd) {
  digitalWrite(EPD_CS, HIGH);
  digitalWrite(EPD_CS, LOW);
  digitalWrite(EPD_DC, LOW);  // D/C#   0:command  1:data

  delayMicroseconds(5) ;
  Epaper_Spi_WriteByte(cmd);
  delayMicroseconds(5) ;
  digitalWrite(EPD_CS, HIGH);
}

/*
 * º¯ÊýÃû£ºEpaper_Write_CommandR
 * ÃèÊö  £º¶ÁÊý¾ÝµÄÐ´ÃüÁî
 * ÊäÈë  £ºcmd
 * Êä³ö  £ºÎÞ
 */ 

void Adafruit_SSD1676::Epaper_Write_CommandR(uint8_t cmd) {
  digitalWrite(EPD_CS, HIGH);
  digitalWrite(EPD_CS, LOW);
  digitalWrite(EPD_DC, LOW);  // D/C#   0:command  1:data

  delayMicroseconds(5);
  Epaper_Spi_WriteByte(cmd);
  delayMicroseconds(5);
}

/*
 * º¯ÊýÃû£ºEpaper_Write_Data
 * ÃèÊö  £ºÐ´Êý¾Ý
 * ÊäÈë  £ºdata
 * Êä³ö  £ºÎÞ
 */ 
void Adafruit_SSD1676::Epaper_Write_Data(uint8_t data)
{
  digitalWrite(EPD_CS, HIGH);
  digitalWrite(EPD_CS, LOW);
  digitalWrite(EPD_DC, HIGH);  // D/C#   0:command  1:data
  
  delayMicroseconds(5);
  Epaper_Spi_WriteByte(data);
  delayMicroseconds(5);
  digitalWrite(EPD_CS, HIGH);
}



/*
 * º¯ÊýÃû£ºEpaper_Update_and_Deepsleep
 * ÃèÊö  £º¸üÐÂ&½øÈëÉî¶ÈË¯Ãß
 * ÊäÈë  £ºÎÞ
 * Êä³ö  £ºÎÞ
 */ 
void Adafruit_SSD1676::Epaper_Update_and_Deepsleep(void) {
       
  Epaper_Write_Command(SSD1676_MASTER_ACTIVATE);
  busy_wait();
  
  Epaper_Write_Command(SSD1676_DEEP_SLEEP);
  Epaper_Write_Data(0x01);   
  delay(100); 
}

/*
 * º¯ÊýÃû£ºEpaper_Init
 * ÃèÊö  £ºµç×ÓÖ½³õÊ¼»¯³ÌÐò
 * ÊäÈë  £ºÎÞ          
 * Êä³ö  £ºÎÞ
 */ 

void Adafruit_SSD1676::Epaper_Init(void) {
    delay(10);
    digitalWrite(EPD_RESET, LOW);  //EPD Ó²¼þ¸´Î»    Hardware  reset
    delay(10); 
    digitalWrite(EPD_RESET, HIGH);      // EPD Ó²¼þ¸´Î»ÊÍ·Å    Hard  reset  release
    delay(10);   
    busy_wait();   //¶ÁbusyÐÅºÅ
    Epaper_Write_Command(0x12);   // Èí¼þ¸´Î»    soft  reset
    busy_wait();
}

  


/*
 * º¯ÊýÃû£ºDisplay_All_White
 * ÃèÊö  £ºË¢ÐÂÏÔÊ¾È«°×
 * ÊäÈë  £ºÎÞ
 * Êä³ö  £ºÎÞ
 */ 
void Adafruit_SSD1676::Display_All_White(void)
{
  uint32_t i,j; 

    

    Epaper_Write_Command(0x24); 
  
    for(i=0;i<296;i++)
   {
     for(j=0;j<16;j++)
     {
      Epaper_Write_Data(0xFF);
     }
   }
  

  

    Epaper_Write_Command(0x26);   
    for(i=0;i<296;i++)
   {
     for(j=0;j<16;j++)
     {
      Epaper_Write_Data(0x00);
     }
   }
} 

/*
 * º¯ÊýÃû£ºDisplay_All_Black
 * ÃèÊö  £ºË¢ÐÂÏÔÊ¾È«ºÚ
 * ÊäÈë  £ºÎÞ
 * Êä³ö  £ºÎÞ
 */ 
void Adafruit_SSD1676::Display_All_Black(void)
{
  uint32_t i,j; 
  Epaper_Write_Command(0x24);   
  for(i=0;i<296;i++) {
    for(j=0;j<16;j++) {
      Epaper_Write_Data(0x00);
    }
  }
  
  Epaper_Write_Command(0x26);   
  for(i=0;i<296;i++) {
    for(j=0;j<16;j++) {
      Epaper_Write_Data(0x00);
    }
  }
}

/*
 * º¯ÊýÃû£ºDisplay_All_Red
 * ÃèÊö  £ºË¢ÐÂÏÔÊ¾È«ºì
 * ÊäÈë  £ºÎÞ
 * Êä³ö  £ºÎÞ
 */ 
void Adafruit_SSD1676::Display_All_Red(void)
{
  uint32_t i,j; 

  Epaper_Write_Command(0x4E);     
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x4F);       
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);


  busy_wait();
  Epaper_Write_Command(0x24);   
  for(i=0;i<296;i++) {
     for (j=0; j<16;j++) {
       Epaper_Write_Data(0xFF);
   }
 }
   
  Epaper_Write_Command(0x4E);     
  Epaper_Write_Data(0x00);

  Epaper_Write_Command(0x4F);       
  Epaper_Write_Data(0x27);
  Epaper_Write_Data(0x01);


  busy_wait();
  Epaper_Write_Command(0x26);   
  for(i=0;i<296;i++) {
    for(j=0;j<16;j++)  {
      Epaper_Write_Data(0xFF);
   }
 }
}

