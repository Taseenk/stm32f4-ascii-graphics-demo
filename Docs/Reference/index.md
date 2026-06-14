# Reference

Reference documentation provides technical lookup information organized by functional area. Find specific details about functions, commands, timing constraints, and escape codes.

## Terminal Module

The terminal module handles all serial output and screen control. Documentation covers cursor positioning and screen management, text styling with colours and attributes, and two rendering strategies with graphics support.

- [Screen & Cursor Control](../Reference/terminal-screen-cursor.md)
- [Colour & Text Attributes](../Reference/terminal-colour-attributes.md)
- [Rendering Modes & Graphics](../Reference/terminal-rendering.md)

## Shell Interface

The shell provides an OpenVMS-style command interface for controlling system behaviour. Documentation covers command syntax, available commands and qualifiers, and error codes.

- [Shell Command Reference](../Reference/shell-commands.md)

## UART Communication

UART transmission operates at 921,600 baud, which constrains frame timing and data capacity. Documentation provides timing calculations, byte budgets, and optimization data.

- [Bandwidth and Timing Reference](../Reference/uart-bandwidth-timing.md)
- [ANSI Code Reference](../Reference/uart-bandwidth-codes.md)
