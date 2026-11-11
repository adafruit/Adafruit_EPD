/*!
 * @file Adafruit_EPD_fast.cpp
 *
 * Fast lag-drive updates for monochrome e-paper.
 *
 * Each displayFast() sends the new frame to the "new" RAM plane and the frame
 * from `lag` updates ago to the "old" plane, then runs a short balanced
 * waveform in display mode 2. Only pixels that differ are driven, and every
 * change is driven for `lag` updates, the same number of frames toward black
 * as toward white, so the drive stays DC balanced.
 *
 * Only the bounding box of changed bytes is written, and the BUSY wait for an
 * update happens at the start of the next call, so drawing the next frame
 * overlaps the panel update.
 *
 * MIT license, all text here must be included in any redistribution.
 */

#include "Adafruit_EPD.h"

#define FAST_BUSY_RISE_US 2000    ///< max wait for BUSY to rise, in us
#define FAST_BUSY_TIMEOUT_MS 5000 ///< max BUSY time for one update, in ms
#define FAST_CHUNK 64             ///< bytes per SPI transfer

// ESP32 SPI reads 32-bit words, so send from an aligned bounce buffer
static uint8_t fast_chunk[FAST_CHUNK] __attribute__((aligned(4)));

/**************************************************************************/
/*!
    @brief Start fast mode: clean the panel to white with a full refresh,
    load the fast waveform and allocate the lag history. Mono mode only, and
    a BUSY pin is required.
    @param lag number of updates each change is driven for, from 1 to 32
    @param spi_hz SPI clock in Hz used while fast mode is active. Ignored for
    software SPI.
    @returns true if fast mode started, false if this display or mode does not
    support it or memory ran out
*/
/**************************************************************************/
bool Adafruit_EPD::fastBegin(uint8_t lag, uint32_t spi_hz) {
  if (fastActive()) {
    fastEnd();
  }
  // Mono uses one buffer for black and color; color panels use two
  if (!fastSupported() || use_sram || _busy_pin < 0 || black_buffer == NULL ||
      color_buffer != black_buffer || !blackInverted || lag < 1 || lag > 32) {
    return false;
  }

  fastSetSPI(spi_hz);
  fastFullClean();
  uint32_t frame = fastFrameBytes();
  bool ok = fastModeEnter() && frame != 0 && frame <= buffer1_size;
  if (ok) {
    _fast_ring = (uint8_t*)malloc(frame * (lag + 1));
    ok = (_fast_ring != NULL);
  }
  if (!ok) {
    fastSetSPI(EPD_DEFAULT_SPI_HZ);
    powerDown();
    return false;
  }
  memset(_fast_ring, 0xFF, frame * (lag + 1));
  _fast_lag = lag;
  _fast_head = 0;
  _fast_prev_valid = false;
  _fast_pending = false;
  return true;
}

/**************************************************************************/
/*!
    @brief Send the current buffer to the panel as the next fast update.
    Returns right after the update starts; the next call waits for it.
    Call it again with an unchanged buffer to finish driving the last changes
    (or use fastSettle()).
    @returns true if an update was started, false if nothing was left to
    drive or fast mode is not active
*/
/**************************************************************************/
bool Adafruit_EPD::displayFast(void) {
  if (!fastActive()) {
    return false;
  }
  return fastStep(black_buffer);
}

/**************************************************************************/
/*!
    @brief Keep updating with the current buffer until every change has had
    its full drive, then wait for the panel.
*/
/**************************************************************************/
void Adafruit_EPD::fastSettle(void) {
  if (!fastActive()) {
    return;
  }
  while (fastStep(fastHeadFrame())) {
  }
  fastWaitIdle();
}

/**************************************************************************/
/*!
    @brief Finish all owed drive, free the lag history, restore the normal
    SPI clock and put the panel to sleep. The image stays on screen.
*/
/**************************************************************************/
void Adafruit_EPD::fastEnd(void) {
  if (!fastActive()) {
    return;
  }
  fastSettle();
  fastRelease();
  powerDown();
}

/**************************************************************************/
/*!
    @brief Leave fast mode without touching the panel: free the lag history
    and restore the normal SPI clock. Used when display() or begin() is
    called while fast mode is active.
*/
/**************************************************************************/
void Adafruit_EPD::fastRelease(void) {
  if (_fast_ring == NULL) {
    return;
  }
  fastWaitIdle();
  free(_fast_ring);
  _fast_ring = NULL;
  fastSetSPI(EPD_DEFAULT_SPI_HZ);
}

/**************************************************************************/
/*!
    @brief The newest frame in the lag history (what the panel is being
    driven toward)
    @returns pointer into the lag history
*/
/**************************************************************************/
const uint8_t* Adafruit_EPD::fastHeadFrame(void) {
  return _fast_ring + (uint32_t)_fast_head * fastFrameBytes();
}

