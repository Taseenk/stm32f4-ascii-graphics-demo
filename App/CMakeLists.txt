cmake_minimum_required(VERSION 3.22)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

add_library(AppModule STATIC "")

# Project sources to executable
target_sources(AppModule PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/terminal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/serial_hw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/dashboard.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/scene_manager.c

    # Utility source files
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/rng_util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/shell.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/shell_strings.c

    # Scene source files
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_glitch.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_matrix_rain.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_colour_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_attributes_demo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_credits.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Scenes/scene_xterm_palette.c
)

# Project include paths
target_include_directories(AppModule PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/Scenes
)

target_include_directories(AppModule PRIVATE
    # Project STM32CubeMX generated include paths
    # Add the missing CMSIS DSP path explicitly for arm_math.h
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/DSP/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

# Project symbols (macros)
target_compile_definitions(AppModule PRIVATE 
    ARM_MATH_CM4    # Required for arm_math.h on STM32F4
)

target_link_libraries(AppModule PUBLIC
    # Project STM32CubeMX generated code
    stm32cubemx
)