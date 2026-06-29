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

/* 4.26" Xteink X4 eReader (SSD1677)
   use these pins:
   #define EPD_DC 4
   #define EPD_CS 21
   #define EPD_BUSY 6
   #define SRAM_CS -1
   #define EPD_RESET 5
   #define EPD_SPI &SPI */
// ThinkInk_426_Grayscale4_GDEQ display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS
// EPD_BUSY, EPD_SPI);


// ThinkInk_154_Grayscale4_T8 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI); 


// 1.54" 200x200 4-gray, SSD1681 - breakout #4196 (panel GDEY0154D67, ribbon P154030-MF1-B)
// ThinkInk_154_Grayscale4_M05 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);


// 2.13" 212x104 Grayscale display with IL0373 chipset
// ThinkInk_213_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

// 2.13" 250x122 4-gray, SSD1680(Z), panel GDEY0213B74
//   FPC-A002: FeatherWing #4195 (post-2020 rev) / Pi bonnet #4687
//   FPC-7528B: breakout #4197 / bare #6383 (SSD1680Z)
ThinkInk_213_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                                     EPD_SPI);

// Same panel but with colstart=8: uncomment this version (and comment the one
// above) if you see an ~8px strip on the display (e.g. the FPC-7528B revision).
// ThinkInk_213_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
// EPD_BUSY, EPD_SPI, /*colstart=*/8);

// 2.66" 296x152 4-gray, SSD1680 - bare display #6392 (ribbon FPC-A003)
// ThinkInk_266_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
//                                      EPD_SPI);

// 2.9" Grayscale Featherwing or Breakout with IL0373
// ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
//                                    EPD_SPI);

// 2.9" 296x128 4-gray, SSD1680 - original MagTag #4800 early batch
//   (ribbon FPC-A005, panel GDEM029T94, colstart=8)
// ThinkInk_290_Grayscale4_EAAMFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

// 2.9" 296x128 4-gray, SSD1680 - MagTag #4800 / FeatherWing #4777
//   (ribbon FPC-7519rev.b, panel GDEM029T94; colstart=8 on MagTag, 0 on Wing)
// ThinkInk_290_Grayscale4_FPC7519 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

// 4.2" 400x300 4-gray, SSD1683 - bare display #6381
//   (panel GDEY042T81, ribbon FPC-190)
// ThinkInk_420_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, EPD_SPI);

#define COLOR1 EPD_BLACK
#define COLOR2 EPD_LIGHT
#define COLOR3 EPD_DARK

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Adafruit EPD full update test in mono & grayscale");
}

bool gray = false;
void loop() {

  // alternate modes!
  if (gray) {
    display.begin(THINKINK_GRAYSCALE4);
    Serial.println("Grayscale!");
  } else {
    display.begin(THINKINK_MONO);
    Serial.println("Monochrome!");
  }

  display.clearBuffer();
  display.setTextSize(3);
  display.setTextColor(EPD_BLACK);
  display.setCursor((display.width() - 180) / 2, (display.height() - 24) / 2);
  if (gray) {
    String text = "Grayscale";
    uint16_t colors[] = {EPD_BLACK, EPD_DARK, EPD_LIGHT};
  
    for (int i = 0; i < text.length(); i++) {
      // Change color for every character (0: BLACK, 1: DARK, 2: LIGHT, 3: BLACK, etc.)
      display.setTextColor(colors[i % 3]);
      display.print(text.charAt(i));
    }
  } else {
    display.print("Monochrome");
  }

  gray = !gray;

  display.display();
  delay(2000);

  display.clearBuffer();
  display.fillRect(display.width() / 4, 0, display.width() / 4,
                   display.height(), EPD_LIGHT);
  display.fillRect((display.width() * 2) / 4, 0, display.width() / 4,
                   display.height(), EPD_DARK);
  display.fillRect((display.width() * 3) / 4, 0, display.width() / 4,
                   display.height(), EPD_BLACK);
  display.display();
  delay(2000);

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
      COLOR1);
  display.display();
  delay(2000);

  display.clearBuffer();
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawLine(0, 0, i, display.height() - 1, COLOR1);
  }

  for (int16_t i = 0; i < display.height(); i += 4) {
    display.drawLine(display.width() - 1, 0, 0, i,
                     COLOR2); // on grayscale this will be mid-gray
  }
  display.display();
  delay(2000);
}

void testdrawtext(const char *text, uint16_t color) {
  display.setCursor(0, 0);
  display.setTextColor(color);
  display.setTextWrap(true);
  display.print(text);
}
