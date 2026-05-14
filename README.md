# pico-expert

Touchscreen-driven stepper-motor controller for the Raspberry Pi Pico 2, with a custom ILI9341 TFT + resistive-touch driver and animated UI feedback.

## Overview

Two on-screen buttons rendered on an ILI9341 TFT panel let the user spin a unipolar stepper motor clockwise or counter-clockwise. While the motor rotates, an animated arrow orbits a "dolphin" bitmap to indicate direction and activity. Touch input is acquired from a 4-wire resistive panel via the Pico's ADC, calibrated to the LCD's pixel coordinates.

## Features

- **GUI on an ILI9341 240x320 TFT** with two software buttons and live text labels
- **4-wire resistive touch** driven through the Pico's ADC, with averaging (64 samples) and a pressure-threshold gate
- **Animated rotating indicator**: 8-position arrow orbiting a bitmap, direction-aware
- **Half-step unipolar stepper control** on four GPIO pins for CW/CCW rotation
- **Custom ILI9341 SPI driver** (with optional DMA path), plus a small `gfx` layer for primitives, text, and bitmaps
- **Coordinate transform** mapping raw ADC readings to screen pixels, parametrised by screen rotation
- Bundled 51×64 monochrome bitmap rendered via `gfx_drawBitmap`

## Tech Stack

- **Language:** C (C11)
- **SDK:** Raspberry Pi Pico SDK 2.2.0
- **Hardware:** Raspberry Pi Pico 2 (RP2350), ILI9341 TFT with resistive touch overlay, unipolar stepper motor + driver (e.g. ULN2003)
- **Buses:** SPI (LCD), ADC + GPIO (resistive touch sampling)
- **Build system:** CMake 3.12+

## Architecture

```
main/
├── main.c                            # Application: UI, motor logic, touch handling
├── image_bitmap.h                    # Embedded monochrome bitmap
└── tft_lcd_ili9341/
    ├── ili9341/                      # Low-level SPI driver for the ILI9341 controller
    ├── gfx/                          # Primitives: shapes, text (font6x8), bitmaps, buttons
    └── touch_resistive/              # 4-wire ADC touch acquisition + calibration
```

The main loop polls the touch panel each iteration, maps the point to button regions, sets a direction state (`-1`, `0`, `+1`), then either issues a half-step pulse sequence or stops the motor. Every 8 steps it advances the animated arrow position.

## Pinout

| Signal              | GPIO |
| ------------------- | ---- |
| Stepper IN1 / IN2 / IN3 / IN4 | 5 / 4 / 3 / 2 |
| Touch X- (ADC0)     | 26   |
| Touch Y+ (ADC1)     | 27   |
| Touch X+            | 14   |
| Touch Y-            | 20   |
| LCD                 | SPI (see `ili9341.c`) |

## How to Build

Requires the [Pico SDK](https://github.com/raspberrypi/pico-sdk) and the ARM toolchain.

```bash
mkdir build && cd build
cmake ..
make
```

Flash the resulting `.uf2` to the Pico 2 in BOOTSEL mode.

---
*Developed as part of the Embedded Systems course at [Insper](https://www.insper.edu.br/).*
