# Scene Concepts and Architecture

This document provides the foundational concepts and architecture behind scenes in the STM32F4 ASCII Graphics Demo. It covers what a scene is, how scenes are structured, how they integrate with the scene manager, and the different playback modes available. This information is essential for understanding how to create new scenes and how they operate within the overall demo environment.

## What Is a Scene?

A scene is modular and independent visual animation or display that runs within the demo. Scenes render ASCII art to a 80x24 terminal display using colour, text attributes (bold, underline, blink, inverse, dim, strike through), and cursor positioning to create effects ranging from static frames to animate sequences.

Each scene operates independently from others. The scene manager automatically cycles through scenes in order, handles timing and state transitions, and resets the terminal environment between scenes. This separation allows scenes to focus solely on animation logic without worrying about scene life cycle management or shared states.

Scenes have two primary requirements. They must render the output to the terminal, run for a configurable duration before transitioning to the next scene and can only use the provided terminal API for output. Beyond that, there are no restrictions on how scenes are implemented, allowing for a wide variety of visual effects and animation styles.

## Scene Structure

Scenes primarily consist of two files, a header file containing function prototypes and a source file containing the animation logic. The following table provides the specific directory locations and purposes for these files.

| File | Location | Purpose |
| --- | --- | --- |
| scene_myeffect.h | App/Inc/Scenes/ | Function declarations |
| scene_myeffect.c | App/Src/Scenes/ | Animation logic and implementation |

!!! tip "File Naming Convention"
    The file naming convention in this project is `scene_<name>.h` and `scene_<name>.c`, where `<name>` describes the scene (for example, `scene_matrix_rain.h` and `scene_matrix_rain.c`).

Scenes follow a strict functional architecture to manage the flow. The following table outlines the specific roles and execution timing for these components.

| Function Type | Name | Frequency | Purpose |
| --- | --- | --- | --- |
| Public | init() | Once | Scene setup and initialization |
| Public | render() | Every Frame | Core loop for drawing and updates |
| Private | Helpers | As Needed | Modular logic (e.g., drawBorder, updateElements) |

Each scene requires exactly two public functions, an init function and a render function. Private helper functions within the scene file organize rendering logic into logical modules. For example, a scene might have separate helper functions to draw a border, update animated elements, and render text. This organization keeps the code modular and maintainable.

### Scene Functions: Init

The init function receives no frame information and executes only a single time when the scene manager transitions into the START state. Typical use cases are static layout elements that remain constant across frames. While typical operations are clearing the display, rendering fixed text, and setting the initial values for scene-local variables.

### Scene Functions: Render

The render function executes every frame while the scene remains active. During execution, the function constantly receives the **scene_frame** count, which begins at zero. This counter serves as the foundation for all animations, providing the necessary data for timing, position calculations, and conditional rendering logic. Since this code runs continuously, maintaining high performance and efficiency remains a top priority.

## Scene Manager Integration

The scene manager operates a lookup table of registered scenes and automatically cycles through them based on the selected mode. It manages scene timing via the frame counter, handles state transitions (START to RUN to EXIT), and resets the terminal between scenes. The manager also handles switching between auto and playlist modes. The following table details the core files and their specific roles within the scene manager architecture:

| File | Location | Purpose |
| :--- | :--- | :--- |
| scene_manager.h | App/Inc/ | Enum definitions |
| scene_manager.c | App/Src/ | Registration in table and playlist |

Scenes depend on the scene manager to call init and render at the correct times and handle state transitions automatically. The scene manager depends on correct function signatures and proper registration.

!!! note "Scene and Manager Separation of Concerns"
    Scenes are designed to be independent and reusable, while the manager provides the necessary infrastructure for scene execution and life cycle management.

### Scene Configuration Structure

The scene manager makes use of the **SceneConfig_t structure** to store metadata for every scene. This structure contains the scene identifier, the duration measured in frames, the transition style, and function pointers for both the init and render functions. See the table below for a breakdown of the scene field properties:

| Field | Description |
| :--- | :--- |
| id | A unique identification number assigned to each specific scene |
| duration | The total count of frames a scene runs before the next sequence starts |
| screen_transition | Controls how the display behaves when switching between active scenes |
| init | A reference to the public function that handles scene setup |
| render | A reference to the public function responsible for drawing the scene |

???+ "Scene Configuration Structure in scene_manager.h"
    ```c
    typedef struct {
        SceneID_t id;                        // Scene identifier
        uint32_t duration;                   // Duration of the scene in frames
        SceneTransition_t screen_transition; // Type of screen transition effect
        void (*init)(void);                  // Initialization function pointer
        void (*render)(uint32_t scene_frame); // Render function pointer
    } SceneConfig_t;
    ```

### Scene Life Cycle

The scene manager controls the life cycle of each scene, which consists of three main states: START, RUN, and EXIT. This life cycle ensures predictable scene behaviour and allows the scene manager to handle common tasks like screen clearing and colour resetting without duplicating code across all scenes. The following table describes each state and its purpose within the scene life cycle.

| Phase | Action | Timing | Responsibilities |
| --- | --- | --- | --- |
| START | Initialization | Once at start | Manager resets frame counter while scene renders static layout |
| RUN | Active | Every frame | Executes animation logic and increments the counter |
| EXIT | Cleanup | At duration limit | Resets terminal styling and prepares the next scene |

The **START** phase serves as the initialization point where the scene manager calls the init function once. During this phase, the manager internally resets the scene frame counter to zero. The scene init function focuses on visual setup, rendering static elements like background text or borders, and initializing local variables.

