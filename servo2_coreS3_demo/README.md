# CoreS3 + Servo2 (PCA9685) Demo (PlatformIO)

This project demonstrates driving the M5Stack **Servo2** module from a **CoreS3** over I²C.

## Hardware notes

- Power Servo2 with **6–12 V** via the XT60 input. USB power on CoreS3 is not enough for servos.
- Set the M5Stack stack bus to **power input** in software (done in `setup()` with `cfg.output_power = false;`).
- I²C address is **0x40–0x47** depending on the three DIP switches (A2 A1 A0). Default (all OFF) is **0x40**.
- CoreS3 external I²C pins are **GPIO1 (SDA)** and **GPIO2 (SCL)**.

## Build & run

```bash
# From this folder:
pio run -t upload
pio device monitor -b 115200
```

On boot the sketch scans I²C and should list the PCA9685 address. It then sets 50 Hz and sweeps channel 0.