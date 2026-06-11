# Shell Command Reference

This document provides a technical specification of the OpenVMS-style command-line interface used in the STM32F4 ASCII Graphics Demo. For a guided introduction, see [How to Use the Shell](../How-to/use-the-shell.md).

## Command Syntax

The shell parser expects commands in a standardized format inherited from OpenVMS conventions. This ensures consistent, predictable behavior across all user input. All shell commands follow the OpenVMS-style grammar:

```shell
COMMAND [TOPIC] [/QUALIFIER=VALUE]
```

* **COMMAND**: Primary action (`RUN`, `HELP`)
* **TOPIC**: Command target or subject (e.g., `DEMO`, `RUN`, program name)
* **QUALIFIER**: Optional modifier prefixed with `/` (e.g., `/MODE=AUTO`)
**Case Sensitivity:** Commands, topics, and qualifiers are case-insensitive.

## Commands

The shell supports two primary commands: `RUN` for program execution and `HELP` for accessing documentation. Each command accepts optional qualifiers to modify behavior.

### RUN

Executes a specified program or application module.

**Syntax:**

```shell
RUN <TOPIC> [/QUALIFIER=VALUE]
```

**Parameters:**

|Parameter|Type|Options|Required|Description|
|---------|----|-------|--------|-----------|
|TOPIC|Literal|`DEMO`|Yes|Program to execute|
|`/MODE`|Qualifier|`AUTO`, `PLAYLIST`|No|Execution mode|
|`/SCENE`|Qualifier|Scene identifier|No|Specific scene to launch|

**Examples:**

```shell
RUN DEMO
RUN DEMO /MODE=AUTO
RUN DEMO /MODE=PLAYLIST
RUN DEMO /SCENE=MATRIX_RAIN
```

**Behavior:**

* If `/MODE` is omitted, defaults to `PLAYLIST` mode
* `/SCENE` bypasses the dashboard menu and launches directly
* Only one mode qualifier may be specified per command

---

### HELP

Displays help documentation for commands, topics, and qualifiers.

**Syntax:**

```shell
HELP [TOPIC] [/QUALIFIER]
```

**Parameters:**

|Parameter|Type|Options|Required|Description|
|---------|----|-------|--------|-----------|
|TOPIC|Literal|`RUN`, `DEMO`|No|Help topic|
|`/MODE`|Qualifier|N/A|No|Subtopic for DEMO modes|
|`/SCENE`|Qualifier|N/A|No|Subtopic for DEMO scenes|

**Examples:**

```shell
HELP
HELP RUN
HELP DEMO
HELP DEMO /MODE
HELP DEMO /SCENE
```

**Behaviour:**

* `HELP` alone returns `SHELL_ERROR_MISSING_TOPIC`
* Subtopics require a parent topic (e.g., `HELP DEMO /MODE`)
* Invalid topics return `SHELL_ERROR_UNKNOWN_TOPIC`

## Qualifiers

Qualifiers are optional parameters that modify command behaviour and use the format: `/QUALIFIER=VALUE`. They follow a consistent naming pattern and format, allowing flexible command construction while maintaining strict syntax validation.

### `/MODE`

Specifies execution behaviour for the DEMO program.

**Valid Values:**

|Value|Behaviour|
|-----|--------|
|`AUTO`|Displays all scenes sequentially at set intervals|
|`PLAYLIST`|Plays curated scenes back-to-back (default)|

**Applicable Commands:** `RUN DEMO`

**Examples:**

```shell
RUN DEMO /MODE=AUTO
RUN DEMO /MODE=PLAYLIST
```

### `/SCENE`

Launches a specific scene directly, bypassing the dashboard.

**Valid Values:** Scene identifiers (implementation-specific)

**Applicable Commands:** `RUN DEMO`

**Examples:**

```shell
RUN DEMO /SCENE=MATRIX_RAIN
RUN DEMO /SCENE=STARFIELD
```

## Error Codes

The shell parser generates error codes for invalid commands or syntax. All errors are displayed as VMS-style messages in red text. The `ShellError_t` enumeration defines six error states, each with a unique message, cause, and recovery path. Consult this section to diagnose command failures and understand the parser's validation rules.

### Error Enumeration

|Code|Enum|Message|Cause|Resolution|
|----|----|-------|-----|----------|
|0|`SHELL_ERROR_NONE`|*(none)*|No error|N/A|
|1|`SHELL_ERROR_UNKNOWN_COMMAND`|`%SYSTEM-E-UNRECOGNIZED, command not found`|Command not recognized|Verify command spelling; use `HELP`|
|2|`SHELL_ERROR_MISSING_TOPIC`|`%HELP-E-NOTOPIC, please specify a help topic (e.g., HELP DEMO)`|`HELP` entered without topic|Specify a topic: `HELP RUN`, `HELP DEMO`|
|3|`SHELL_ERROR_UNKNOWN_TOPIC`|`%HELP-E-UNKNOWNTOPIC, no documentation available for that topic`|Topic not in help library|Use valid topics: `RUN`, `DEMO`, `DEMO /MODE`, `DEMO /SCENE`|
|4|`SHELL_ERROR_UNKNOWN_QUALIFIER`|`%SYSTEM-E-INVQUAL, unrecognized qualifier in command string`|Invalid qualifier or qualifier value|Use valid qualifiers per command specification|
|5|`SHELL_ERROR_INVALID_PARAMETER`|`%SYSTEM-E-INVPARAM, invalid parameter value provided`|Qualifier value is invalid or out of range|Verify qualifier values; use `HELP`|
|6|`SHELL_ERROR_UNEXPECTED_ERROR`|`%SYSTEM-F-ANOMALY, an unexpected shell error occurred`|Unexpected runtime error|Check system state; restart if needed|

