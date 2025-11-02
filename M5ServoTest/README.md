# M5Stack CoreS3 Servo Sine Wave Controller

This project controls a servo motor using an M5Stack CoreS3 to generate a sine wave motion pattern. The servo oscillates smoothly based on configurable frequency and amplitude parameters.

## Hardware Setup

### M5Stack CoreS3 Configuration

- **Board**: M5Stack CoreS3
- **Target Device**: ESP32-S3 based M5Stack CoreS3

### Important: PORT.A Reuse

⚠️ **Note**: This project reuses **PORT.A** (the Grove connector Port A) which is an unusual configuration. 

Typically, PORT.A on the M5Stack CoreS3 is designed for I2C-based Grove sensors and modules. In this project, we're repurposing PORT.A's GPIO pin (GPIO2) for direct servo control.

**Why this matters:**
- PORT.A normally expects I2C devices
- We're using one of its GPIO pins directly for PWM servo control
- The 5V power output from the Grove/M-Bus connector is enabled to power the servo

### Hardware Connections

1. **Servo Connection**:
   - **Signal wire (PWM)**: Connect to **GPIO2** (from PORT.A)
   - **Power wire**: Connect to **5V** from the Grove/M-Bus connector (enabled via software)
   - **Ground wire**: Connect to **GND** from the Grove/M-Bus connector

2. **Power Configuration**:
   - The code enables 5V output on Grove/M-Bus ports via `cfg.output_power = true`
   - This provides power for the servo motor

### Servo Specifications

- **PWM Signal**: GPIO2
- **Pulse Width Range**: 500μs - 2500μs (standard servo range)
- **Angle Range**: 0-180 degrees
- **Power**: Provided via Grove/M-Bus 5V output

## Software Setup

### Prerequisites

- PlatformIO installed
- USB cable for programming and power

### Dependencies

The project uses the following libraries (defined in `platformio.ini`):

- **M5Unified** (v0.1.15): M5Stack unified library for CoreS3 support
- **ESP32Servo** (v3.0.5): Servo control library for ESP32

### Building and Uploading

1. Open the project in PlatformIO
2. Connect the M5Stack CoreS3 via USB
3. Build and upload:
   ```bash
   pio run --target upload
   ```

4. Open serial monitor (115200 baud):
   ```bash
   pio device monitor
   ```

## Configuration

### Servo Parameters (in `main.cpp`)

- **SERVO_PIN**: GPIO2 (PORT.A pin)
- **FREQUENCY**: 1.0 Hz (adjustable)
- **AMPLITUDE**: 90.0 degrees (adjustable)
- **OFFSET**: 90.0 degrees (center position)
- **SAMPLE_RATE**: 50 Hz (20ms update interval)

### Adjusting Parameters

Edit the constants at the top of `main.cpp`:

```cpp
const float FREQUENCY = 1.0;        // Hz - frequency of sine wave
const float AMPLITUDE = 90.0;       // degrees - amplitude of servo movement
const float OFFSET = 90.0;          // degrees - center position of servo
const int SAMPLE_RATE = 50;         // samples per second (50 Hz = 20ms)
```

## Usage

1. Power on the M5Stack CoreS3
2. The servo will automatically start generating a sine wave motion
3. Monitor output via serial console at 115200 baud
4. The serial output shows:
   - Time elapsed
   - Current servo angle
   - Normalized sine value (-1 to 1)
   - Visual ASCII bar graph

## Serial Output Format

```
Time(s) | Angle(deg) | Value | [Visual Bar]
```

Example:
```
  0.000 s |  90.0 deg |  0.000 |          |                    *
  0.100 s |  95.4 deg |  0.309 |          |                *
  0.200 s | 135.6 deg |  0.951 |          |    *
```

## Troubleshooting

### Servo Not Moving

1. Verify 5V power is connected correctly
2. Check that GPIO2 signal wire is properly connected
3. Ensure `cfg.output_power = true` is set in the code
4. Verify servo power requirements match 5V output capability

### PORT.A Conflicts

Since PORT.A is being reused for servo control:
- Do not connect other I2C devices to PORT.A
- If using other Grove ports, be aware of potential pin conflicts
- GPIO2 may conflict with other peripherals if used elsewhere

## Technical Notes

- The sine wave generation is time-based and continuous
- Servo updates occur at 50 Hz (20ms intervals) for smooth motion
- The code includes CPU frequency and memory information on startup
- M5.update() is called in the main loop to maintain M5Stack functionality

