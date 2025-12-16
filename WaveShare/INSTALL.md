# Waveshare ESP32-S3-SIM7670G-4G + PlatformIO on macOS (Reliable Flashing)

This doc describes how to reliably upload firmware to the Waveshare **ESP32-S3-SIM7670G-4G** board from **macOS** using **PlatformIO**.

Key facts about this board (from Waveshare):
- Onboard **CH343** (UART↔USB) and **CH334** (USB hub), plus a USB switching IC. :contentReference[oaicite:0]{index=0}
- A 4-position DIP switch controls **CAM**, **HUB power**, **4G power**, and **USB channel switching for the SIM7670G module**. :contentReference[oaicite:1]{index=1}
- The **ESP32-S3 UART↔USB** and the **4G module USB** share the same USB Type-C interface; you select which is connected using the **USB channel** DIP switch. :contentReference[oaicite:2]{index=2}

## Goal

Use the **CH343 UART↔USB serial port** for uploads. On macOS it appears as:

- `/dev/cu.wchusbserial*`

This avoids native-USB re-enumeration issues that can break uploads.

---

## 1) Hardware: DIP switch guide (what each switch does)

Waveshare’s product page defines the DIP functions as: :contentReference[oaicite:3]{index=3}

- **CAM**: turn camera circuit on/off
- **HUB**: controls power to the **USB HUB driver circuit**
- **4G**: controls power to the **SIM7670G module**
- **USB**: switches to the **USB channel of the SIM7670G module** (i.e., selects whether the Type-C USB data lines go to the modem USB vs the ESP32/CH343 side to avoid conflicts)

Waveshare wiki adds the crucial detail: :contentReference[oaicite:4]{index=4}
- ESP32-S3 UART↔USB and 4G module USB share the **same Type-C**.
- The **USB channel DIP** chooses between:
  - the 4G module USB interface, or
  - the ESP32-side connection via Type-C.

### Recommended DIP settings for firmware flashing (PlatformIO)

For *simple, stable firmware upload*, the safest approach is:

- **CAM:** OFF
- **4G:** OFF (reduce power draw + avoid USB conflicts while you’re just flashing)
- **HUB:** typically OFF (unless your board revision requires it; see verification below)
- **USB channel:** set to the **ESP32/CH343 side** (NOT the SIM7670 USB channel)

> Important: DIP “ON” direction varies by silkscreen and switch orientation.  
> Don’t rely on “ON/OFF” text alone — rely on the verification checks below.

### Verification: you’re in the correct DIP state when…

After plugging the board in, **macOS must show a WCH serial device**:

```bash
ls /dev/cu.* | egrep "wch|usbserial"
````

Expected output example:

```text
/dev/cu.wchusbserial59720625521
```

If you **only** see `/dev/cu.usbmodem*` (or nothing), your USB channel DIP is likely routing Type-C to the wrong side (or the driver isn’t installed).

---

## 2) macOS: install the CH34x / CH343 driver (required for `/dev/cu.wchusbserial*`)

The CH343 is a WCH/QinHeng USB-serial device. Waveshare lists CH343 onboard. ([Waveshare][1])

### Download (official WCH)

Use WCH’s official macOS driver page:

```text
https://www.wch-ic.com/downloads/CH34XSER_MAC_ZIP.html
```

WCH also maintains an installation guide repo referencing that same driver link. ([GitHub][2])

### Install steps (macOS 11+ especially)

1. Download and install the `.pkg` from the ZIP.
2. Open **System Settings → Privacy & Security**
3. If you see a message about blocked system software (WCH/QinHeng), click **Allow**
4. Reboot

(Adafruit has a clear walkthrough of the macOS “Allow” step for CH34x drivers.) ([Adafruit Learning System][3])

### Confirm the driver is working

Plug the board in and run:

```bash
ls /dev/cu.* | egrep "wch|usbserial"
```

If you see `/dev/cu.wchusbserial*`, the driver is active and the board is routed correctly.

You can also confirm at the USB level:

```bash
system_profiler SPUSBDataType | egrep -i "1a86|wch|qinheng|usb single serial|ch34" -n
```

Look for WCH/QinHeng (Vendor ID 0x1a86) and a “USB Single Serial”-type device.

---

## 3) PlatformIO: minimal working project configuration

Create/modify `platformio.ini`:

```ini
[env:waveshare]
platform = espressif32
board = esp32-s3-devkitc-1
framework = arduino

monitor_speed = 115200

