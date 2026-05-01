# Rendering Strategies

The demo supports two rendering strategies with different bandwidth costs and performance tradeoffs. Full-screen double-buffered DMA transmits the entire 1,920-byte screen every frame at a fixed cost. Blocking/direct print updates only changed positions, costing fewer bytes for sparse updates but requiring careful synchronization. This page compares both approaches and guides strategy selection for new scenes.

## Blocking/Direct Print Mode

In Blocking Print Mode, the system minimizes overhead by updating only the specific character positions that have changed.

Rather than a full-screen refresh, this mode allows each scene to selectively identify which characters at which positions need to be modified. When a change is triggered, the mode sends a cursor positioning command followed by the new character, blocking further execution until the UART transmission is complete. This ensures only the necessary data is pushed to the terminal.

This strategy is built for localized updates where the byte cost varies directly with the number of changed positions. While highly efficient for sparse scenes, this advantage diminishes as the volume of updates increases. In scenes with heavy movement, the overhead of sending individual coordinates for every character can eventually exceed the cost of a single full-screen dump. The following section breaks down how these factors impact system performance and reliability.

### Advantages vs. Disadvantages for blocking print transmission

The following table outlines the trade-offs of using blocking print for terminal updates, focusing on resource allocation, performance scalability, and potential technical bottlenecks.

| Category | Advantages | Disadvantages |
| :--- | :--- | :--- |
| **Efficiency** | Only changed positions consume bandwidth, allowing dramatic reductions compared to full-screen transmission. | Requires scene logic to track changed positions and avoid redundant updates. |
| **Performance** | Localized changes allow for smooth motion and can scale to 60 FPS when updates are sparse. | Frame costs vary based on scene content, making consistent frame budget calculations difficult. |
| **CPU Impact** | Low transmission time for sparse updates allows the CPU to focus on complex logic and animation. | The CPU is blocked during transmission; high-volume updates pause execution until bytes leave the UART. |
| **Reliability** | Transmission time scales with activity level, ensuring efficiency during low-motion periods. | Risk of partial updates if interleaving occurs; highly dependent on terminal-specific positioning accuracy. |

### When to Use Blocking Print

Blocking print works best for visuals where only specific parts of the screen change at a time. It can manmage individual character animations or falling particles by focusing strictly on the changing coordinates. Because the logic only processes these active fragments, the display remains fluid and responsive without straining the hardware.

Persistent UI elements provide a great example of this efficiency. A scoreboard or a clock display highlights the core strength of the technique. The static background remains untouched, while the system only pushes data when a number or a digit actually changes.

## Full-Screen Double-Buffered DMA

In DMA mode, the entire 80x24 screen buffer is rendered by the CPU into a back buffer in memory. When the frame is complete, a buffer swap and DMA trigger command transmit the complete frame as a single burst to the terminal, freeing the CPU to prepare the next frame while data flows over the serial link.

This strategy ensures predictability with a fixed cost of 1,923 bytes per frame. Stable data loads enable reliable timing and consistent performance across different scenes. This method keeps the entire screen in sync during transmission, eliminating visual tearing and terminal-specific positioning errors.

DMA simplifies development by rendering directly to a linear buffer. By relying only on a "home cursor" command and a full buffer dump. The following section summarizes the technical trade-offs regarding system resources and performance limits when using this approach.

### Advantages vs. Disadvantages for full screen DMA transmission

The following table compares the trade-offs of full-screen DMA transmission, specifically regarding timing consistency, bandwidth usage, and hardware compatibility.

| Category | Advantages | Disadvantages |
| :--- | :--- | :--- |
| **Efficiency** | Scene logic remains simple because the code renders to a linear buffer without tracking specific changes. | Fixed transmission costs mean sparse scene updates do not reduce overhead or save bandwidth. |
| **Performance** | Timing is entirely predictable; every frame costs exactly 1,923 bytes regardless of what is happening on screen. | Bandwidth limits are easily reached; at 60 FPS, transmission takes 20.85 ms, which exceeds a standard 16.7 ms frame budget. |
| **CPU Impact** | No complex calculations are needed to determine which areas of the screen require refreshing. | Transmission consumes 62.5% of the frame interval at 30 FPS, leaving only 37.5% for computation and state management. |
| **Reliability** | The entire screen remains in sync during transmission, eliminating tearing and terminal-specific positioning errors. | High bandwidth consumption can starve the CPU of time needed for complex per-frame logic. |

### When to Use DMA

DMA is the best choice for visuals that require a total screen refresh all at once. Full-screen transitions, image galleries, or menu swaps work well here because replacing the entire display in one go is more efficient than tracking thousands of tiny individual changes. In these cases, the simplicity of the setup often outweighs concerns about data usage.

This method ensures that every pixel on the screen updates at the same time. This prevents visual "tearing" and keeps the image perfectly synced during fast motion.

## Trade-Off Byte Cost by Scenario

The choice between blocking print and DMA depends on how many positions change per frame. The byte cost comparison reveals the threshold where one strategy becomes more efficient than the other.

The following table breaks down the data transmission costs for different levels of screen activity, identifying which method uses the least amount of bandwidth in each case.

| Scenario | Changed Positions | Blocking Print Bytes | DMA Bytes | Winner | Efficiency |
| :--- | :--- | :--- | :--- | :--- | :--- |
| Sparse changes | 1-50 | 52-375 | 1,923 | **Blocking Print** | 25-37x more efficient |
| Moderate changes | 50-255 | 375-1,912 | 1,923 | **Blocking Print** | Comparable |
| Heavy changes | 255-500 | 1,912-3,750 | 1,923 | **DMA** | Fixed cost more efficient |

!!! note "Byte Cost Calculations"
    Blocking print per-position cost: cursor positioning (6-8 bytes) + character (1 byte) = 7.5 bytes average
    Full-screen DMA cost: 1,920-byte buffer + 3-byte home sequence (ESC[H) = 1,923 bytes
    For detailed baud rate conversions and frame budget calculations, see [Fundamentals](../Explanation/uart-bandwidth.md)

## Hybrid Approach

Complex visuals scenes can use both strategies at once by splitting the screen into separate zones. A common setup involves using DMA for the main area while reserving a single line at the bottom for blocking print updates. This balances high-speed animation with targeted, efficient data transfer.

The sequence of events impacts the final result. If DMA runs first followed by the blocking print update, the screen looks correct. If the order is reversed, the full-screen DMA refresh might accidentally wipe out the specific changes made by the blocking print. Always plan the execution order to prevent the larger update from overwriting the smaller one.

While this approach saves a lot of bandwidth, it makes the code more difficult to manage. It is best used when there is a clear benefit, such as keeping a status clock ticking smoothly while the rest of the screen performs heavy animations. The bandwidth savings should always be worth the extra effort required to track different screen regions.

## See Also

Deepen your understanding by consulting the reference tables for frame budget limits, the ANSI guide for sequence costs, and the bandwidth fundamentals for core timing constraints.

- [Bandwidth and Timing Fundamentals](../Explanation/uart-bandwidth.md) - Terminal display cost, UART framing, and why frame budgets matter
- [Frame Timing and Bandwidth Reference](../Reference/uart-bandwidth-timing.md) - Lookup tables for frame budgets and transmission times
- [ANSI Code Reference](../Reference/uart-bandwidth-codes.md) - Byte costs for each escape sequence used in terminal updates
