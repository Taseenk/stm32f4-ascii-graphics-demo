/**
 ******************************************************************************
 * @file           : scene_credits.h
 * @brief          : Header for scene_credits.c, This scene implements a vertical
 * motion crawl where text elements originate from the bottom edge and move upward
 * to fixed layout coordinates. Includes support for staggered entry timings to separate
 * headers from body text, culminating in a static final billboard display.
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