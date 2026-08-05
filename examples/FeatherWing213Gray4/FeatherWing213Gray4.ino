/***************************************************
  4-level grayscale test for the 2.13" 250x122 eInk FeatherWing (SSD1680,
  GDEY0213B74 / FPC-A002). Companion to FeatherWingTest.ino (mono/tri-color).

  Self-contained: the shared ThinkInk_gray4.ino doesn't run on this wing as-is
  (its SRAM_CS default targets a different host and its while(!Serial) startup
  blocks a native-USB Feather), so the wing-correct pins and a non-blocking
  start live here.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"

// --- FeatherWing pins (per FeatherWingTest.ino + ESP32 variants from the ILI9341
//     graphicstest_featherwing example; the wing pinout is fixed per host board) ---
#if defined(ESP8266)
  #define SRAM_CS 16
  #define EPD_CS   0
  #define EPD_DC  15
#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32C6)
  #define SRAM_CS -1   // grayscale buffer fits in MCU RAM (no external SRAM needed)
  #define EPD_CS   7
  #define EPD_DC   8
#elif defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2)         || \
      defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_REVTFT)  || \
      defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2_TFT)     || \
      defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)         || \
      defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_NOPSRAM) || \
      defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_REVTFT)  || \
      defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_TFT)
  #define SRAM_CS -1   // grayscale buffer fits in MCU RAM (no external SRAM needed)
  #define EPD_CS   9
  #define EPD_DC  10
#elif defined(ESP32)   // classic HUZZAH32 V1
  #define SRAM_CS 32
  #define EPD_CS  15
  #define EPD_DC  33
#elif defined(TEENSYDUINO)
  #define SRAM_CS  3
  #define EPD_CS   4
  #define EPD_DC  10
#elif defined(ARDUINO_NRF52832_FEATHER)
  #define SRAM_CS 30
  #define EPD_CS  31
  #define EPD_DC  11
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
  #define SRAM_CS -1   // grayscale buffer fits in MCU RAM on the RP2040 (no external SRAM)
  #define EPD_CS   9
  #define EPD_DC  10
#else
  // AVR/M0/M4/nRF52840 Feathers (external SRAM) and the CI fallback. Adjust for your wiring.
  #define SRAM_CS  6
  #define EPD_CS   9
  #define EPD_DC  10
#endif

// On the eInk FeatherWing, RST is tied to the Feather RESET line and BUSY is not
// broken out -> use -1 for both. RESET=-1 is essential: the SSD1680 driver only
// issues a deep-sleep power-down when it owns a reset pin, and a deep-sleep panel
// on the FeatherWing can ONLY be woken by a full Feather reset. Leaving RESET=-1
// keeps power-down a no-op so the panel stays drivable (refresh waits a fixed delay).
#define EPD_RESET -1
#define EPD_BUSY  -1

// 2.13" 250x122 SSD1680 Grayscale FeatherWing (GDEY0213B74 / FPC-A002)
ThinkInk_213_Grayscale4_MFGN display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

void setup() {
  Serial.begin(115200);
  // Don't block on Serial: native-USB Feathers hang at while(!Serial) until a
  // serial monitor opens the port. A short delay lets the sketch run standalone.
  delay(2000);
  Serial.println("eInk 2.13\" FeatherWing 4-gray test (#4195)");

  display.begin(THINKINK_GRAYSCALE4);

  // If your 4-gray comes out inverted (black<->white), uncomment to flip the
  // grayscale buffers. The GDEY0213B74 / FPC-A002 renders correctly with the
  // library defaults, so this is left off.
  // display.setColorBuffer(1, true);
  // display.setBlackBuffer(0, true);

  // rotation=0 is upright landscape (250 wide x 122 tall) on this wing.
  display.setRotation(0);

  display.clearBuffer();
  display.fillScreen(EPD_WHITE);

  display.setTextSize(2);
  display.setTextColor(EPD_BLACK);
  display.setCursor(6, 6);  display.print("Adafruit ThinkInk");
  display.setCursor(6, 28); display.print("2.13\" 250x122");
  display.setCursor(6, 50); display.print("4-Gray E-Ink");
  display.setTextSize(1);
  display.setTextColor(EPD_DARK);   // mid-gray text confirms 4-gray
  display.setCursor(6, 74); display.print("SSD1680 #4195");

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
