/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_THINKINK // detects if compiling for
                                                // Feather RP2040 ThinkInk
#define EPD_DC PIN_EPD_DC       // ThinkInk 24-pin connector DC
#define EPD_CS PIN_EPD_CS       // ThinkInk 24-pin connector CS
#define EPD_BUSY PIN_EPD_BUSY   // ThinkInk 24-pin connector Busy
#define SRAM_CS -1              // use onboard RAM
#define EPD_RESET PIN_EPD_RESET // ThinkInk 24-pin connector Reset
#define EPD_SPI &SPI1           // secondary SPI for ThinkInk
#else
#define EPD_DC 10
#define EPD_CS 9
#define EPD_BUSY 7 // can set to -1 to not use a pin (will wait a fixed delay)
#define SRAM_CS 6
#define EPD_RESET 8  // can set to -1 and share with microcontroller Reset!
#define EPD_SPI &SPI // primary SPI
#endif

// 1.54" 152x152 Tricolor EPD with ILI0373 chipset
// ThinkInk_154_Tricolor_Z17 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 1.54" 152x152 Tricolor EPD with SSD1680 chipset
// ThinkInk_154_Tricolor_RW display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 1.54" 200x200 Tricolor EPD with SSD1681 chipset
// ThinkInk_154_Tricolor_Z90 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 2.13" Tricolor EPD with SSD1680 chipset
ThinkInk_213_Tricolor_RW display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                 EPD_SPI);

// 2.13" Tricolor EPD with SSD1680Z chipset
// ThinkInk_213_Tricolor_MFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, 
//                                     EPD_BUSY, EPD_SPI);

// 2.13" Tricolor EPD with IL0373 chipset
// ThinkInk_213_Tricolor_Z16 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);


// 2.66" Tricolor EPD with SSD1680Z chipset
// ThinkInk_266_Tricolor_MFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, 
//                                     EPD_BUSY, EPD_SPI);


// 2.7" Tricolor Featherwing or Breakout with IL91874 chipset
// ThinkInk_270_Tricolor_C44 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 2.7" Tricolor Featherwing or Breakout with EK79686 chipset
// ThinkInk_270_Tricolor_Z70 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);


// 2.9" Tricolor Featherwing or Breakout with IL0373 chipset
// ThinkInk_290_Tricolor_Z10 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 2.9" Tricolor Featherwing or Breakout with UC8151D chipset
// ThinkInk_290_Tricolor_Z13 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 2.9" Tricolor Featherwing or Breakout with SSD1680 chipset and negative
// offset
// ThinkInk_290_Tricolor_Z94 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// 3.7" Tricolor Display with 420x240 pixels and UC8253 chipset
// ThinkInk_370_Tricolor_BABMFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI);

// ThinkInk_420_Tricolor_RW display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI); 

// ThinkInk_420_Tricolor_Z21 display(EPD_DC, EPD_RESET,
// EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

// 4.2" Tricolor EPD with SSD1683 chipset
// ThinkInk_420_Tricolor_MFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, 
//                                     EPD_BUSY, EPD_SPI);

// 5.83 Tricolor displays with 648x480 pixels and UC8179 chipset
// ThinkInk_583_Tricolor_AABMFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

// 7.5" Tricolor displays with 800x480 pixels and UC8179 chipset
// ThinkInk_750_Tricolor_AABMFGNR display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);


void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Adafruit EPD full update test in red/black/white");
  display.begin(THINKINK_TRICOLOR);
}

void loop() {
  Serial.println("Banner demo");
  display.clearBuffer();
  display.setTextSize(3);
  display.setCursor((display.width() - 144) / 2, (display.height() - 24) / 2);
  display.setTextColor(EPD_BLACK);
  display.print("Tri");
  display.setTextColor(EPD_RED);
  display.print("Color");
  display.display();

  delay(15000);

  Serial.println("Color rectangle demo");
  display.clearBuffer();
  display.fillRect(display.width() / 3, 0, display.width() / 3,
                   display.height(), EPD_BLACK);
  display.fillRect((display.width() * 2) / 3, 0, display.width() / 3,
                   display.height(), EPD_RED);
  display.display();

  delay(15000);

  Serial.println("Text demo");
  // large block of text
  display.clearBuffer();
  display.setTextSize(1);
  testdrawtext(
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur "
      "adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, "
      "fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor "
      "neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet "
      "ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a "
      "tortor imperdiet posuere. ",
      EPD_BLACK);
  display.display();

  delay(15000);

  display.clearBuffer();
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, EPD_BLACK);
  }

  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i, EPD_RED);
  }
  display.display();

  delay(15000);
}

void testdrawtext(const char *text, uint16_t color) {
  display.setCursor(0, 0);
  display.setTextColor(color);
  display.setTextWrap(true);
  display.print(text);
}
