/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_EPD.h"
#include <SD.h>

#define EPD_CS     10
#define EPD_DC      9
#define SRAM_CS     8
#define EPD_RESET  -1 // shared with microcontroller Reset!
#define SD_CS       4


/* Uncomment the following line if you are using 2.7" tricolor or grayscale EPD */
Adafruit_IL91874 display(264, 176, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS);

float p = 3.1415926;

int8_t readButtons(void) {
  uint16_t reading = analogRead(A3);
  //Serial.println(reading);

  if (reading > 600) {
    return 0; // no buttons pressed
  }
  if (reading > 400) {
    return 4; // button D pressed
  }
  if (reading > 250) {
    return 3; // button C pressed
  }
  if (reading > 125) {
    return 2; // button B pressed
  }
  return 1; // Button A pressed
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);

  delay(100);
  
  Serial.println(F("Hello! Adafruit EInk Shield Test"));

  display.begin();
  display.setRotation(2);

  display.clearBuffer();
  display.setCursor(5, 5);
  display.setTextColor(EPD_BLACK);
  display.setTextSize(2);
  display.setTextWrap(true);
  display.println(F("Adafruit\n2.7\"EInk Shield Test"));

  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("SD Card Init failed!"));
    display.setTextColor(EPD_RED);
    display.println(F("SD card not found"));
  } else {
    Serial.println(F("OK!"));
    display.println(F("SD card found"));
  }
  Serial.print(F("Drawing..."));
  display.display();
  Serial.println(F("Ready!"));
}

void loop() {
  int button = readButtons();
  if (button == 0) {
    return;
  }
  Serial.print(F("Button ")); Serial.print(button); Serial.println(F(" pressed"));

  if (button == 1) {
    Serial.print(F("Triangle Test..."));
    testtriangles();
    Serial.println(F("Done"));
  }

  if (button == 2) {
    Serial.print(F("Lines Test..."));
    testlines(EPD_BLACK, EPD_RED);   
    Serial.println(F("Done"));
  }

  if (button == 3) {
    Serial.print(F("Text Test..."));
    epdPrintTest();
    Serial.println("Done");
  }

  if (button == 4) {
    Serial.print(F("Bitmap Test..."));
    display.clearBuffer();
    bmpDraw("/blinka27.bmp",0,0);
    Serial.println(F("Done"));
  }

  // wait until button is released
  while (readButtons()) {
    delay(10);
  }
}

void testlines(uint16_t color1, uint16_t color2) {
  display.clearBuffer();
  for (int16_t i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, color1);
  }

  for (int16_t i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, color2);  // on grayscale this will be mid-gray
  }
  display.display();
}

void testdrawtext(char *text, uint16_t color) {
  display.clearBuffer();
  display.setCursor(5, 5);
  display.setTextColor(color);
  display.setTextWrap(true);
  display.print(text);
  display.display();
}

void testtriangles() {
  display.clearBuffer();
  int color = EPD_BLACK;
  int t;
  int w = display.width()/2;
  int x = display.height()-1;
  int y = 0;
  int z = display.width();
  for(t = 0 ; t <= 15; t++) {
    display.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    if(t == 8) color = EPD_RED;
  }
  display.display();
}

void epdPrintTest() {
  display.clearBuffer();
  display.setCursor(5, 5);
  display.fillScreen(EPD_WHITE);
  display.setTextColor(EPD_BLACK);
  display.setTextSize(2);
  display.println("Hello World!");
  display.setTextSize(1);
  display.setTextColor(EPD_RED);
  display.print(p, 6);
  display.println(" Want pi?");
  display.println(" ");
  display.print(8675309, HEX); // print 8,675,309 out in HEX!
  display.println(" Print HEX!");
  display.println(" ");
  display.setTextColor(EPD_BLACK);
  display.println("Sketch has been");
  display.println("running for: ");
  display.setTextColor(EPD_RED);
  display.print(millis() / 1000);
  display.setTextColor(EPD_BLACK);
  display.print(" seconds.");
  display.display();
}


// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void bmpDraw(char *filename, int16_t x, int16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col, x2, y2, bx1, by1;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= display.width()) || (y >= display.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == (int)NULL) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        x2 = x + bmpWidth  - 1; // Lower-right corner
        y2 = y + bmpHeight - 1;
        if((x2 >= 0) && (y2 >= 0)) { // On screen?
          w = bmpWidth; // Width/height of section to load/display
          h = bmpHeight;
          bx1 = by1 = 0; // UL coordinate in BMP file
  
          for (row=0; row<h; row++) { // For each scanline...
  
            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
            else     // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + (row + by1) * rowSize;
            pos += bx1 * 3; // Factor in starting column (bx1)
            if(bmpFile.position() != pos) { // Need seek?
              bmpFile.seek(pos);
              buffidx = sizeof(sdbuffer); // Force buffer reload
            }
            for (col=0; col<w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sizeof(sdbuffer)) { // Indeed
                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                buffidx = 0; // Set index to beginning
              }
              // Convert pixel from BMP to EPD format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];

              uint8_t c = 0;
              if ((r < 0x80) && (g < 0x80) && (b < 0x80)) {
                 c = EPD_BLACK; // try to infer black
              } else if ((r >= 0x80) && (g >= 0x80) && (b >= 0x80)) {
                 c = EPD_WHITE;
              } else if (r >= 0x80) {
                c = EPD_RED; //try to infer red color
              }
              
              display.writePixel(col, row, c);
            } // end pixel
          } // end scanline
        } // end onscreen
        display.display();
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}