The **RUN** phase represents the active state where the render function is called repeatedly. It makes use of the scene frame counter for animation timing and position calculations until the specified duration is met.

The **EXIT** phase handles the cleanup automatically once the frame counter reaches the configured limit. The manager resets terminal styling to default values so the scene itself does not need to manage cleanup logic.

#### Understanding Transition Types

Transition types determine the behaviour of the display during the movement from one scene to the next. The **SceneTransition_t enum** within `scene_manager.h` defines two distinct styles. See the table below for a comparison of the transition types:

| Transition Type | Behaviour | Use Case |
| :-- | :-- | :-- |
| SCENE_TRANSITION_CLEAR | Wipes the screen and buffer before the next scene starts | Standard scenes requiring a clean slate |
| SCENE_TRANSITION_NONE | Immediate switch where the new scene renders over old content | Continuous animations like rain or scrolling text |

**SCENE_TRANSITION_CLEAR** is the standard choice for most scenarios by wiping the display and resetting styles before a new scene begins. Ensuring a clean start for scenes with unique layouts.

In contrast, **SCENE_TRANSITION_NONE** keeps the previous frame visible as the next scene starts rendering. This supports visual continuity for specific implementations that would benefit from the previous frame's content remaining on screen.

???+ "SceneTransition_t Enumeration in scene_manager.h"
    ```c
    typedef enum {
        SCENE_TRANSITION_NONE,  // Instant switch with no visual effect
        SCENE_TRANSITION_CLEAR, // Clear the screen and buffer before next scene
        SCENE_TOTAL_TRANSITIONS // Total number of available transition types
    } SceneTransition_t;
    ```

### Play back Modes

The demo supports two playback modes that determine how scenes are selected and cycled. The following table highlights the differences in scene ordering and duration management between these modes.

| Mode | Scene Order | Duration | Registration |
| --- | --- | --- | --- |
| Auto | Enum sequence | Overridden to fixed 300 frames | scene_table array |
| Playlist | Custom array | Individually configured | scene_playlist array |

Auto Mode ensures that all available scenes run in the order defined by the scene_table array in scene_manager.c. While the scene_table contains individual duration values, Auto Mode overrides these settings and runs each scene for a fixed duration of 300 frames. This is approximately 10 seconds at 30 fps. When the last scene finishes, the system returns to the CLI shell and user input mode.

!!! note "Scene Registration in Auto Mode"
    For a scene to appear in auto mode, it must first be registered in the `scene_table` array in `scene_manager.c`. The table order determines the playback sequence. The scene_table is the lookup mechanism the scene manager uses to find scenes by ID.

!!! example "Scene registration using the Attributes Demo"
    ```c
    static const SceneConfig_t scene_table[] = {
        {
            .id = SCENE_ATTRIBUTES_DEMO,
            .duration = 400,
            .screen_transition = SCENE_TRANSITION_CLEAR,
            .init = SceneAttributesInit,
            .render = SceneAttributesRender
        },
    }
    ```

Playlist Mode allows scenes to run in the order specified by the scene_playlist array in scene_manager.c. Each scene runs for its individually configured duration from the scene_table array. Terminal reset behaviour in this mode depends on the transition type. A reset occurs if the transition is set to SCENE_TRANSITION_CLEAR. If set to SCENE_TRANSITION_NONE, the terminal does not clear. The system returns to the CLI shell upon completion of the final scene. This mode provides flexibility for custom scene sequences and durations.

!!! note "Scene Registration in Playlist Mode"
    To include a scene in playlist mode, it must be added to the `scene_playlist` array in `scene_manager.c`. The playlist mode uses this array to determine the order.

!!! example "Scene registration in playlist mode"
    ```c
    static const SceneID_t scene_playlist[] = {
        SCENE_ATTRIBUTES_DEMO,
        SCENE_SMPTE_CALIBRATION,
    };
    ```

### Scene Identifiers

Scene identifiers are enumeration values defined in the **SceneID_t enum** in `scene_manager.h`. Each scene must have a unique identifier. The identifier serves as the lookup key when the scene manager searches the scene_table to find a scene's configuration. When adding a new scene, insert the identifier before `SCENE_TOTAL_SCENES`:

???+ Scene_ID_t Enumeration in scene_manager.h
    ```c
    typedef enum {
        SCENE_ATTRIBUTES_DEMO,
        // ... existing scenes ...
        SCENE_MYEFFECT,        // The new scene
        SCENE_TOTAL_SCENES     // Must remain last
    } SceneID_t;
    ```

!!! none "Naming conventions for scene identifiers"
    Scene identifiers follow the naming convention `SCENE_<NAME>` where `<NAME>` describes the scene in uppercase (for example, `SCENE_MATRIX_RAIN`, `SCENE_ATTRIBUTES_DEMO`). The enum must end with `SCENE_TOTAL_SCENES`, which is used internally by the manager.

## See Also

Scenes depend on the terminal.h API for all output operations. The terminal module provides cursor positioning, text rendering, colour control, and attribute management. Reference documentation for these capabilities and bandwidth constraints is available in the technical reference section.

- [Add a New Scene](../How-to/add-a-scene.md) — Step-by-step guide to create and register a new scene
- [Screen & Cursor Control](../Reference/terminal-screen-cursor.md) — Positioning and clearing the display
- [Rendering Modes & Graphics](../Reference/terminal-rendering.md) — Direct and buffered rendering strategies
- [Bandwidth and Timing](../Reference/uart-bandwidth-timing.md) — Frame timing constraints and byte budgets
- [Shell Command Reference](../Reference/shell-commands.md) — Running scenes from the command line
