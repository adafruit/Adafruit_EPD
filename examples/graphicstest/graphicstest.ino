/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_EPD.h"

#define EPD_CS     10
#define EPD_DC      9
#define SRAM_CS     8
#define EPD_RESET   5 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    3 // can set to -1 to not use a pin (will wait a fixed delay)

/* Uncomment the following line if you are using 1.54" tricolor EPD */
//Adafruit_IL0373 epd(152, 152 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
Adafruit_IL0373 epd(212, 104 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
//#define FLEXIBLE_213

/* Uncomment the following line if you are using 2.7" tricolor or gray EPD */
//Adafruit_IL91874 epd(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);

#if defined(FLEXIBLE_213)
  #define COLOR1 EPD_RED
  #define COLOR2 EPD_RED
#else // any other tricolor
  #define COLOR1 EPD_BLACK
  #define COLOR2 EPD_RED
#endif


float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! EPD Test");

  epd.begin();
  
#if defined(FLEXIBLE_213)
  epd.invertColorLogic(0, false);
  epd.invertColorLogic(1, false);
#endif

  Serial.println("Initialized");

  epd.clearBuffer();
  epd.fillRect(epd.width()/3, 0, epd.width()/3, epd.height(), COLOR2);
  epd.fillRect((epd.width()*2)/3, 0, epd.width()/3, epd.height(), COLOR1);
  epd.display();

  delay(15 * 1000);

  // large block of text
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", COLOR1);

  delay(15 * 1000);

  // epd print function!
  epdPrintTest();

  delay(15 * 1000);

  // a single pixel
  epd.clearBuffer();
  epd.drawPixel(epd.width()/2, epd.height()/2, COLOR1);

  delay(15 * 1000);

  testtriangles();

  delay(15 * 1000);
  
  // line draw test
  testlines(COLOR1);

  delay(15 * 1000);

  // optimized lines
  testfastlines(COLOR1, COLOR2);

  delay(15 * 1000);

  testdrawrects(COLOR2);

  delay(15 * 1000);

  testfillrects(COLOR1, COLOR2);

  delay(15 * 1000);

  epd.fillScreen(EPD_WHITE);
  testfillcircles(10, COLOR2);
  testdrawcircles(10, COLOR1);

  delay(15 * 1000);

  testroundrects();

  delay(15 * 1000);

  mediabuttons();

  Serial.println("done");
}

void loop() {
  delay(500);
}

void testlines(uint16_t color) {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < epd.width(); x+=6) {
    epd.drawLine(0, 0, x, epd.height()-1, color);
  }
  for (int16_t y=0; y < epd.height(); y+=6) {
    epd.drawLine(0, 0, epd.width()-1, y, color);
  }

  epd.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < epd.width(); x+=6) {
    epd.drawLine(epd.width()-1, 0, x, epd.height()-1, color);
  }
  for (int16_t y=0; y < epd.height(); y+=6) {
    epd.drawLine(epd.width()-1, 0, 0, y, color);
  }

  epd.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < epd.width(); x+=6) {
    epd.drawLine(0, epd.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < epd.height(); y+=6) {
    epd.drawLine(0, epd.height()-1, epd.width()-1, y, color);
  }

  epd.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < epd.width(); x+=6) {
    epd.drawLine(epd.width()-1, epd.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < epd.height(); y+=6) {
    epd.drawLine(epd.width()-1, epd.height()-1, 0, y, color);
  }
  epd.display();
}

void testdrawtext(char *text, uint16_t color) {
  epd.clearBuffer();
  epd.setCursor(5, 5);
  epd.setTextColor(color);
  epd.setTextWrap(true);
  epd.print(text);
  epd.display();
}

void testfastlines(uint16_t color1, uint16_t color2) {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  for (int16_t y=0; y < epd.height(); y+=5) {
    epd.drawFastHLine(0, y, epd.width(), color1);
  }
  for (int16_t x=0; x < epd.width(); x+=5) {
    epd.drawFastVLine(x, 0, epd.height(), color2);
  }
  epd.display();
}

void testdrawrects(uint16_t color) {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < epd.width(); x+=6) {
    epd.drawRect(epd.width()/2 -x/2, epd.height()/2 -x/2 , x, x, color);
  }
  epd.display();
}

void testfillrects(uint16_t color1, uint16_t color2) {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  for (int16_t x=epd.width()-1; x > 6; x-=6) {
    epd.fillRect(epd.width()/2 -x/2, epd.height()/2 -x/2 , x, x, color1);
    epd.drawRect(epd.width()/2 -x/2, epd.height()/2 -x/2 , x, x, color2);
  }
  epd.display();
}

void testfillcircles(uint8_t radius, uint16_t color) {
  epd.clearBuffer();
  for (int16_t x=radius; x < epd.width(); x+=radius*2) {
    for (int16_t y=radius; y < epd.height(); y+=radius*2) {
      epd.fillCircle(x, y, radius, color);
    }
  }
  epd.display();
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  epd.clearBuffer();
  for (int16_t x=0; x < epd.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < epd.height()+radius; y+=radius*2) {
      epd.drawCircle(x, y, radius, color);
    }
  }
  epd.display();
}

void testtriangles() {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  int color = COLOR1;
  int t;
  int w = epd.width()/2;
  int x = epd.height()-1;
  int y = 0;
  int z = epd.width();
  for(t = 0 ; t <= 15; t++) {
    epd.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    if(t == 8) color = COLOR2;
  }
  epd.display();
}

void testroundrects() {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  int color = COLOR1;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = epd.width()-2;
    int h = epd.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      epd.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      if(i == 7) color = COLOR2;
    }
    color = COLOR1;
  }
  epd.display();
}

void epdPrintTest() {
  epd.clearBuffer();
  epd.setCursor(2, 0);
  epd.fillScreen(EPD_WHITE);
  epd.setTextColor(COLOR1);
  epd.setTextSize(2);
  epd.println("Hello World!");
  epd.setTextSize(1);
  epd.setTextColor(COLOR2);
  epd.print(p, 6);
  epd.println(" Want pi?");
  epd.println(" ");
  epd.print(8675309, HEX); // print 8,675,309 out in HEX!
  epd.println(" Print HEX!");
  epd.println(" ");
  epd.setTextColor(COLOR1);
  epd.println("Sketch has been");
  epd.println("running for: ");
  epd.setTextColor(COLOR2);
  epd.print(millis() / 1000);
  epd.setTextColor(COLOR1);
  epd.print(" seconds.");
  epd.display();
}

void mediabuttons() {
  epd.clearBuffer();
  // play
  epd.fillScreen(EPD_WHITE);
  epd.fillRoundRect(25, 10, 78, 60, 8, COLOR1);
  epd.fillTriangle(42, 20, 42, 60, 90, 40, COLOR2);
  // pause
  epd.fillRoundRect(25, 90, 78, 60, 8, COLOR1);
  epd.fillRoundRect(39, 98, 20, 45, 5, COLOR2);
  epd.fillRoundRect(69, 98, 20, 45, 5, COLOR2);
  epd.display();
}
