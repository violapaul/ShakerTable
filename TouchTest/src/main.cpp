#include <M5Unified.h>
#include "display.h"
#include "test.h"

// ============================================
// SELECT WHICH MODE TO RUN:
// ============================================
#define MODE_DISPLAY  0   // Visual touch demo with circle
#define MODE_TEST     1   // Performance test (measures update rate)

#define CURRENT_MODE  MODE_TEST   // <-- Change this to switch modes
// ============================================

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  
  Serial.begin(115200);
  Serial.println("\n\nTouch Test Starting...");
  
#if CURRENT_MODE == MODE_DISPLAY
  Serial.println("Mode: DISPLAY (visual demo)");
  displaySetup();
#elif CURRENT_MODE == MODE_TEST
  Serial.println("Mode: TEST (performance measurement)");
  testSetup();
#endif
}

void loop() {
  M5.update();
  
#if CURRENT_MODE == MODE_DISPLAY
  displayLoop();
#elif CURRENT_MODE == MODE_TEST
  testLoop();
#endif
}
