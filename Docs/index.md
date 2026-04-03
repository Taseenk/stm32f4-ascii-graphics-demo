# STM32F4 ASCII Graphics Demo Documentation

Welcome to the official documentation for the **STM32F4 ASCII Graphics Demo**. This project transforms a standard serial terminal into a high-performance visual canvas using bare-metal C on the STM32F407 Discovery board.

## Quick Start

If you have your hardware ready, jump straight into the setup:

* **[Setting Up the Toolchain](Tutorials/setup.md)**: Install the ARM GCC compiler and flashing tools.
* **[Flashing the Board](Tutorials/flashing.md)**: Get the demo running on your STM32 in minutes.
* **[Connecting a Terminal](Tutorials/terminal-setup.md)**: Configure your PC terminal (PuTTY, Tera Term, etc.) to handle the demo in 921,600 baud.

## Explore the Documentation

### [Tutorials](Tutorials/index.md)

Step-by-step lessons for beginners to get the project building and running from scratch.

### [How-to Guides](How-to/index.md)

Recipe-style instructions for specific tasks, such as [Adding a New Scene](How-to/add-a-scene.md) or [Changing the Frame Rate](How-to/change-baud-fps.md).

### [Technical Reference](Reference/index.md)

Detailed technical data, including [Public API](Reference/api.md) documentation, hardware pinouts, and [UART Bandwidth Calculations](Reference/uart-bandwidth.md).

### [Explanations](Explanation/three-tier-architecture.md)

Deep dives into the "Why" behind the design, such as the [3-Tier Architecture](Explanation/three-tier-architecture.md) and the [DMA Double-Buffering strategy](Explanation/dma-double-buffer.md).

## System Overview

The demo operates at **921,600 baud**, pushing the limits of standard UART communication to achieve smooth animations. It leverages the STM32F4's DMA (Direct Memory Access) controller to ensure that rendering doesn't stop while data is being transmitted.

## Project Links

* **Source Code**: [GitHub Repository](https://github.com/Taseenk/stm32f4-ascii-graphics-demo)
* **Author**: [Taseen Khan](https://github.com/Taseenk)
* **Inspiration**: [ASCII AA Project BB Demo](https://youtu.be/FLlDt_4EGX4?si=c_ntV8wBtghTJN6d)
