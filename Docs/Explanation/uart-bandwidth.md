# Fundamentals

## What Is Bandwidth?

Bandwidth is the maximum capacity to transmit data within a set time frame.[^1] In serial communication, this refers to the total volume of data that can be sent over the serial interface per second. This is a physical limit that determines animation smoothness and frame rate feasibility. Staying within these capacity limits ensures a consistent signal and reliable data transfer.

### Bandwidth in Real-World Scenarios

Now that bandwidth is defined as the physical limit of a connection, it’s important to differentiate it from related concepts. While bandwidth sets the maximum data capacity, other factors determine how effectively that bandwidth is utilized in real-world scenarios.

#### Bandwidth Vs Speed

Bandwidth is often confused with speed, but they are distinct concepts. Speed refers to how fast data is transmitted, while bandwidth is the total amount of data that can be transmitted in a given time.[^1]

#### Throughput

Throughput is the measure of data successfully delivered over a connection in real time. Unlike bandwidth, which is a potential maximum capacity, throughput reflects actual performance.[^1] It takes into account external factors like signal interference, hardware limitations, and protocol overhead that frequently reduce the effective transfer rate.

#### Latency

Latency is the duration of the delay between a sender initiating a transfer and the receiver obtaining that data. This measurement focuses on the time spent travelling.[^1] Even with high bandwidth, significant latency results in noticeable pauses or lag during communication.

### Understanding Baud Rate

While bandwidth is the maximum capacity, baud rate determines the actual speed at which individual bits travel to fill that capacity. In the context of serial communication, baud rate measures the number of signal transitions per second.[^2] In this project this rate directly corresponds to the number of bits per second being moved.

Baud rate is a fundamental element for UART (Universal Asynchronous Receiver-Transmitter), which serves as the primary method for serial data communication in this project. When communicating via UART it is important that the transmitter and receiver have matching baud rate. A difference in the baud rate often results in corrupted data, transmission errors, or a total loss of connectivity.[^3]

### Project Configuration

Following the theoretical concepts of bandwidth and baud rate, it’s important to look at how these are applied in the context of this project. This demo is configured to use a baud rate of 921.600 baud. The highest available speed supported by the STM32F4 microcontroller. This configuration is necessary to maximize bandwidth for animation data. This enables higher frame rates, providing the stability needed for more complex scenes.

#### The 8-N-1 Packet Overhead

To calculate the actual throughput of data, it is necessary to look at how each piece of information is packaged. In a standard UART transmission, every byte of data is wrapped in extra bits to ensure the receiver can identify the beginning and end of the message in the form of a packet.[^2] The table below breaks down the components of a standard 8-N-1 UART packet, which is the configuration used in this project:

| Bit Type | Purpose | Count |
| :--- | :--- | :--- |
| Start | Signals the beginning of data | 1 |
| Data | Contains the actual character | 8 |
| Stop | Signals the end of the frame | 1 |
| Parity | Error checking (disabled here) | 0 |
| **Total** | **Complete serial frame** | **10 bits** |

As show in the table, in this configuration each byte of data requires 10 bits to be transmitted.

!!! note "Throughput Calculation"
    The 10-bit frame directly impacts the maximum bytes per second. At 921.600 baud, the actual transfer capacity is calculated as:

    $$ \frac{921.600 \text{ bits per second}}{10 \text{ bits per byte}} = 92.160 \text{ bytes per second} $$
    
    This 92.160 bytes per second limit represents the total ceiling for all animation data and protocol overhead.

### Why Bandwidth Matters

Every animation frame has a limited time window to complete rendering and transmission. If a frame tries to send more data than this window allows, the transmission spills into the next frame, causing dropped frames and visual stuttering.

!!! note "Byte Budget Constraint"
    Exceeding the available bytes per frame results in dropped frames and visual artifacts. Understanding byte budgets is critical for predictable animation timing.

## The Display Canvas

The demo targets an **80-column by 24-row character display**, matching the standard VT100 terminal dimension. This provides exactly **1.920 character positions** on screen. Each character position stores one byte of data. A full screen refresh therefore requires 1.920 bytes of character data.

### Full-Screen Transmission Cost

To calculate the overhead of a complete refresh, the 1.920 bytes of display data must be paired with the terminal control sequence required to reset the cursor position. Moving the cursor to the top-left corner ensures that the incoming stream aligns correctly with the hardware buffer. The table below breaks down these components to arrive at the total transmission size.

| Component | Description | Data Size |
| :--- | :--- | :--- |
| **Home Cursor** | `ESC[H` (Row 1, Col 1) | 3 bytes |
| **Character Data** | Entire 1.920-byte buffer | 1.920 bytes |
| **Total Cost** | Sum of sequence and data | **1.923 bytes** |

