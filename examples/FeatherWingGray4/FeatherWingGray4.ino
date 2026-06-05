/***************************************************
  4-level grayscale test for the Adafruit eInk FeatherWings (SSD1680).
  Companion to FeatherWingTest.ino (mono/tri-color).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"

// --- FeatherWing pins (same convention as FeatherWingTest.ino) ---
#ifdef ESP8266
  #define SRAM_CS 16
  #define EPD_CS   0
  #define EPD_DC  15
#endif
#ifdef ESP32
  #define SRAM_CS 32
  #define EPD_CS  15
  #define EPD_DC  33
#endif
#if defined(__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(ARDUINO_FEATHER_M4) || defined(__AVR_ATmega328P__) || defined(ARDUINO_NRF52840_FEATHER)
  #define SRAM_CS 6
  #define EPD_CS  9
  #define EPD_DC  10
#endif
#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040
  #define SRAM_CS 8
  #define EPD_CS  9
  #define EPD_DC  10
#endif

// On the eInk FeatherWing, RST is tied to the Feather RESET line and BUSY is not
// broken out -> use -1 for both (panel resets with the Feather; refresh waits a fixed delay).
#define EPD_RESET -1
#define EPD_BUSY  -1

// --- Pick the grayscale FeatherWing you have (uncomment one) ---

// 2.13" 250x122 SSD1680 Grayscale FeatherWing
// ThinkInk_213_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

// 2.9" 296x128 SSD1680 Grayscale FeatherWing — NEWER panel (FPC-7519rev.b, User ID 0xca)
ThinkInk_290_Grayscale4_FPC7519 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

// 2.9" 296x128 SSD1680 Grayscale FeatherWing — OLDER panel (FPC-A005). Polarity differs by
// sub-revision, so use the matching class for your panel.
// ThinkInk_290_Grayscale4_EAAMFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

void setup() {
  Serial.begin(115200);
  Serial.println("eInk FeatherWing 4-gray test");

  display.begin(THINKINK_GRAYSCALE4);

  // Polarity is sub-revision-specific on the FPC-7519 panel. This FeatherWing ships the
  // newer 0xca sub-rev, which renders inverted with the library's default (un-inverted)
  // grayscale buffers, so flip both buffers here. (The driver has no User-ID readback to
  // auto-detect, so the flip lives in the sketch rather than the panel class.) If your
  // 4-gray comes out inverted, comment these two lines back out.
  display.setColorBuffer(1, true);
  display.setBlackBuffer(0, true);

  // The FeatherWing mounts the panel reversed vs the bare/MagTag modules, so the upright
  // rotation differs. rotation=0 is upright for the 2.9" #4777; adjust for your wing.
  display.setRotation(0);

  display.clearBuffer();
  display.fillScreen(EPD_WHITE);

  display.setTextSize(2);
  display.setTextColor(EPD_BLACK);
  display.setCursor(6, 6);  display.print("Adafruit ThinkInk");
  display.setCursor(6, 28); display.print("4-Gray FeatherWing");
  display.setTextColor(EPD_DARK);   // mid-gray text confirms 4-gray
  display.setCursor(6, 50); display.print("SSD1680 #4777"); // part# of the active panel above

  // 4-level ramp: black | dark | light | white, with a border so white reads distinctly
  int16_t W = display.width();
  int16_t seg = W / 4, top = display.height() - 28, h = 22;
  uint16_t ramp[4] = {EPD_BLACK, EPD_DARK, EPD_LIGHT, EPD_WHITE};
  for (int i = 0; i < 4; i++) {
    display.fillRect(i * seg, top, (i < 3 ? seg : W - i * seg), h, ramp[i]);
  }
  display.drawRect(0, top, W, h, EPD_BLACK);

  display.display();
  Serial.println("done");
}

void loop() {
  // nothing
}
