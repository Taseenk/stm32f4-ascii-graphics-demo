/**
 ******************************************************************************
 * @file           : scene_glitch.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __SCENE_GLITCH_H
#define __SCENE_GLITCH_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void GlitchInit(void);
void AsciiGlitchRender(uint32_t scene_frame);
void BinaryGlitchRender(uint32_t scene_frame);

#endif /* __SCENE_GLITCH_H */