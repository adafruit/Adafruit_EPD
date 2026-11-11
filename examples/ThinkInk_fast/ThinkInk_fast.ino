// SPDX-FileCopyrightText: 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// Fast e-paper animation with ordinary Adafruit_GFX drawing.
//
// displayFast() sends only the changed area and drives each change for a few
// short updates, the same amount toward black as toward white, so the panel
// stays DC balanced. fastClean() wipes away the gray build-up now and then.
//
// Written for the Adafruit MagTag 2.9" (SSD1680). Mono mode only.

#include "Adafruit_ThinkInk.h"

#ifndef ARDUINO_MAGTAG29_ESP32S2 // MagTag has these pins in its variant
#define EPD_DC 10
#define EPD_CS 9
#define EPD_BUSY 7 // fast mode needs the BUSY pin
#define EPD_RESET 8
#endif

// Pick the panel on your MagTag. The ribbon label is printed on the panel:
// FPC-A005 -> EAAMFGN, FPC-7519 rev.b -> FPC7519.
#ifdef MAGTAG_FPC7519
ThinkInk_290_Grayscale4_FPC7519 display(EPD_DC, EPD_RESET, EPD_CS, -1, EPD_BUSY,
                                        &SPI);
#else
ThinkInk_290_Grayscale4_EAAMFGN display(EPD_DC, EPD_RESET, EPD_CS, -1, EPD_BUSY,
                                        &SPI);
#endif

#define WIPE_MS 4000UL   // balanced wipe this often
#define CLEAN_MS 30000UL // full refresh this often
#define WIPE_FRAMES 8    // frames each wipe spans
#define BALL_R 12        // ball radius in pixels
#define TEXT_H 18        // fps text at the top

float ball_x = 40, ball_y = 40, ball_dx = 2.3, ball_dy = 1.7;
float angle = 0;
uint32_t last_wipe, last_clean, fps_t0;
uint16_t fps_frames;
float fps = 0;

// Cube corners and edges for a small wireframe
const int8_t cube[8][3] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
                           {-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};
const uint8_t edges[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
                              {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

void drawCube(int16_t cx, int16_t cy, float size, float a) {
  int16_t px[8], py[8];
  float ca = cos(a), sa = sin(a), cb = cos(a * 0.7), sb = sin(a * 0.7);
  for (int i = 0; i < 8; i++) {
    float x = cube[i][0], y = cube[i][1], z = cube[i][2];
    float x1 = x * ca - z * sa, z1 = x * sa + z * ca;
    float y1 = y * cb - z1 * sb, z2 = y * sb + z1 * cb;
    float s = size / (z2 + 4) * 4;
    px[i] = cx + x1 * s;
    py[i] = cy + y1 * s;
  }
  for (int e = 0; e < 12; e++) {
    display.drawLine(px[edges[e][0]], py[edges[e][0]], px[edges[e][1]],
                     py[edges[e][1]], EPD_BLACK);
  }
}

void setup() {
  Serial.begin(115200);
  display.begin(THINKINK_MONO);
  if (!display.fastBegin()) {
    Serial.println("fast mode not supported on this display");
    while (1) {
      delay(10);
    }
  }
  last_wipe = last_clean = fps_t0 = millis();
}

void loop() {
  uint32_t now = millis();
  if (now - last_clean >= CLEAN_MS) {
    display.fastClean(WIPE_FRAMES, true);
    last_clean = last_wipe = millis();
  } else if (now - last_wipe >= WIPE_MS) {
    display.fastClean(WIPE_FRAMES);
    last_wipe = millis();
  }

  display.clearBuffer();
  display.fillCircle(ball_x, ball_y, BALL_R, EPD_BLACK);
  drawCube(220, 64, 26, angle);
  display.setTextColor(EPD_BLACK);
  display.setTextSize(2);
  display.setCursor(4, 4);
  display.print(fps, 1);
  display.print(" fps");

  // The panel updates while we draw the next frame
  if (display.displayFast()) {
    fps_frames++;
  }

  ball_x += ball_dx;
  ball_y += ball_dy;
  if (ball_x < BALL_R || ball_x > display.width() - BALL_R) {
    ball_dx = -ball_dx;
  }
  if (ball_y < TEXT_H + BALL_R || ball_y > display.height() - BALL_R) {
    ball_dy = -ball_dy;
  }
  angle += 0.06;

  if (now - fps_t0 >= 1000) {
    fps = fps_frames * 1000.0 / (now - fps_t0);
    Serial.printf("%.1f fps  busy %lu us  %lu bytes\n", fps,
                  display.fastBusyMicros(), display.fastBytes());
    fps_frames = 0;
    fps_t0 = now;
  }
}