## String Constants

All user-visible text for the shell is centralized in `shell_strings.c` to simplify localization and maintenance. Command tokens, qualifiers, and the system prompt are defined as string constants, enabling quick updates without needing to modify the parser logic. This section catalogs all constants and their values.

### Command Tokens

|Constant|Value|Length|
|--------|-----|------|
|`run_command_text`|`"run"`|3|
|`help_command_text`|`"help"`|4|
|`demo_paremeter_text`|`"demo"`|N/A|

### Qualifier Tokens

|Constant|Value|
|--------|-----|
|`auto_mode_qualifier_text`|`"/mode=auto"`|
|`playlist_mode_qualifier_text`|`"/mode=playlist"`|
|`mode_qualifier_text`|`"/mode"`|
|`scene_qualifier_text`|`"/scene"`|

### Prompt

|Constant|Value|
|----------|-------|
|`prompt`|`"STM32F4>"`|

## Shell String Arrays

Help text and boot messages are organized as string arrays in `shell_strings.c`, with corresponding length constants for safe iteration and bounds checking. This design allows help content to be updated independently of the parser, and enables easy addition of new topics and messages.

### Boot Sequence (`shell_boot[]`)

Displayed during shell initialization. 10 lines, accessed via `shell_boot_len`.

```c
const char *const shell_boot[] = {
    /* [0] NAME      */ "STM32F407VG BIOS v1.0.4",
    /* [1] COPYRIGHT */ "(C) 2026 Taseen ASCII Graphics Demo",
    /* [2] CPU       */ "CPU: ARM Cortex-M4 @ 168MHz (PLL_LOCKED)",
    /* [3] SRAM      */ "SRAM: 128KB OK",
    /* [4] FLASH     */ "FLASH: 1024KB OK",
    /* [5] DMA       */ "DMA Controller... Initialized",
    /* [6] UART      */ "UART2 Terminal... Connected at 921600bps",
    /* [7] TERMINAL  */ "Display Mode... 80x24 ANSI Color",
    /* [8] READY     */ "System is ready...",
    /* [9] HINT      */ "Type 'HELP DEMO' for command usage information.",
};
```

### Help Topics

|Array|Purpose|Length|
|-----|-------|------|
|`shell_help_demo[]`|Help for DEMO program|`shell_help_demo_len`|
|`shell_help_run[]`|Help for RUN command|`shell_help_run_len`|
|`shell_help_subkey_mode[]`|Help for `/MODE` qualifier|`shell_help_subkey_mode_len`|
|`shell_help_subkey_scene[]`|Help for `/SCENE` qualifier|`shell_help_subkey_scene_len`|

### Error Messages

Stored in `shell_error[]` array. Access by error code index.

```c
const char *const shell_error[] = {
    /* [0] NONE               */ "",
    /* [1] UNKNOWN_COMMAND    */ "%SYSTEM-E-UNRECOGNIZED, command not found",
    /* [2] MISSING_TOPIC      */ "%HELP-E-NOTOPIC, please specify a help topic (e.g., HELP DEMO)",
    /* [3] UNKNOWN_TOPIC      */ "%HELP-E-UNKNOWNTOPIC, no documentation available for that topic",
    /* [4] UNKNOWN_QUALIFIER  */ "%SYSTEM-E-INVQUAL, unrecognized qualifier in command string",
    /* [5] INVALID_PARAMETER  */ "%SYSTEM-E-INVPARAM, invalid parameter value provided",
    /* [6] UNEXPECTED_ERROR   */ "%SYSTEM-F-ANOMALY, an unexpected shell error occurred",
};
```

## Implementation Details

This section provides the core API and data structures needed to integrate with or extend the shell. Function prototypes define the public interface, while type definitions expose the error enumeration and other critical types used throughout the parser.

### Function Prototypes

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void ShellInit(void)

::: doxy.stm32f4_ascii_graphics_demo.Function
    name: void ShellCommandParser(char *rx_buffer)

### Type Definitions

```c
typedef enum {
 SHELL_ERROR_NONE = 0,          // No error occurred
 SHELL_ERROR_UNKNOWN_COMMAND,   // Command not recognized (e.g., "FLY DEMO")
 SHELL_ERROR_MISSING_TOPIC,     // Command exists, but no topic (e.g., "HELP")
 SHELL_ERROR_UNKNOWN_TOPIC,     // Topic not in library (e.g., "HELP PROJECT")
 SHELL_ERROR_UNKNOWN_QUALIFIER, // Key-2 not recognized (e.g., "HELP DEMO /FAST")
 SHELL_ERROR_INVALID_PARAMETER  // Qualifier value is wrong (e.g., "DEMO /MODE=TURBO")
} ShellError_t;
```

### Source Files

|File|Purpose|
|----|-------|
|`shell.h`|Function prototypes and error type definitions|
|`shell.c`|Command parser implementation|
|`shell_strings.h`|External string declarations and macros|
|`shell_strings.c`|String constants and arrays|
