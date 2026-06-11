# How to Add a New Scene

Developing a new visual effect for the demo requires building a scene from scratch, registering it within the scene manager, and integrating it directly into the playback system.

This guide assumes familiarity with C programming and the terminal rendering API. For scene structure and life cycle concepts, see [Scene Concepts and Architecture](../Explanation/scenes-architecture.md).

## Step 1: Create the Header File

Create a new header file in `App/Inc/Scenes/` using the naming pattern `scene_<name>.h`. The header at the minimum should include two functions an initialization function and a render function.

The initialization function accepts no parameters and runs once when the scene manager enters the START state. The render function accepts the current frame count, starting at zero, and executes every frame while the scene remains active. Below is an example structure for the header file:

```c
/**
 ******************************************************************************
 * @file           : scene_ascii_glitch.h
 * @brief          : Header for the ASCII glitch scene.
 ******************************************************************************
 */

#ifndef __SCENE_ASCII_GLITCH_H
#define __SCENE_ASCII_GLITCH_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void AsciiGlitchInit(void);
void AsciiGlitchRender(uint32_t scene_frame);

#endif /* __SCENE_ASCII_GLITCH_H */
```

!!! warning "Function signatures are required"
    The initialization function must accept no parameters and the render function must accept a single parameter of type uint32_t representing the current frame count. The scene manager depends on these exact signatures to call the functions correctly.

## Step 2: Create the Source File

Create a new source file in `App/Src/Scenes/` matching the header filename. The source file contains the animation logic, state variables, and helper functions required by the scene.

To keep scenes modular and self contained organize rendering logic into private helper functions to maintain clarity in the render function. Static variables can be used to store animation state and persist across function calls. The scene manager calls the initialization function once per cycle, automatically resetting state.

```c
/**
 ******************************************************************************
 * @file           : scene_ascii_glitch.c
 * @brief          : Implementation of the ASCII glitch scene.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "scene_ascii_glitch.h"
#include "rng_util.h"
#include "terminal.h"
#include "main.h"

/* Private Defines -----------------------------------------------------------*/
#define ASCII_PRINTABLE_START 33   // The '!' character
#define ASCII_CHAR_MASK       0x3F // Mask for 64 printable ASCII characters

#define GLITCH_DENSITY_HIGH   20
#define GLITCH_DENSITY_MEDIUM 4
#define GLITCH_DENSITY_LOW    2

/* Private Variables ---------------------------------------------------------*/
static uint32_t rand_number;

/* Private Functions ---------------------------------------------------------*/
static char GetAsciiChar_(void)
{
    return (rand_number & ASCII_CHAR_MASK) + ASCII_PRINTABLE_START;
}

static void CharacterNoise_(uint32_t frame, uint8_t density_scale)
{
    uint16_t random_col, random_row;
    char char_buffer[2] = {0, '\0'};

    uint32_t spawn_count = (frame % density_scale) + 1;

    for (int i = 0; i < spawn_count; i++)
    {
        XorshiftRandomNumber(&rand_number);
        random_col = (rand_number % TERMINAL_WIDTH) + 1;

        XorshiftRandomNumber(&rand_number);
        random_row = (rand_number % TERMINAL_HEIGHT) + 1;

        XorshiftRandomNumber(&rand_number);
        char_buffer[0] = GetAsciiChar_();

        TerminalPrintString(char_buffer, random_col, random_row);
    }
}

static void CharacterDissolve_(uint32_t frame, uint8_t density_scale)
{
    uint16_t random_col, random_row;
    uint32_t spawn_count = (frame % density_scale) + 1;

    for (int i = 0; i < spawn_count; i++)
    {
        XorshiftRandomNumber(&rand_number);
        random_col = (rand_number % TERMINAL_WIDTH) + 1;

        XorshiftRandomNumber(&rand_number);
        random_row = (rand_number % TERMINAL_HEIGHT) + 1;

        TerminalPrintString(" ", random_col, random_row);
    }
}

/* Public Functions ----------------------------------------------------------*/
void AsciiGlitchInit(void)
{
    rand_number = GetRandomNumber();
    TerminalResetStyle();
}

void AsciiGlitchRender(uint32_t scene_frame)
{
    if (scene_frame < 21)
    {
        CharacterNoise_(scene_frame, GLITCH_DENSITY_HIGH);

        if ((scene_frame & 0x04) == 0)
            CharacterDissolve_(scene_frame, 5);
    }
    else if (scene_frame < 75)
    {
        CharacterNoise_(scene_frame, GLITCH_DENSITY_MEDIUM);
        CharacterDissolve_(scene_frame, 15);
    }
    else
    {
        CharacterNoise_(scene_frame, GLITCH_DENSITY_LOW);
        CharacterDissolve_(scene_frame, 15);
    }
}
```

## Step 3: Add Scene Files to CMake

Update `App/CMakeLists.txt` to include the new scene source file in the build. Locate the scene sources section in the file and add the source:

```cmake
# Scene source files
${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_glitch.c
${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_matrix_rain.c
${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_bouncing_box.c
```

The order in CMakeLists.txt does not affect scene playback order.

## Step 4: Define a Scene Identifier

Add a new identifier to the `SceneID_t` enumeration in `App/Inc/scene_manager.h`. The enumeration defines all available scene identifiers.