upload_protocol = esptool
upload_port = /dev/cu.wchusbserial59720625521
upload_speed = 460800
```

Notes:

* Replace `upload_port` with whatever `ls /dev/cu.*` shows.
* Keep this minimal until upload is reliable.

---

## 4) Upload workflow

From the project directory:

```bash
pio run -t upload
```

If it fails to connect, do a one-time manual ROM bootloader entry:

1. Hold **BOOT**
2. Tap **RESET**
3. Release **BOOT**
4. Immediately retry upload

---

## 5) Common failure modes & fixes

### A) Port exists but upload fails with “resource not found” / disconnect mid-flash

Symptom (often with native USB `/dev/cu.usbmodem*`):

* Upload starts, then macOS device disappears and esptool errors.

Fix:

* Use the **CH343 UART** path (`/dev/cu.wchusbserial*`) instead.
* Ensure USB channel DIP routes Type-C to ESP32/CH343 side. ([Waveshare][4])

### B) pySerial “Inappropriate ioctl for device”

Often means:

* wrong device node,
* a conflicting driver,
* or the port is actually held open.

Check and close anything using the port:

```bash
lsof | grep wchusbserial
```

If you see `screen` (or another process), kill it:

```bash
kill <PID>
# or
kill -9 <PID>
```

### C) PlatformIO esptool dependency errors (e.g., `ModuleNotFoundError: intelhex`)

This usually means PlatformIO’s tool package is corrupted.

Reset the tool package:

```bash
rm -rf ~/.platformio/packages/tool-esptoolpy
rm -rf ~/.platformio/.cache
pio run -t clean
pio run -t upload
```

### D) No `/dev/cu.wchusbserial*` even after driver install

Work the checklist:

1. Try a known-good **data** USB-C cable and plug directly into the Mac.
2. Flip the **USB channel** DIP and replug until the WCH serial device appears.
3. Confirm WCH device appears in `system_profiler SPUSBDataType`.
4. Re-check **Privacy & Security → Allow** and reboot.

---

## 6) DIP switch “recipes” (practical setups)

These recipes are described in terms of *intent* plus the *verification checks* that prove they worked.

### Recipe 1 — Firmware flashing (recommended)

Intent:

* Use CH343 UART for stable flashing.

Actions:

* Disable power-hungry/conflicting circuits: CAM OFF, 4G OFF
* Route USB Type-C data to ESP32/CH343 side (NOT modem USB channel)

Verify:

* `ls /dev/cu.* | egrep "wch|usbserial"` shows `/dev/cu.wchusbserial*`

### Recipe 2 — Modem USB debugging / dial-up on a PC

Intent:

* Put SIM7670 on USB for modem debugging / dial-up internet.

Actions:

* Power modem: 4G ON
* Switch USB channel to SIM7670G module USB

Verify:

* macOS/PC enumerates modem USB interfaces (won’t show as `wchusbserial`)

(Waveshare describes the USB switching + DIP purpose as enabling SIM7670G USB use for dial-up/debugging, and explicitly notes the shared Type-C and USB channel selection.) ([Waveshare][1])

---

## Appendix: Why this board is confusing (and how to stay sane)

Because the board includes:

* CH343 UART↔USB for ESP32 development ([Waveshare][1])
* a USB hub and a USB switching IC ([Waveshare][1])
* a DIP switch that can route Type-C USB data lines to either the ESP32/CH343 side or the modem USB side ([Waveshare][4])

…the only reliable way to confirm you’re in the right mode is:

* check what macOS enumerated (`system_profiler SPUSBDataType`)
* check which `/dev/cu.*` nodes appear

If you can see `/dev/cu.wchusbserial*`, PlatformIO upload via `esptool` will be stable.

- https://www.waveshare.com/esp32-s3-sim7670g-4g.htm "ESP32-S3 SIM7670G 4G Development Board, LTE Cat-1 / WiFi / Bluetooth / GNSS Positioning Support, Global Coverage Multi Band |  ESP32-S3-SIM7670G-4G"
- https://github.com/WCHSoftGroup/ch34xser_macos?utm_source=chatgpt.com "WCHSoftGroup/ch34xser_macos: MacOS USB driver for ..."
- https://learn.adafruit.com/how-to-install-drivers-for-wch-usb-to-serial-chips-ch9102f-ch9102/mac-driver-installation?utm_source=chatgpt.com "How To Install Drivers for WCH CH34X / CH340 / CH341 ..."
- https://www.waveshare.com/wiki/ESP32-S3-SIM7670G-4G "ESP32-S3-SIM7670G-4G - Waveshare Wiki"
