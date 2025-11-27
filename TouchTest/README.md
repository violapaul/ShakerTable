# M5Stack CoreS3 Touch Test

A test project to investigate and measure touch screen performance on the M5Stack CoreS3.

## Background

When implementing a touch-following circle demo, noticeable lag was observed when dragging a finger across the screen. This project was created to investigate the causes and measure actual touch performance.

## Findings

### Touch Controller
- The CoreS3 uses the **FT6336U** capacitive touch controller
- Communicates over **I2C** bus
- Typical polling rate: ~100Hz (10ms between updates)

### Performance Bottlenecks

| Bottleneck | Impact | Mitigation |
|------------|--------|------------|
| **Full-screen sprite push** | 153,600 bytes over SPI per frame | Use partial updates (small sprites) |
| **Multiple draw calls** | Each call blocks until complete | Batch drawing to off-screen buffer |
| **I2C touch polling** | ~10ms hardware limit | Cannot be improved in software |
| **Serial output** | Adds latency if done every frame | Throttle to every 50ms or per-second |

### Key Insight
The **display SPI transfer is often the bottleneck**, not the touch controller. A 320×240×16-bit display requires transferring 153KB per full-screen update. Using small sprites (e.g., 52×52 = 5KB) reduces data transfer by **17×**.

## Project Structure

```
src/
├── main.cpp      # Entry point - switch between modes
├── display.cpp   # Visual demo (circle follows touch)
├── display.h
├── test.cpp      # Performance measurement (update rate)
└── test.h
```

## Usage

### Switching Modes

Edit `main.cpp` line 11:

```cpp
#define CURRENT_MODE  MODE_TEST     // Performance measurement
#define CURRENT_MODE  MODE_DISPLAY  // Visual circle demo
```

### Mode: DISPLAY

Visual demo that draws a cyan circle with crosshair at the touch point.

**Optimizations applied:**
- Small sprite (52×52) instead of full-screen buffer
- Only redraws when position changes
- Background color for text (avoids flicker)

### Mode: TEST

Measures touch performance and prints stats to serial every second:

```
Updates/s:   85  |  Polls/s:  1200  |  Loops/s: 45000  |  Latency:  11.8 ms
```

| Metric | Description |
|--------|-------------|
| **Updates/s** | Position changes detected (perceived responsiveness) |
| **Polls/s** | Touch reads while finger is pressed |
| **Loops/s** | Main loop iterations per second |
| **Latency** | Average ms between position updates |

**Note:** The test mode still updates the display, which may affect measurements. For pure touch performance, comment out lines 66-69 and 88-91 in `test.cpp`.

## Expected Results

With optimized code on CoreS3:
- **Updates/s**: 80-100 (limited by FT6336U I2C polling)
- **Latency**: 10-12ms minimum (hardware limit)
- **Loops/s**: 40,000+ when not touching

If you see significantly lower numbers, check for:
- Other I2C devices causing bus contention
- Heavy computation in the loop
- Unnecessary display updates

## Configuration

`platformio.ini`:
```ini
[env:m5stack-cores3]
platform = espressif32
board = m5stack-cores3
framework = arduino
monitor_speed = 115200
lib_deps = 
    m5stack/M5Unified@^0.1.17
```

## References

- [M5Stack Touch Issues Forum Thread](https://forum.m5stack.com/topic/4219/touch-stopped-working/21)
- [PaperS3 Touch Latency Discussion](https://community.m5stack.com/topic/7567/solved-papers3-touch-screen-latency)
- [M5Unified Library](https://github.com/m5stack/M5Unified)

