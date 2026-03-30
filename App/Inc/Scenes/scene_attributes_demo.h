/**
 ******************************************************************************
 * @file           : scene_attributes_demo.h
 * @brief          : Header for scene_attributes_demo.c.defining the initialization
 * and rendering functions for the VT100 attribute torture test. A showcase
 * of every supported ANSI text attribute and their combinations.
 ******************************************************************************
 */

#ifndef __SCENE_ATTRIBUTES_DEMO_H
#define __SCENE_ATTRIBUTES_DEMO_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Function prototypes -------------------------------------------------------*/
void SceneAttributesInit(void);
void SceneAttributesRender(uint32_t scene_frame);

#endif /* __SCENE_ATTRIBUTES_DEMO_H */