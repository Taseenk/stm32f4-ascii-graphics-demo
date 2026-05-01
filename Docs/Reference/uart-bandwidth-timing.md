# Bandwidth and Timing Reference

This reference provides lookup tables and calculations for frame timing, baud rate conversions, and bandwidth budgets at the demo target of 921,600 baud.

Each section contains values for common scenarios along with the underlying formulas to calculate values for custom configurations. The byte budget (total bytes allowed per frame) is the fundamental constraint determining all rendering decisions.

## Frame Interval and Available Byte Budget at 921,600 Baud

At the target baud rate of 921,600, the following table illustrates the byte capacity within each frame interval for common frame rates. This byte budget (the total amount of data allowed before exceeding the time limit) dictates whether blocking print updates or full-screen DMA transfers remain within the specified timing constraints.

| Target FPS | Frame Interval (ms) | Bytes Available per Frame |
| :--- | :--- | :--- |
| 10 | 100 | 9,216 |
| 15 | 66.7 | 6,144 |
| 20 | 50 | 4,608 |
| 30 | 33.3 | 3,072 |
| 60 | 16.7 | 1,536 |

!!! note "Frame Timing Calculation"
    The demo frame timing relies on the HAL tick incrementer. To determine the duration of a single frame in milliseconds, divide 1,000 by the targeted frames per second.
    $$ \text{Frame Interval (ms)} = \frac{1000}{\text{Target FPS}} $$

!!! note "Byte Budget Calculation"
    The available bytes per frame depend on the raw throughput of the serial interface. At 921,600 baud using 8-N-1 signaling (10 bits per byte), the interface provides a constant 92,160 bytes per second.
    $$ \text{Bytes Available per Frame} = \frac{92,160}{\text{Target FPS}} $$

## Transmission Time at Different Data Sizes
  
For a screen to update without tearing, the entire data payload must finish sending before the next frame begins. The following table shows the time required to send specific byte data at 921,600 baud. These values help determine how much of the frame interval a specific update strategy consumes.

| Bytes Transmitted | Transmission Time (ms) |
| :-- | :-- |
| 100 | 1.1 |
| 500 | 5.4 |
| 1,000 | 10.9 |
| 1,500 | 16.3 |
| 1,923 (full screen + Home) | 20.9 |
| 3,000 | 32.6 |
  
!!! note "Transmission Time Calculation"
    The transmission time represents the duration required to push data through the interface. This is calculated by dividing the byte count by the bytes per second and multiplying by 1,000 to convert to milliseconds.
    $$ \text{Transmission Time (ms)} = (\frac{\text{Bytes}}{92,160}) \times 1000 $$

## Blocking Print Cost per Position

Blocking print sends cursor positioning and a single character byte for every updated position. Byte data payload depends on coordinate length because single digit coordinates require fewer bytes than double digit rows and columns. This table breaks down the byte payload for scene updates using blocking print.

| Coordinate Type | Fixed Bytes | Positioning Bytes | Character Bytes | Total Cost |
| :--- | :--- | :--- | :--- | :--- |
| **Single digits** | 5 bytes | 2 bytes | 1 byte | **8 bytes** |
| **Mixed digits** (e.g. 5,20) | 5 bytes | 3 bytes | 1 byte | **9 bytes** |
| **Double digits** | 5 bytes | 4 bytes | 1 byte | **10 bytes** |
| **Weighted Average** | 5 bytes | 3.5 bytes | 1 byte | **9.5 bytes** |

!!! note "Position Cost Calculation"
    Each position update costs 3 bytes (ESC[) + row digits + 1 byte separator (;) + column digits + 1 byte (H) + 1 byte (character).
    $$Cost\ per\ Position = 3 + len(row) + 1 + len(column) + 1 + 1$$

## Frame Interval Impact of Full-Screen DMA Updates
  
Full-screen updates via DMA carry a fixed transmission cost of 20.9 ms per refresh. This duration represents the time required to send 1,923 bytes plus positioning overhead at 921,600 baud.

The following table illustrates what percentage of the available frame interval is consumed by a single complete screen refresh. If this value exceeds 100%, the transmission cannot complete before the next frame is scheduled to begin.

| Target FPS | Frame Interval (ms) | Transmission Time (ms) | Percentage of Interval |
| :-- | :-- | :-- | :-- |
| 10 | 100.0 | 20.9 | 21% |
| 15 | 66.7 | 20.9 | 31% |
| 20 | 50.0 | 20.9 | 42% |
| 30 | 33.3 | 20.9 | 63% |
| 60 | 16.7 | 20.9 | 125% (Exceeds Budget) |
  
!!! note "Frame Budget Percentage Calculation"
    The percentage of the frame consumed is the ratio between the transmission time and the available frame interval.
    $$ \text{Percentage of Frame} = \left( \frac{\text{DMA Transmission Time}}{\text{Frame Interval}} \right) \times 100 $$

## Rendering Strategy Comparison: Blocking Print vs. DMA

Selecting between blocking print and full-screen DMA depends on the number of position changes per frame. The comparison below outlines the byte costs and frame consumption for typical scenes at 921,600 baud. This data identifies whether multiple blocking updates or a single full-screen DMA transfer fits the frame budget.

| Scenario blocking print | Bytes Transmitted | Transmission Time (ms) | 30 FPS % | 60 FPS % |
| :--- | :--- | :--- | :--- | :--- |
| Sparse (10 positions) | 80 | 0.9 | 3% | 5% |
| Moderate (100 positions) | 750 | 8.1 | 24% | 49% |
| Heavy (500 positions) | 3,750 | 40.7 | 122% | 244% |

| Scenario | Bytes Transmitted | Transmission Time (ms) | 30 FPS % | 60 FPS % |
| :--- | :--- | :--- | :--- | :--- |
| Full screen DMA | 1,923 | 20.9 | 63% | 125% |

!!! note "When Each Strategy Works"
    Blocking print maintains efficiency when fewer than 255 positions change per frame. Full-screen DMA is superior when changes impact the entire screen because the fixed $1,923$ byte cost is lower than the cumulative total of hundreds of blocking print updates.

## See Also

This document provides lookup tables and concrete numbers for reference. Effective application requires understanding the underlying concepts.

- [Fundamentals](../Explanation/uart-bandwidth.md) - Why baud rate and framing affect frame timing
- [Rendering Strategies](../Explanation/uart-bandwidth-strategies.md) - Deep dive into when to use blocking print vs. DMA based on byte costs and frame budgets
- [ANSI Code Reference](../Reference/uart-bandwidth-codes.md) - Exact byte costs for each escape sequence
