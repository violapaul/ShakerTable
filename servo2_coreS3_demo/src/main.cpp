/*
  CoreS3 + Servo2 (PCA9685) minimal demo
  - Sets M-BUS to input power mode
  - Brings up external I2C on GPIO1/2
  - Scans I2C and initializes PCA9685 at 50 Hz
  - Sweeps channel 0 as a sanity check
*/

#include <M5Unified.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

// Adjust if you changed the DIP (A2 A1 A0). Default is 0x40.
constexpr uint8_t SERVO2_ADDR = 0x40;

// CoreS3 external I2C pins:
//constexpr int SDA_PIN = 1;
//constexpr int SCL_PIN = 2;
constexpr int SDA_PIN = 12;
constexpr int SCL_PIN = 11;

constexpr int SERVO_CHANNEL = 8;

// Use the default Wire instance because the Adafruit driver accepts a reference.
Adafruit_PWMServoDriver pca(SERVO2_ADDR, Wire);

// Map 0..180 degrees to PCA9685 ticks for ~50 Hz operation.
static uint16_t angleToTicks(float deg) {
  const float period_ms = 20.0f;   // 50 Hz
  const float min_ms = 0.5f;       // typical pulse width range for hobby servos
  const float max_ms = 2.5f;
  float pulse_ms = min_ms + (deg / 180.0f) * (max_ms - min_ms);
  uint16_t ticks = (uint16_t)(pulse_ms / period_ms * 4096.0f);
  if (ticks > 4095) ticks = 4095;
  return ticks;
}

static void i2cScan() {
  Serial.println("I2C scan...");
  M5.Display.println("I2C Scan:");
  int deviceCount = 0;
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  - found 0x%02X\r\n", addr);
      M5.Display.printf("  Found: 0x%02X\n", addr);
      deviceCount++;
      delay(2);
    }
  }
  if (deviceCount == 0) {
    M5.Display.println("  No devices found!");
  }
}

void setup() {
  auto cfg = M5.config();
  // Critical for stack modules: make the M-BUS a power INPUT (module powered by host).
  cfg.output_power = false;
  // Initialize CoreS3 (display, touch, speaker, IMU, etc.).
  M5.begin(cfg);

  Serial.begin(115200);
  delay(200);

  // Setup display
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.println("CoreS3 + Servo2");
  M5.Display.println("PCA9685 Demo");
  M5.Display.println("---------------");

  // Bring up *external* I2C on GPIO1/2 explicitly.
  M5.Display.println("Init I2C...");
  Wire.begin(SDA_PIN, SCL_PIN, 400000); // 400kHz works fine for PCA9685
  delay(100);

  i2cScan(); // Expect 0x40..0x47 depending on DIP switches.

  M5.Display.println("---------------");
  M5.Display.printf("Init PCA9685\n@ 0x%02X...\n", SERVO2_ADDR);
  
  if (!pca.begin()) {
    Serial.println("ERROR: PCA9685 not found at configured address. Check power, DIP, wiring.");
    M5.Display.setTextColor(TFT_RED);
    M5.Display.println("\nERROR: PCA9685");
    M5.Display.println("not found!");
    while (true) { delay(1000); }
  }
  pca.setPWMFreq(50); // 50 Hz for hobby servos
  Serial.println("PCA9685 ready at 50 Hz.");
  
  M5.Display.setTextColor(TFT_GREEN);
  M5.Display.println("OK - 50 Hz");
  
  // Optional: center channel 0 on boot
  pca.setPWM(SERVO_CHANNEL, 0, angleToTicks(90.0f));
  M5.Display.setTextColor(TFT_YELLOW);
  M5.Display.println("\nCentering CH0...");
  delay(1000);
  
  // Clear screen for servo display
  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setCursor(0, 0);
  M5.Display.setTextColor(TFT_CYAN);
  M5.Display.setTextSize(3);
  M5.Display.println("Servo CH0");
  M5.Display.println();
}

void loop() {
  // Simple sweep on channel 0

  uint32_t delayTime = 10;
  int startAngle = 30;
  int endAngle = 150;
  float angleStep = 0.5;

  for (float deg = startAngle; deg <= endAngle; deg += angleStep) {
    pca.setPWM(SERVO_CHANNEL, 0, angleToTicks((float)deg));
    
    // Update display with current angle
    M5.Display.fillRect(0, 80, 320, 60, TFT_BLACK); // Clear angle area
    M5.Display.setCursor(0, 80);
    M5.Display.setTextSize(4);
    M5.Display.setTextColor(TFT_YELLOW);
    M5.Display.printf("%3d", deg);
    M5.Display.setTextSize(3);
    M5.Display.print(" deg");
    
    // Draw a simple bar graph
    int barWidth = (deg * 300) / 180;
    M5.Display.fillRect(10, 160, barWidth, 30, TFT_GREEN);
    M5.Display.fillRect(10 + barWidth, 160, 300 - barWidth, 30, TFT_DARKGREY);
    
    delay(delayTime);
  }
  for (float deg = endAngle; deg >= startAngle; deg -= angleStep) {
    pca.setPWM(SERVO_CHANNEL, 0, angleToTicks((float)deg));
    
    // Update display with current angle
    M5.Display.fillRect(0, 80, 320, 60, TFT_BLACK); // Clear angle area
    M5.Display.setCursor(0, 80);
    M5.Display.setTextSize(4);
    M5.Display.setTextColor(TFT_YELLOW);
    M5.Display.printf("%3d", deg);
    M5.Display.setTextSize(3);
    M5.Display.print(" deg");
    
    // Draw a simple bar graph
    int barWidth = (deg * 300) / 180;
    M5.Display.fillRect(10, 160, barWidth, 30, TFT_GREEN);
    M5.Display.fillRect(10 + barWidth, 160, 300 - barWidth, 30, TFT_DARKGREY);
    
    delay(delayTime);
  }

  // If you build a touch UI, you may call M5.update() here. If you see
  // flakiness while touching, throttle UI updates or servo writes.
  // M5.update();
}