Edit `App/Inc/scene_manager.h` and locate the typedef, inserting the new identifier before `SCENE_TOTAL_SCENES`, which must remain the final entry. Follow the naming convention `SCENE_<NAME>` in uppercase.

```c
typedef enum {
    SCENE_ATTRIBUTES_DEMO,
    SCENE_ASCII_GLITCH,    // Add the new identifier

    SCENE_TOTAL_SCENES     // Must remain last
} SceneID_t;
```

## Step 5: Include the Header in Scene Manager

Add an include directive in `App/Src/scene_manager.c` to access the scene functions.

Edit `App/Src/scene_manager.c` and add to the includes section:

```c
#include "scene_ascii_glitch.h"
```

## Step 6: Register in the Scene Table

Because the scene table stores metadata required by the scene manager to locate and execute scenes, an entry must be added to the `scene_table[]` definition.

Define a new configuration entry in the `scene_table[]` array located in `App/Src/scene_manager.c`.

```c
static const SceneConfig_t scene_table[] = {
    {SCENE_ATTRIBUTES_DEMO, ATTRIBUTES_DURATION, SCENE_TRANSITION_CLEAR, SceneAttributesInit, SceneAttributesRender},

    // Add the new scene configuration
    {SCENE_ASCII_GLITCH_NOISE, GLITCH_NOISE_DURATION, SCENE_TRANSITION_CLEAR, GlitchInit, AsciiGlitchRender},
};
```

!!! note "Scene table fields"
    The **scene identifier** maps a scene to its enumeration value. **Duration** defines its execution length (typically 150–400 frames). **Transition types** dictate display behaviour: `SCENE_TRANSITION_CLEAR` wipes the screen prior to the next scene, whereas `SCENE_TRANSITION_NONE` renders over the previous frame for continuous animations. Finally, dedicated function pointers reference the scene's respective **initialization** and **render functions**.

## Step 7: Add to Playlist (Optional)

To include a scene in playlist mode, the scene identifier must be added to the `scene_playlist[]` array located within `App/Src/scene_manager.c`.

When active, playlist mode processes scenes in the exact order specified by this array, adhering to the individual durations defined in the scene table.

This configuration step is unnecessary if the scene is meant solely for auto mode, given that auto mode loops through every scene in the scene table sequentially, independent of playlist inclusion.

```c
static const SceneID_t scene_playlist[] = {
    SCENE_ATTRIBUTES_DEMO,

    SCENE_ASCII_GLITCH,    // Add here to include in playlist
};
```

## Step 8: Build and Test

Compile and build the project to verify all integrations are correct.

If the build succeeds, flash the firmware to the target board. The new scene will appear automatically in the demo cycle.

!!! tip "Testing specific modes"
    Use the shell command `RUN DEMO /MODE=AUTO` to test auto mode or `RUN DEMO /MODE=PLAYLIST` to test playlist mode.

## Verification

To ensure the new scene is integrated correctly, here are key points to verify during testing:

- The scene appears in the correct position within the sequence of all scenes.
- The transition from the previous scene to the new scene behaves as configured. The screen clears if `SCENE_TRANSITION_CLEAR` was specified, or new content renders over the previous frame if `SCENE_TRANSITION_NONE` was specified
- Any rendering within terminal bounds remains visible without wrapping or clipping outside the display area
- The animation maintains smooth, uniform frame timing without stuttering, unexpected pauses, or timing inconsistencies.
- The animation duration feels appropriate for the visual content. If the scene runs too quickly or too slowly, adjust the duration value in the scene table entry and rebuild

## Troubleshooting

Below are common issues that may arise when adding a new scene, along with steps to diagnose and resolve them.

### Undefined Reference to Scene Functions

Verify the source file is listed in `App/CMakeLists.txt` under the scene sources section. Confirm function names in the source file match the declarations in the header file exactly, including capitalization.

### Scene Does Not Appear in the Demo Cycle

Check that the scene identifier is registered in `scene_table[]` in `scene_manager.c` with the correct function pointers. Verify that `SCENE_TOTAL_SCENES` remains the final entry in the `SceneID_t` enumeration. Confirm the identifier spelling matches between the enumeration definition and the scene table entry.

### Scene Appears but Renders at Wrong Position

The scene table order determines play back sequence in auto mode. Verify the scene entry position in `scene_table[]` matches the intended location in the cycle. For playlist mode, check the scene identifier position in `scene_playlist[]`.

### Scene Duration Is Incorrect

Verify the duration value in the scene table entry (third parameter) matches the intended frame count. Remember that duration is measured in frames, not seconds. At approximately 30 FPS, 300 frames equals about 10 seconds. Adjust the duration value and rebuild if the timing does not match expectations.

## See Also

The following documentation provides essential background information regarding scene architecture, rendering strategies, and performance constraints within the demo environment.

- [Scene Concepts and Architecture](../Explanation/scenes-architecture.md) - Explanation of scene life cycle, state management, and design patterns
- [Terminal Rendering Modes](../Reference/terminal-rendering.md) - Buffered versus direct rendering strategies and performance implications
- [UART Bandwidth and Timing](../Explanation/uart-bandwidth.md) - Frame timing constraints and byte budgets for high-speed UART communication
