# STM32F4 ASCII Graphics Demo

![Build](https://github.com/Taseenk/stm32f4-ascii-graphics-demo/actions/workflows/build.yml/badge.svg)
![Platform](https://img.shields.io/badge/platform-STM32F4-blue)
![Language](https://img.shields.io/badge/language-C11-lightgrey)

> Real-time ASCII art demo scenes rendered on a PC terminal over a high-speed UART link, running bare-metal on the STM32F407 Discovery board.

<!-- TODO: Add a demo GIF here -->

## Overview

Inpsire by the ASCII BB demo showcasing a constrained machine, continuous multi-scene loop could produce something worth watching. This project applies that same philosophy to the STM32F407.

This project aims to use a standard serial terminal as a visual canvas. Running at **921,600 baud** on an ARM Cortex-M4, the firmware renders animated scenes, while a CLI shell lets you control playback.

## Features

- **High-speed UART** — 921,600 baud with DMA-backed transmission to keep the CPU free for rendering
- **3-tier architecture** — Separation between animation logic, terminal formatting, and hardware drivers
- **Scene manager** — auto and playlist playback modes with configurable durations and transition effects
- **CLI shell** — VMS-style command interface (`RUN DEMO /MODE=AUTO`, `HELP DEMO`, etc.)
- **Hardware RNG** — true random seed from the STM32F407 TRNG, extended per-frame with Xorshift
- **Interactive dashboard** — boot menu with blinking selection and FPS display

## Getting Started

### Prerequisites

#### Hardware

#### Software

### Build

### Flash & Run

## Project Structure

```yaml
├── App/
│   ├── Inc/                            # Application headers
│   │   └── Scenes/                     # Scene headers
│   └── Src/                            # Application source
│       └── Scenes/                     # Scene implementations
├── Core/                               # CubeMX-generated HAL init
│   ├── Inc/
│   └── Src/
├── Drivers/                            # STM32 HAL & CMSIS
├── Docs/
│   └── LikeC4/                         # Architecture-as-code diagrams
├── CMakeLists.txt
└── stm32f4-ascii-graphics-demo.ioc     # CubeMX project file
```

## Documentation

Full documentation — tutorials, how-to guides, module reference, and architecture diagrams — is hosted at:

> **[docs link — TODO](#)**

## License

## Acknowledgements & Inspiration

- [ASCII AA Project BB Demo](https://youtu.be/FLlDt_4EGX4?si=c_ntV8wBtghTJN6d): Main Inspiration for the project
- [VT100 ASCII Animation: TortureTest](https://youtu.be/4ZeDudfzAs0?si=S6oy03UdSebeTyBo): Inspiration for Attributes demo scene
