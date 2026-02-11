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
// Dashboard Status Bar Text and positions
#define SYSTEM_TEXT         "[SYSTEM: STM32F407VG]"     // Text to display for system information
#define MAIN_PAGE_TEXT      "[MAIN MENU]"               // Text to display for the dashboard page
#define FPS_TEXT            "[FPS: ??]"                 // Placeholder text for FPS display

#define SYSTEM_TEXT_LEN     sizeof(SYSTEM_TEXT) - 1     // Length of the system text without null terminator
#define MAIN_PAGE_TEXT_LEN  sizeof(MAIN_PAGE_TEXT) - 1  // Length of the main page text without null terminator
#define FPS_TEXT_LEN        sizeof(FPS_TEXT) - 1        // Length of the FPS text without null terminator

#define SYSTEM_TEXT_POSITION        1       // Starting column for system information text
#define MAIN_PAGE_TEXT_POSITION     ((TERMINAL_WIDTH / 2) - (MAIN_PAGE_TEXT_LEN / 2))    // Middle of the screen
#define FPS_TEXT_POSITION           70      // Starting column for FPS text

// Dashboard Main Menu Option Texts and positions
#define HELP_TEXT       " ?       HELP    -  Get help on using the application "
#define SELECT_TEXT     " S       SELECT  -  Select a scene to load "
#define AUTO_TEXT       " A       AUTO    -  Cycle through scenes "
#define INFO_TEXT       " I       INFO    -  About the application and system information "
#define QUIT_TEXT       " Q       QUIT    -  Exit the application "

#define OPTIONS_COL_POSITION        10      // Starting column for the dashboard option texts

// Row positions for each Main Menu option
#define OPTIONS_ROW_OFFSET          3       // Number of rows to space between each dashboard option
#define HELP_ROW_POSITION           5
#define SELECT_ROW_POSITION         (HELP_ROW_POSITION + OPTIONS_ROW_OFFSET)        
#define AUTO_ROW_POSITION           (SELECT_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define INFO_ROW_POSITION           (AUTO_ROW_POSITION + OPTIONS_ROW_OFFSET)
#define QUIT_ROW_POSITION           (INFO_ROW_POSITION + OPTIONS_ROW_OFFSET)

/* Function prototypes -------------------------------------------------------*/
void DashboardInit(void);

// Functions to render different parts of the dashboard
void DashboardStatusBar(void);
void DashboardMainBody(void);

void DashboardFPSUpdater(uint32_t fps_counter);

#endif /* __DASHBOARD_H */