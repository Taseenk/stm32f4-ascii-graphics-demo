/**
 ******************************************************************************
 * @file           : scene_xterm_palette.h
 * @brief          : Header for scene_xterm_palette.c, defining the
 * initialization and render functions for the xterm-256 colour palette
 * test card. Exposes the full 256-colour space as a static single-frame
 * scene: the 216-colour RGB cube and the 24-step grayscale ramp.
 ******************************************************************************
 */

#ifndef __SCENE_XTERM_PALETTE_H
#define __SCENE_XTERM_PALETTE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void SceneXtermPaletteInit(void);
void SceneXtermPaletteRender(uint32_t scene_frame);

#endif /* __SCENE_XTERM_PALETTE_H */