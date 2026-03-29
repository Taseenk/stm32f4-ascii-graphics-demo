/**
 ******************************************************************************
 * @file           : scene_colour_demo.h
 * @brief          : Header file for the Colour Demo scene, defining the
 * initialization and rendering functions for terminal colour capabilities,
 * specifically SMPTE test patterns and background colour cycling.
 ******************************************************************************
 */

#ifndef __SCENE_COLOUR_DEMO_H
#define __SCENE_COLOUR_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void ColourDemoInit(void);
void SmpteCalibrationRender(uint32_t scene_frame);
void RadialGreyscaleRender(uint32_t scene_frame);
void RadialColourRender(uint32_t scene_frame);

#endif /* __SCENE_COLOUR_DEMO_H */