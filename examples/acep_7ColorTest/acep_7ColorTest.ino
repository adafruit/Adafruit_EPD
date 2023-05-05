/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_EPD.h"
#include <Adafruit_SPIFlash.h>
#include <SPI.h>
#include <SdFat.h>

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_THINKINK // detects if compiling for
                                                // Feather RP2040 ThinkInk
#define EPD_DC PIN_EPD_DC       // ThinkInk 24-pin connector DC
#define EPD_CS PIN_EPD_CS       // ThinkInk 24-pin connector CS
#define EPD_BUSY PIN_EPD_BUSY   // ThinkInk 24-pin connector Busy
#define SRAM_CS -1              // use onboard RAM
#define EPD_RESET PIN_EPD_RESET // ThinkInk 24-pin connector Reset
#define EPD_SPI &SPI1           // secondary SPI for ThinkInk
#else
#define EPD_CS 9
#define EPD_DC 10
#define SRAM_CS -1   // Use the build in memory, we need 133KB!
#define EPD_RESET 6  // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY 5   // can set to -1 to not use a pin (will wait a fixed delay)
#define EPD_SPI &SPI // primary SPI
#endif

// Uncomment the following line if you are using ACeP 7 color 5.65"
Adafruit_ACEP display(600, 448, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY,
                      EPD_SPI);

Adafruit_FlashTransport_QSPI flashTransport;
Adafruit_SPIFlash flash(&flashTransport);
FatFileSystem fatfs;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit ACeP EPD test");

  if (!flash.begin()) {
    Serial.println("Error, failed to initialize flash chip!");
    while (1)
      delay(1);
  }
  Serial.print("Flash chip JEDEC ID: 0x");
  Serial.println(flash.getJEDECID(), HEX);
  // First call begin to mount the filesystem.  Check that it returns true
  // to make sure the filesystem was mounted.
  if (!fatfs.begin(&flash)) {
    Serial.println("Error, failed to mount newly formatted filesystem!");
    Serial.println(
        "Was the flash chip formatted with the fatfs_format example?");
    while (1)
      delay(1);
  }
  Serial.println("Mounted filesystem!");
  File root = fatfs.open("/");
  printDirectory(root, 0);

  display.begin();
  display.setRotation(0);
  display.clearBuffer();
  // draw 7 color bars
  for (uint8_t color = ACEP_COLOR_BLACK; color <= ACEP_COLOR_ORANGE; color++) {
    display.fillRect(display.width() * color / 7, 0, display.width() / 7,
                     display.height(), color);
  }
  display.display();
}

void loop() {
  bmpDraw("adafruit_characters.bmp", 0, 0);
  bmpDraw("adabot.bmp", 0, 0);
}

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

bool bmpDraw(const char *filename, int16_t x, int16_t y) {
  File bmpFile;
  int bmpWidth, bmpHeight;            // W+H in pixels
  uint8_t bmpDepth;                   // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;            // Start of image data in file
  uint32_t rowSize;                   // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];    // pixel buffer (R+G+B per pixel)
  uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean goodBmp = false;            // Set to true on valid header parse
  boolean flip = true;                // BMP is stored bottom-to-top
  int w, h, row, col, x2, y2, bx1, by1;
  uint8_t r, g, b;
  uint32_t pos = 0, startTime = millis();

  if ((x >= display.width()) || (y >= display.height()))
    return false;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = fatfs.open(filename, FILE_READ)) == false) {
    Serial.print(F("File not found"));
    return false;
  }

  // Parse BMP header
  if (read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: "));
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile);            // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: "));
    Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: "));
    Serial.println(read32(bmpFile));
    bmpWidth = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if (read16(bmpFile) == 1) {   // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: "));
      Serial.println(bmpDepth);
      if ((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if (bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip = false;
        }

        // Crop area to be loaded
        x2 = x + bmpWidth - 1; // Lower-right corner
        y2 = y + bmpHeight - 1;
        if ((x2 >= 0) && (y2 >= 0)) { // On screen?
          w = bmpWidth;               // Width/height of section to load/display
          h = bmpHeight;
          bx1 = by1 = 0; // UL coordinate in BMP file

          for (row = 0; row < h; row++) { // For each scanline...

            // Seek to start of scan line.  It might seem labor-
            // intensive to be doing this on every line, but this
            // method covers a lot of gritty details like cropping
            // and scanline padding.  Also, the seek only takes
            // place if the file position actually needs to change
            // (avoids a lot of cluster math in SD library).
            if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
              pos = bmpImageoffset + (bmpHeight - 1 - (row + by1)) * rowSize;
            else // Bitmap is stored top-to-bottom
              pos = bmpImageoffset + (row + by1) * rowSize;
            pos += bx1 * 3;                  // Factor in starting column (bx1)
            if (bmpFile.position() != pos) { // Need seek?
              bmpFile.seek(pos);
              buffidx = sizeof(sdbuffer); // Force buffer reload
            }
            for (col = 0; col < w; col++) { // For each pixel...
              // Time to read more pixel data?
              if (buffidx >= sizeof(sdbuffer)) { // Indeed
                bmpFile.read(sdbuffer, sizeof(sdbuffer));
                buffidx = 0; // Set index to beginning
              }
              // Convert pixel from BMP to EPD format, push to display
              b = sdbuffer[buffidx++];
              g = sdbuffer[buffidx++];
              r = sdbuffer[buffidx++];
              uint32_t color = r;
              color <<= 8;
              color |= g;
              color <<= 8;
              color |= b;

              uint8_t c;
              switch (color) {
              case 0x000000:
                c = ACEP_COLOR_BLACK;
                break;
              case 0xFFFFFF:
                c = ACEP_COLOR_WHITE;
                break;
              case 0x00FF00:
                c = ACEP_COLOR_GREEN;
                break;
              case 0x0000FF:
                c = ACEP_COLOR_BLUE;
                break;
              case 0xFF0000:
                c = ACEP_COLOR_RED;
                break;
              case 0xFFFF00:
                c = ACEP_COLOR_YELLOW;
                break;
              case 0xFF8000:
                c = ACEP_COLOR_ORANGE;
                break;
              default: {
                Serial.print("Unknown color 0x");
                Serial.println(color, HEX);
                c = ACEP_COLOR_WHITE;
              }
              }
              display.writePixel(col, row, c);
            } // end pixel
          }   // end scanline
        }     // end onscreen
        display.display();
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if (!goodBmp) {
    Serial.println(F("BMP format not recognized."));
    return false;
  }
  return true;
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

void printDirectory(File dir, int numTabs) {
  char filename[80];

  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    entry.getName(filename, 80);
    Serial.print(filename);
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
