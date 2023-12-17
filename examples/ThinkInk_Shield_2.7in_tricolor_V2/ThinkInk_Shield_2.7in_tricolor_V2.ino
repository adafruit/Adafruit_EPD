/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"

#define EPD_CS 10
#define EPD_DC 9
#define SRAM_CS 8
#define EPD_BUSY -1
#define EPD_RESET -1
#define EPD_SPI &SPI // primary SPI

// 2.7" Tricolor Featherwing or Breakout with EK79686 chipset (v2 Black Shield)
ThinkInk_270_Tricolor_Z70 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
EPD_BUSY, EPD_SPI);

#include <Adafruit_XCA9554.h>
Adafruit_XCA9554 expander;
#define XCA_EPD_RESET 5
#define XCA_EPD_CARDDET 6
#define XCA_EPD_BUSY 7

void expanderEPDReset(void) {
  expander.pinMode(XCA_EPD_RESET, OUTPUT);
  expander.digitalWrite(XCA_EPD_RESET, HIGH); // start high for sure
  delay(10);
  expander.digitalWrite(XCA_EPD_RESET, LOW);  // enter reset
  delay(10);
  expander.digitalWrite(XCA_EPD_RESET, HIGH); // bring out of reset
  delay(10);
}

bool expanderEPDBusy(void) {
  expander.pinMode(XCA_EPD_BUSY, INPUT);
  return expander.digitalRead(XCA_EPD_BUSY);
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }
  Serial.println("Adafruit ThinkInk Shield test in red/black/white");

  // Begin communication with the expander
  if (!expander.begin(0x20)) { // Replace with actual I2C address if different
    Serial.println("Failed to find XCA9554 chip");
    while (1);
  }
  display.setCustomBusyFunction(expanderEPDBusy);
  display.setCustomResetFunction(expanderEPDReset);
  display.begin(THINKINK_TRICOLOR);

  display.clearBuffer();
  display.setTextSize(2);
  display.setCursor(10, (display.height() - 24) / 2);
  display.setTextColor(EPD_BLACK);
  display.print("Press the ");
  display.setTextColor(EPD_RED);
  display.print("Buttons!");
  display.display();

  // set the 4 buttons to inputs via the expander
  expander.pinMode(0, INPUT);
  expander.pinMode(1, INPUT);
  expander.pinMode(2, INPUT);
  expander.pinMode(3, INPUT);
}

void loop() {
  if (! expander.digitalRead(0)) {
    Serial.println("Button A pressed");
    Serial.println("Banner demo");
    display.clearBuffer();
    display.setTextSize(3);
    display.setCursor((display.width() - 144) / 2, (display.height() - 24) / 2);
    display.setTextColor(EPD_BLACK);
    display.print("Tri");
    display.setTextColor(EPD_RED);
    display.print("Color");
    display.display();
  }

  if (! expander.digitalRead(1)) {
    Serial.println("Button B pressed");
    Serial.println("Color rectangle demo");
    display.clearBuffer();
    display.fillRect(display.width() / 3, 0, display.width() / 3,
                     display.height(), EPD_BLACK);
    display.fillRect((display.width() * 2) / 3, 0, display.width() / 3,
                     display.height(), EPD_RED);
    display.display();
  }

  if (! expander.digitalRead(2)) {
    Serial.println("Button C pressed");
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
  }

  if (! expander.digitalRead(3)) {
    Serial.println("Button D pressed");
    display.clearBuffer();
    for (int16_t i = 0; i < display.width(); i += 4) {
      display.drawLine(0, 0, i, display.height() - 1, EPD_BLACK);
    }
  
    for (int16_t i = 0; i < display.height(); i += 4) {
      display.drawLine(display.width() - 1, 0, 0, i, EPD_RED);
    }
    display.display();
  }
}

void testdrawtext(const char *text, uint16_t color) {
  display.setCursor(0, 0);
  display.setTextColor(color);
  display.setTextWrap(true);
  display.print(text);
}
