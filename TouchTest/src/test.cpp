#include <M5Unified.h>
#include "test.h"

// Performance counters
static unsigned long lastSecond = 0;
static unsigned int touchUpdates = 0;      // How many times touch position changed
static unsigned int touchPolls = 0;        // How many times we polled while touching
static unsigned int loopCount = 0;         // Total loop iterations

// Last position to detect changes
static int lastX = -1;
static int lastY = -1;

// Stats history
static unsigned int lastUpdatesPerSec = 0;
static unsigned int lastPollsPerSec = 0;
static unsigned int lastLoopsPerSec = 0;

void testSetup() {
  M5.Display.setRotation(1);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(2);
  
  M5.Display.setCursor(20, 20);
  M5.Display.println("Touch Performance Test");
  M5.Display.setCursor(20, 60);
  M5.Display.println("Drag finger around");
  M5.Display.setCursor(20, 80);
  M5.Display.println("to measure update rate");
  
  M5.Display.setCursor(20, 140);
  M5.Display.setTextColor(TFT_YELLOW);
  M5.Display.println("Stats printed to Serial");
  
  Serial.println("\n=== Touch Performance Test ===");
  Serial.println("Drag your finger around the screen.");
  Serial.println("Stats will print every second.\n");
  Serial.println("Columns:");
  Serial.println("  Updates/s  = position changes detected");
  Serial.println("  Polls/s    = touch reads while pressing");
  Serial.println("  Loops/s    = main loop iterations");
  Serial.println("  Latency    = avg ms between updates\n");
  
  lastSecond = millis();
}

void testLoop() {
  loopCount++;
  
  auto touch = M5.Touch.getDetail();
  
  if (touch.isPressed()) {
    touchPolls++;
    
    int x = touch.x;
    int y = touch.y;
    
    // Count position changes
    if (x != lastX || y != lastY) {
      touchUpdates++;
      lastX = x;
      lastY = y;
      
      // // Show current position on display
      // M5.Display.fillRect(20, 180, 280, 40, TFT_BLACK);
      // M5.Display.setCursor(20, 180);
      // M5.Display.setTextColor(TFT_CYAN);
      // M5.Display.printf("X=%3d  Y=%3d", x, y);
    }
  } else {
    // Reset last position when not touching
    lastX = -1;
    lastY = -1;
  }
  
  // Print stats every second
  unsigned long now = millis();
  if (now - lastSecond >= 1000) {
    // Calculate average latency
    float avgLatency = touchUpdates > 0 ? 1000.0f / touchUpdates : 0;
    
    // Print to serial
    Serial.printf("Updates/s: %4u  |  Polls/s: %5u  |  Loops/s: %6u  |  Latency: %5.1f ms\n",
                  touchUpdates, touchPolls, loopCount, avgLatency);
    
    // Update display with stats
    M5.Display.fillRect(20, 100, 280, 30, TFT_BLACK);
    M5.Display.setCursor(20, 100);
    M5.Display.setTextColor(TFT_GREEN);
    M5.Display.printf("%u updates/sec", touchUpdates);
    
    // Store for reference
    lastUpdatesPerSec = touchUpdates;
    lastPollsPerSec = touchPolls;
    lastLoopsPerSec = loopCount;
    
    // Reset counters
    touchUpdates = 0;
    touchPolls = 0;
    loopCount = 0;
    lastSecond = now;
  }
}