The 1.923-byte figure represents the **fixed baseline cost** for a full-screen DMA transmission. This value functions as a benchmark for evaluating the efficiency of blocking print updates.

!!! hint "Baseline Reference"
    The 1.923-byte cost appears in every frame rate and bandwidth calculation throughout the project. Memorizing this number helps predict frame timings quickly.

## Frame Intervals and Byte Budgets

Frame intervals and byte budgets are at the core of understanding the constraints of this project. The frame interval is the time window available to complete all rendering and transmission before the next frame begins. The byte budget is the maximum amount of data that can be sent within that interval without causing dropped frames.

The following section breaks down how to calculate these values and what they mean for animation performance at different frame rates.

### Calculating Available Time Per Frame

Stability at 921.600 baud requires a fixed transmission window. Setting a target frame rate defines this window. For example, at 30 FPS, the frame interval is approximately 33,3 ms and is calculated as follows:

$$ \text{Frame Interval (ms)} = \frac{1000}{\text{Target FPS}} $$

This interval establishes the hard deadline for all serial communication before the next update cycle begins.

### Byte Budget Calculation

The byte budget represents the total volume of data allowed within a single frame interval. As calculated earlier, the effective throughput of the UART interface at 921.600 baud is 92.160 bytes per second. To determine how many bytes can be sent within each frame, the following formula is used:

$$ \text{Bytes Available per Frame} = \frac{92.160}{\text{Target FPS}} $$

The table below provides a lookup for common targets:

| Target FPS | Frame Interval (ms) | Bytes Available per Frame |
| :--- | :--- | :--- |
| 10 | 100 | 9.216 |
| 30 | 33,3 | 3.072 |
| 60 | 16,7 | 1.536 |

!!! tip "Constraint Awareness"
    As the frame rate increases, the byte budget shrinks rapidly. While 30 FPS allows for 3.072 bytes, a move to 60 FPS cuts that capacity in half to 1.536 bytes. This reduction is critical because a full screen DMA refresh requires 1.923 bytes, making it impossible to sustain 60 FPS using DMA without exceeding the budget.

For detailed tables on transmission times and a breakdown of how different rendering strategies impact these budgets, see the full [bandwidth and timing reference](../Reference/uart-bandwidth-timing.md).

## The Hidden Cost with Escape Sequences

While individual escape sequences appear small, they represent a significant hidden cost that can quickly consume the frame budget. Because these sequences are non-printable control characters, they occupy transmission time without contributing to the actual scene on the screen. The table below details example sequences and their associated costs:

| Sequence | Description | Byte Cost |
| --- | --- | --- |
| `ESC[31m` | Set red foreground | 5 bytes |
| `ESC[12;34H` | Move cursor to row 12, col 34 | 8 bytes |
| `ESC[2J` | Clear screen | 4 bytes |

For a complete breakdown of costs, see the [ANSI Code Reference](../Reference/uart-bandwidth-codes.md) on the exact byte costs for each escape sequence.

!!! warning "Budget Exhaustion"
    At 30 FPS, the total budget is 3.072 bytes. Updating just 220 characters with individual color and position codes can exceed this limit:
    $$ 220 \times (8 \text{ bytes pos} + 5 \text{ bytes color} + 1 \text{ byte char}) = 3.080 \text{ bytes} $$

    This calculation shows how 3.080 bytes surpasses the 3.072 byte limit, leading to frame drops or tearing.

## Rendering Strategies

The demo uses two rendering strategies to manage how data reaches the terminal. Choosing between blocking direct print and full-screen DMA (Direct Memory Access) depends on the number of character changes per frame and the desired frame rate.

### Strategy Comparison

The primary constraint is the byte budget of the UART interface. While blocking print is efficient for sparse updates, full-screen DMA provides predictable timing for complex scenes. The table below illustrates the byte cost and performance tradeoffs between these two modes.

| Category | Blocking Print | Full-Screen DMA |
| :--- | :--- | :--- |
| **Data Cost** | Variable (based on changes) | Fixed (1.923 bytes) |
| **CPU Impact** | Blocks during transmission | CPU is free during transfer |
| **Complexity** | High (must track changes) | Low (linear buffer dump) |
| **Best For** | Sparse UI, text, particles | Heavy motion, full-screen FX |

### Calculating the Efficiency Threshold

Determining the appropriate strategy requires calculating the total bytes required for blocking updates versus the fixed cost of a DMA transfer.

Assuming average of **9,5 bytes per position change** for a mix of single and double digit coordinates. The byte cost for block printing is approximately calculated as:
$$ \text{Total Bytes} \approx \text{Number of Position Changes} \times 9,5 \text{ Bytes} $$

