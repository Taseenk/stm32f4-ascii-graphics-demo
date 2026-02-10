/**
 ******************************************************************************
 * @file           : dashboard.h
 * @brief          :
 *
 ******************************************************************************
 */

#ifndef __DASHBOARD_H
#define __DASHBOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines -------------------------------------------------------------------*/
#define SYSTEM_TEXT         "[SYSTEM: STM32F407VG]"     // Text to display for system information
#define MAIN_PAGE_TEXT      "[MAIN MENU]"               // Text to display for the dashboard page
#define FPS_TEXT            "[FPS: ??]"                 // Placeholder text for FPS display

#define SYSTEM_TEXT_LEN     sizeof(SYSTEM_TEXT) - 1     // Length of the system text without null terminator
#define MAIN_PAGE_TEXT_LEN  sizeof(MAIN_PAGE_TEXT) - 1  // Length of the main page text without null terminator
#define FPS_TEXT_LEN        sizeof(FPS_TEXT) - 1        // Length of the FPS text without null terminator

#define SYSTEM_TEXT_POSITION    1       // Starting column for system information text
#define FPS_TEXT_POSITION       70      // Starting column for FPS text

/* Function prototypes -------------------------------------------------------*/
void DashboardStatusBar(uint32_t fps_counter);

#endif /* __DASHBOARD_H */