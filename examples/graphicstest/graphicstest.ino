/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include "Adafruit_EPD.h"

#define EPD_DC     9
#define EPD_CS     10
#define EPD_RESET  3
#define EPD_BUSY   5
#define SRAM_CS     8

/* Uncomment the following line if you are using 1.54" tricolor EPD */
Adafruit_IL0373 epd(152, 152 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.15" tricolor EPD */
//Adafruit_IL0373 epd(212, 104 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

/* Uncomment the following line if you are using 2.7" tricolor EPD */
//Adafruit_IL91874 epd(264, 176 ,EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);


float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! EPD Test");

  epd.begin();

  Serial.println("Initialized");

  // large block of text
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", EPD_BLACK);

  // epd print function!
  epdPrintTest();

  // a single pixel
  epd.clearBuffer();
  epd.drawPixel(epd.width()/2, epd.height()/2, EPD_BLACK);

  testtriangles();
  
  // line draw test
  testlines(EPD_BLACK);

  // optimized lines
  testfastlines(EPD_BLACK, EPD_RED);

  testdrawrects(EPD_RED);

  testfillrects(EPD_BLACK, EPD_RED);

  epd.fillScreen(EPD_WHITE);
  testfillcircles(10, EPD_RED);
  testdrawcircles(10, EPD_BLACK);

  testroundrects();

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
  epd.setCursor(0, 0);
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
  int color = EPD_BLACK;
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
    if(t == 8) color = EPD_RED;
  }
  epd.display();
}

void testroundrects() {
  epd.clearBuffer();
  epd.fillScreen(EPD_WHITE);
  int color = EPD_BLACK;
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
      if(i == 7) color = EPD_RED;
    }
    color = EPD_BLACK;
  }
  epd.display();
}

void epdPrintTest() {
  epd.clearBuffer();
  epd.setCursor(2, 0);
  epd.fillScreen(EPD_WHITE);
  epd.setTextColor(EPD_BLACK);
  epd.setTextSize(2);
  epd.println("Hello World!");
  epd.setTextSize(1);
  epd.setTextColor(EPD_RED);
  epd.print(p, 6);
  epd.println(" Want pi?");
  epd.println(" ");
  epd.print(8675309, HEX); // print 8,675,309 out in HEX!
  epd.println(" Print HEX!");
  epd.println(" ");
  epd.setTextColor(EPD_BLACK);
  epd.println("Sketch has been");
  epd.println("running for: ");
  epd.setTextColor(EPD_RED);
  epd.print(millis() / 1000);
  epd.setTextColor(EPD_BLACK);
  epd.print(" seconds.");
  epd.display();
}

void mediabuttons() {
  epd.clearBuffer();
  // play
  epd.fillScreen(EPD_WHITE);
  epd.fillRoundRect(25, 10, 78, 60, 8, EPD_BLACK);
  epd.fillTriangle(42, 20, 42, 60, 90, 40, EPD_RED);
  // pause
  epd.fillRoundRect(25, 90, 78, 60, 8, EPD_BLACK);
  epd.fillRoundRect(39, 98, 20, 45, 5, EPD_RED);
  epd.fillRoundRect(69, 98, 20, 45, 5, EPD_RED);
  epd.display();
}