/**************************************************************************/
/*!
    @brief Push one frame through the lag history and start an update for
    the changed area
    @param frame frame in panel RAM layout, NULL for all white
    @returns true if an update was started
*/
/**************************************************************************/
bool Adafruit_EPD::fastStep(const uint8_t* frame) {
  const uint16_t stride = _fast_stride, rows = _fast_rows;
  const uint32_t size = fastFrameBytes();
  const uint8_t slots = _fast_lag + 1;

  fastWaitIdle(); // RAM writes during BUSY are dropped

  _fast_head = (_fast_head + 1) % slots;
  uint8_t* cur = _fast_ring + (uint32_t)_fast_head * size;
  const uint8_t* old = _fast_ring + (uint32_t)((_fast_head + 1) % slots) * size;
  if (frame) {
    memcpy(cur, frame, size);
  } else {
    memset(cur, 0xFF, size);
  }

  // Bounding box of bytes that differ between frame N and frame N-lag
  int16_t box[4] = {(int16_t)stride, -1, (int16_t)rows, -1};
  for (uint16_t y = 0; y < rows; y++) {
    const uint8_t* a = cur + (uint32_t)y * stride;
    const uint8_t* b = old + (uint32_t)y * stride;
    if (memcmp(a, b, stride) == 0) {
      continue;
    }
    // memcmp found a difference, so both scans stop inside the row
    uint16_t first = 0;
    while (a[first] == b[first]) {
      first++;
    }
    uint16_t last = stride - 1;
    while (a[last] == b[last]) {
      last--;
    }
    box[0] = min(box[0], (int16_t)first);
    box[1] = max(box[1], (int16_t)last);
    if ((int16_t)y < box[2]) {
      box[2] = y;
    }
    box[3] = y;
  }

  // Union with the previous step's box so the two planes stay equal outside
  // the window; the first step after a reset writes everything.
  int16_t w[4];
  if (!_fast_prev_valid) {
    w[0] = 0;
    w[1] = stride - 1;
    w[2] = 0;
    w[3] = rows - 1;
  } else {
    memcpy(w, box, sizeof(w));
    if (_fast_prev[1] >= _fast_prev[0]) {
      if (w[1] < w[0]) {
        memcpy(w, _fast_prev, sizeof(w));
      } else {
        w[0] = min(w[0], _fast_prev[0]);
        w[1] = max(w[1], _fast_prev[1]);
        w[2] = min(w[2], _fast_prev[2]);
        w[3] = max(w[3], _fast_prev[3]);
      }
    }
  }
  memcpy(_fast_prev, box, sizeof(box));
  _fast_prev_valid = true;

  if (w[1] < w[0]) {
    _fast_bytes = 0;
    return false; // nothing owed
  }

  fastWriteBox(1, old, w); // old plane: frame N-lag
  fastWriteBox(0, cur, w); // new plane: frame N
  _fast_bytes = 2UL * (w[1] - w[0] + 1) * (w[3] - w[2] + 1);
  fastStart();
  return true;
}

/**************************************************************************/
/*!
    @brief Start an update over the whole RAM window. The next fastWaitIdle()
    waits for it.
*/
/**************************************************************************/
void Adafruit_EPD::fastStart(void) {
  fastWindow(0, _fast_stride - 1, 0, _fast_rows - 1);
  fastUpdate();
  _fast_t0 = micros();
  _fast_pending = true;
}

/**************************************************************************/
/*!
    @brief Wait for the last fast update to finish
*/
/**************************************************************************/
void Adafruit_EPD::fastWaitIdle(void) {
  if (!_fast_pending) {
    return;
  }
  // BUSY may not have risen yet right after the update command
  while (!fastIsBusy() && (micros() - _fast_t0) < FAST_BUSY_RISE_US) {
  }
  uint32_t start = millis();
  while (fastIsBusy()) {
    if (millis() - start > FAST_BUSY_TIMEOUT_MS) {
      // Writes may have been dropped: rewrite the whole frame next time
      _fast_prev_valid = false;
      break;
    }
  }
  _fast_busy_us = micros() - _fast_t0;
  _fast_pending = false;
}

/**************************************************************************/
/*!
    @brief Write a rectangle of a frame to one RAM plane
    @param ram 0 for the new plane, 1 for the old plane
    @param src frame in panel RAM layout
    @param box box to write: first and last RAM byte column, then first and
    last RAM row (both inclusive)
*/
/**************************************************************************/
void Adafruit_EPD::fastWriteBox(uint8_t ram, const uint8_t* src,
                                const int16_t* box) {
  const uint16_t w = box[1] - box[0] + 1;
  uint16_t n = 0;

  fastWindow(box[0], box[1], box[2], box[3]);
  writeRAMCommand(ram);
  dcHigh();
  csLow();
  for (int16_t y = box[2]; y <= box[3]; y++) {
    const uint8_t* p = src + (uint32_t)y * _fast_stride + box[0];
    for (uint16_t i = 0; i < w; i++) {
      fast_chunk[n++] = p[i];
      if (n == FAST_CHUNK) {
        spi_dev->transfer(fast_chunk, n);
        n = 0;
      }
    }
  }
  if (n) {
    spi_dev->transfer(fast_chunk, n);
  }
  csHigh();
}

/**************************************************************************/
/*!
    @brief Fill a whole RAM plane with one byte value
    @param ram 0 for the new plane, 1 for the old plane
    @param value byte to write
*/
/**************************************************************************/
void Adafruit_EPD::fastFill(uint8_t ram, uint8_t value) {
  uint32_t n = fastFrameBytes();

  fastWindow(0, _fast_stride - 1, 0, _fast_rows - 1);
  writeRAMCommand(ram);
  dcHigh();
  csLow();
  while (n) {
    uint16_t m = n > FAST_CHUNK ? FAST_CHUNK : n;
    memset(fast_chunk, value, m); // transfer() overwrites the buffer
    spi_dev->transfer(fast_chunk, m);
    n -= m;
  }
  csHigh();
}

/**************************************************************************/
/*!
    @brief Change the SPI clock (hardware SPI only)
    @param hz new clock in Hz
*/
/**************************************************************************/
void Adafruit_EPD::fastSetSPI(uint32_t hz) {
  if (_spi_bus == NULL) {
    return;
  }
  csHigh();
  delete spi_dev;
  spi_dev = new Adafruit_SPIDevice(_cs_pin, hz, SPI_BITORDER_MSBFIRST,
                                   SPI_MODE0, _spi_bus);
  spi_dev->begin();
}