The following calculation shows the **theoretical threshold** where blocking direct print becomes less efficient than a full-screen DMA transfer.

$$ 1.923 \text{ DMA bytes} \div 9,5 \text{ bytes/pos} \approx 202 \text{ positions} $$

Based on this calculation, if a scene modifies **fewer than 200 character positions**, block printing is theoretically more efficient. If **more than 200 position changes** occur, full-screen DMA is the superior choice because the cost remains capped at 1.923 bytes.

### Performance Implications

At 30 FPS, the hardware must complete all transmission within a 33,3 ms frame interval. With a 921.600 baud rate (8-N-1), the total capacity per frame is 3.072 bytes. The table below details how the data volume of each strategy consumes this 33,3 ms time window:

| Strategy | Changed Positions | Total Bytes | Frame Interval Consumed |
| :--- | :--- | :--- | :--- |
| **Blocking Print** | 50 | 475 | $5,2\%$ |
| **Blocking Print** | 200 | 1.900 | $20,6\%$ |
| **Full-Screen DMA** | 1.920 | 1.923 | $20,9\%$ |

For the logic and trade-offs behind these strategies, refer to [rendering strategies](../Explanation/uart-bandwidth-strategies.md). For timing constraints and budget tables, refer to [bandwidth and timing reference](../Reference/uart-bandwidth-timing.md).

!!! warning "Transmission Latency and Jitter"
    Blocking print idles the CPU during transmission, causing the render loop execution time to fluctuate with screen activity. Unlike the constant latency of DMA, a spike in character updates can push transmission beyond the 33,3 ms window, delaying the next frame trigger and causing visible stuttering.

## Frame Rate and Animation Smoothness

Higher frame rates look smoother but demand more bandwidth. A scene running at 30 FPS has 2 times the byte budget of the same scene at 60 FPS, but appears less fluid. The demo defaults to 30 FPS as a practical balance between smooth animation and predictable timing.

Scenes using blocking print for sparse updates can achieve 45 to 60 FPS. Scenes using full-screen DMA are limited to 20 to 30 FPS.

!!! note "Default Frame Rate Selection"
    30 FPS was chosen because it provides sufficient smoothness for most animations while leaving 1.149 bytes per frame for animation logic and effects after DMA transmission.

## Key Metrics at a Glance

Below is a summary of the key metrics and calculations that are referenced throughout the document and the project. This table serves as a quick reference for understanding the bandwidth constraints and performance implications of different rendering strategies.

| Metric | Value | Context |
| :--- | :--- | :--- |
| Baud Rate | 921.600 bits/sec | Hardware limit on serial link |
| Effective Throughput | 92.160 bytes/sec | After 10-bit framing overhead |
| Screen Size | 80×24 = 1.920 bytes | VT100 standard dimensions |
| Full-Screen Cost | 1.923 bytes | Including ESC[H home command |
| Blocking Print per Position | ~9,5 bytes average | Varies by coordinate length |
| 30 FPS Byte Budget | 3.072 bytes | Leaves 1.149 bytes after DMA |
| 60 FPS Byte Budget | 1.536 bytes | Full-screen DMA exceeds this |

## See Also

Following the fundamentals outlined in this document, see the reference tables and strategy guides on how these are applied in this project.

* [Frame Timing and Bandwidth Reference](../Reference/uart-bandwidth-timing.md) - Lookup tables for frame budgets, transmission times, and cost breakdowns at common frame rates
* [ANSI Code Reference](../Reference/uart-bandwidth-codes.md) - Exact byte costs for every escape sequence used in the demo
* [Rendering Strategies](../Explanation/uart-bandwidth-strategies.md) - Decision framework for choosing between full-screen DMA and blocking print based on scene characteristics

## References

[^1]:
    Verizon. (2023, February 21). What is Bandwidth - Definition, Meaning & Explanation. Verizon.com. [https://www.verizon.com/articles/internet-essentials/bandwidth-definition/](https://www.verizon.com/articles/internet-essentials/bandwidth-definition/)

[^2]:
    Pena, E., & Legaspi, M. G. (2024). UART: A Hardware Communication Protocol Understanding Universal Asynchronous Receiver/Transmitter | Analog Devices. Analog.com. [https://www.analog.com/en/resources/analog-dialogue/articles/uart-a-hardware-communication-protocol.html](https://www.analog.com/en/resources/analog-dialogue/articles/uart-a-hardware-communication-protocol.html)

[^3]:
    Ilias Patsiaouras. (2024, August 20). Baud Rate Demystified: Basics and Applications. Bota Systems. [https://www.botasys.com/post/baud-rate-guide](https://www.botasys.com/post/baud-rate-guide)
