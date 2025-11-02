#include <Arduino.h>
#include <ESP32Servo.h>
#include <M5Unified.h>

// Servo configuration
const int SERVO_PIN = 2;            // GPIO1 - adjust based on your M5Stack CoreS3 connection
Servo myServo;

// Sine wave parameters
const float FREQUENCY = 1.0;        // Hz - frequency of sine wave
const float AMPLITUDE = 90.0;       // degrees - amplitude of servo movement
const float OFFSET = 90.0;          // degrees - center position of servo
const int SAMPLE_RATE = 50;         // samples per second (50 Hz = 20ms)
const int PRINT_INTERVAL = 100;     // milliseconds between prints

unsigned long lastPrintTime = 0;
unsigned long startTime = 0;

void setup() {
  // Initialize M5 with power output enabled
  auto cfg = M5.config();
  cfg.output_power = true;     // <- enables 5V on Grove/M-Bus ports
  M5.begin(cfg);
  
  // Initialize serial communication
  Serial.begin(115200);
  while (!Serial) {
    delay(10);  // Wait for serial port to connect
  }
  
  delay(300);  // Give time for serial monitor to open
  
  // Initialize servo
  myServo.attach(SERVO_PIN, 500, 2500);  // Attach servo with standard pulse widths (500us - 2500us)
  myServo.write(OFFSET);  // Move to center position
  
  // Print system information
  Serial.println("\n\n================================");
  Serial.println("  M5 Servo Sine Wave Controller");
  Serial.println("================================");
  Serial.println();
  
  Serial.print("Board: ");
  Serial.println(ARDUINO_BOARD);
  
  Serial.print("CPU Frequency: ");
  Serial.print(getCpuFrequencyMhz());
  Serial.println(" MHz");
  
  Serial.print("Flash Size: ");
  Serial.print(ESP.getFlashChipSize() / (1024 * 1024));
  Serial.println(" MB");
  
  Serial.print("Free Heap: ");
  Serial.print(ESP.getFreeHeap() / 1024);
  Serial.println(" KB");
  
  Serial.println();
  Serial.println("M5 Power Configuration:");
  Serial.println("  Grove/M-Bus 5V Output: ENABLED");
  
  Serial.println();
  Serial.println("Servo Configuration:");
  Serial.print("  Pin: GPIO");
  Serial.println(SERVO_PIN);
  Serial.print("  Range: 0-180 degrees");
  Serial.println();
  
  Serial.println();
  Serial.println("Sine Wave Parameters:");
  Serial.print("  Frequency: ");
  Serial.print(FREQUENCY);
  Serial.println(" Hz");
  
  Serial.print("  Amplitude: ");
  Serial.print(AMPLITUDE);
  Serial.println(" degrees");
  
  Serial.print("  Offset: ");
  Serial.print(OFFSET);
  Serial.println(" degrees");
  
  Serial.print("  Sample Rate: ");
  Serial.print(SAMPLE_RATE);
  Serial.println(" Hz");
  
  Serial.println();
  Serial.println("Starting sine wave output...");
  Serial.println("Format: Time(s) | Angle(deg) | Value");
  Serial.println("----------------------------------------");
  
  startTime = millis();
  lastPrintTime = millis();
}

void loop() {
  unsigned long loopStartTime = millis();
  unsigned long currentTime = loopStartTime;
  
  // Update M5 (handles button events, display updates, etc.)
  M5.update();
  
  // Calculate time in seconds since start
  float timeSeconds = (currentTime - startTime) / 1000.0;
  
  // Calculate sine wave angle (0-180 degrees for servo)
  float angle = OFFSET + AMPLITUDE * sin(2 * PI * FREQUENCY * timeSeconds);
  
  // Calculate normalized sine value (-1 to 1)
  float sineValue = sin(2 * PI * FREQUENCY * timeSeconds);
  
  // Move servo to calculated angle
  myServo.write((int)angle);
  
  // Print to serial at specified interval
  if (currentTime - lastPrintTime >= PRINT_INTERVAL) {
    lastPrintTime = currentTime;
    
    // Use fixed-width formatting for clean alignment
    char buffer[100];
    sprintf(buffer, "%7.3f s | %5.1f deg | %6.3f | ", timeSeconds, angle, sineValue);
    Serial.print(buffer);
    
    // Add a simple ASCII visualization
    int barLength = (int)((sineValue + 1.0) * 20);  // Scale to 0-40
    for (int i = 0; i < 40; i++) {
      if (i == barLength) {
        Serial.print("*");
      } else if (i == 20) {
        Serial.print("|");
      } else {
        Serial.print(" ");
      }
    }
    
    Serial.println();
  }
  
  // Calculate elapsed time and delay appropriately to maintain sample rate
  unsigned long elapsedTime = millis() - loopStartTime;
  unsigned long targetDelay = 1000 / SAMPLE_RATE;
  
  if (elapsedTime < targetDelay) {
    delay(targetDelay - elapsedTime);
  }
  // If execution took longer than target, skip delay (no negative delay!)
}