/**
 ******************************************************************************
 * @file           : scene_credits.h
 * @brief          : Header for scene_credits.c, defining the initialization
 * and rendering functions for the Credits scene. A demoscene-style animated
 * credits card that acts as a bookend for the playlist, sliding each line
 * up into its final position with a staggered reveal effect.
 ******************************************************************************
 */

#ifndef __SCENE_CREDITS_H
#define __SCENE_CREDITS_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void SceneCreditsInit(void);
void SceneCreditsRender(uint32_t scene_frame);

#endif /* __SCENE_CREDITS_H */