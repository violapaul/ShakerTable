#include <M5Unified.h>
#include "display.h"

// Store last touch position
static int lastX = -1;
static int lastY = -1;

// Small sprite just for the touch indicator (faster than full screen)
static M5Canvas circle(&M5.Display);

static const int CIRCLE_SIZE = 25;
static const int SPRITE_SIZE = CIRCLE_SIZE * 2 + 2;  // Diameter + border

static void drawCircleSprite(int x, int y) {
  // Constrain to screen bounds
  int spriteX = x - CIRCLE_SIZE - 1;
  int spriteY = y - CIRCLE_SIZE - 1;
  
  // Draw the circle indicator
  circle.fillSprite(TFT_BLACK);
  circle.fillCircle(CIRCLE_SIZE + 1, CIRCLE_SIZE + 1, CIRCLE_SIZE, TFT_CYAN);
  circle.drawCircle(CIRCLE_SIZE + 1, CIRCLE_SIZE + 1, CIRCLE_SIZE, TFT_WHITE);
  circle.drawLine(CIRCLE_SIZE - 9, CIRCLE_SIZE + 1, CIRCLE_SIZE + 11, CIRCLE_SIZE + 1, TFT_RED);
  circle.drawLine(CIRCLE_SIZE + 1, CIRCLE_SIZE - 9, CIRCLE_SIZE + 1, CIRCLE_SIZE + 11, TFT_RED);
  
  // Push only the small sprite area
  circle.pushSprite(spriteX, spriteY);
}

void displaySetup() {
  // Set up the display
  M5.Display.setRotation(1);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(2);
  M5.Display.setCursor(60, 100);
  M5.Display.println("Touch the screen!");
  M5.Display.setCursor(40, 140);
  M5.Display.println("A circle will appear");
  
  // Create small sprite for circle only (52x52 vs 320x240 = 17x less data!)
  circle.createSprite(SPRITE_SIZE, SPRITE_SIZE);
  
  Serial.println("Display demo started");
}

void displayLoop() {
  auto touch = M5.Touch.getDetail();
  
  if (touch.isPressed()) {
    int x = touch.x;
    int y = touch.y;
    
    // Only redraw if position changed
    if (x != lastX || y != lastY) {
      // Erase old circle by drawing black rect (faster than full redraw)
      if (lastX >= 0) {
        M5.Display.fillRect(lastX - CIRCLE_SIZE - 1, lastY - CIRCLE_SIZE - 1, 
                            SPRITE_SIZE, SPRITE_SIZE, TFT_BLACK);
      }
      
      // Update header only if needed
      static bool headerDrawn = false;
      if (!headerDrawn) {
        M5.Display.fillRect(0, 0, 320, 30, TFT_DARKGREY);
        headerDrawn = true;
      }
      M5.Display.setCursor(10, 5);
      M5.Display.setTextColor(TFT_WHITE, TFT_DARKGREY);
      M5.Display.printf("Touch: X=%3d  Y=%3d", x, y);
      
      // Draw new circle
      drawCircleSprite(x, y);
      
      // Print to serial (less often to reduce overhead)
      static unsigned long lastPrint = 0;
      if (millis() - lastPrint > 50) {
        Serial.printf("Touch: X=%d, Y=%d\n", x, y);
        lastPrint = millis();
      }
      
      lastX = x;
      lastY = y;
    }
  }
}

