#include <Arduino.h>
#include <M5Unified.h>

int touchCount = 0;

// Forward declaration
void updateDisplay();

void setup() {
  // Initialize M5Stack CoreS3
  auto cfg = M5.config();
  M5.begin(cfg);
  
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  
  // Wait briefly for serial port (with timeout to avoid blocking)
  // This allows the device to start even without a serial connection
  unsigned long startTime = millis();
  while (!Serial && (millis() - startTime < 500)) {
    delay(10);
  }
  
  // Print hello world message to serial
  Serial.println("\n\n=== Hello World from M5Stack CoreS3 ===");
  Serial.println("Board initialized successfully!");
  Serial.println("Display initialized!");
  Serial.println("Touch the screen to interact!");
  
  // Display initialization message on screen
  updateDisplay();
}

void updateDisplay() {
  M5.Display.clear();
  
  // Main message - changes based on touch count
  M5.Display.setTextColor(WHITE);
  M5.Display.setTextDatum(middle_center);
  M5.Display.setTextSize(2);
  
  if (touchCount == 0) {
    M5.Display.drawString("Hello World!", M5.Display.width() / 2, 40);
  } else {
    M5.Display.drawString("Touched!", M5.Display.width() / 2, 40);
  }
  
  // Touch count
  M5.Display.setTextSize(1.5);
  M5.Display.setTextColor(CYAN);
  char countStr[30];
  sprintf(countStr, "Touches: %d", touchCount);
  M5.Display.drawString(countStr, M5.Display.width() / 2, 80);
  
  // Instructions
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(YELLOW);
  M5.Display.drawString("Touch screen to count", M5.Display.width() / 2, M5.Display.height() - 15);
}

void loop() {
  M5.update();
  
  // Check for touch events
  auto t = M5.Touch.getDetail();
  if (t.wasPressed()) {
    touchCount++;
    
    // Get touch coordinates
    int touchX = t.x;
    int touchY = t.y;
    
    // Print to serial
    Serial.printf("Touch #%d detected at (%d, %d)\n", touchCount, touchX, touchY);
    
    // Update display with touch info
    M5.Display.clear();
    
    // Main message
    M5.Display.setTextColor(WHITE);
    M5.Display.setTextDatum(middle_center);
    M5.Display.setTextSize(2);
    M5.Display.drawString("Touched!", M5.Display.width() / 2, 30);
    
    // Touch count
    M5.Display.setTextSize(1.5);
    M5.Display.setTextColor(CYAN);
    char countStr[30];
    sprintf(countStr, "Total: %d touches", touchCount);
    M5.Display.drawString(countStr, M5.Display.width() / 2, 65);
    
    // Touch location
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(GREEN);
    char locStr[40];
    sprintf(locStr, "Location: (%d, %d)", touchX, touchY);
    M5.Display.drawString(locStr, M5.Display.width() / 2, 95);
    
    // Draw a small circle at touch point
    M5.Display.fillCircle(touchX, touchY, 5, RED);
    
    // Instructions
    M5.Display.setTextColor(YELLOW);
    M5.Display.drawString("Touch screen again", M5.Display.width() / 2, M5.Display.height() - 15);
  }
  
  delay(10); // Small delay for responsive touch detection
}