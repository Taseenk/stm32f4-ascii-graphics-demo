/**
 ******************************************************************************
 * @file           : dashboard.h
 * @brief          : Header file for the dashboard module of the STM32F4 ASCII graphics 
 * demo application, defining the interface and data structures for managing the dashboard 
 * UI, including page tracking and menu selection functionality.
 ******************************************************************************
 */

#ifndef __DASHBOARD_H
#define __DASHBOARD_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* typedefs ------------------------------------------------------------------*/
// Dashboard pages for tracking the current page being displayed in the dashboard menu
typedef enum {
    DASHBOARD_HELP,
    DASHBOARD_AUTO,
    DASHBOARD_PLAYLIST,
    DASHBOARD_INFO,
    DASHBOARD_QUIT,
    DASHBOARD_TOTAL_PAGES
} DashboardPages_t;

extern DashboardPages_t current_page;   // Global variable to track the current dashboard page being displayed (e.g., help, auto, playlist, etc.)

/* Function prototypes -------------------------------------------------------*/
// Initializes the dashboard UI by clearing the terminal and rendering the header, menu list, and footer.
void MainPageInit(void);

// Renders the top status bar containing system info, page title, and FPS labels.
void DashboardHeader(void);

// Prints the static list of menu options (Help, Auto, Playlist, etc.) to the terminal.
void DashboardMenuList(void);

// Renders the bottom navigation guide and copyright information.
void DashboardFooter(void);

// Updates only the numeric FPS value in the header; includes range validation.
void DashboardFPSRefresh(uint32_t fps, uint8_t fps_range);

//Handles the visual blinking effect of the selected menu item and manages state transitions for "Auto" and "Playlist" modes.
void DashboardMenuSelection(uint32_t global_frame);

#endif /* __DASHBOARD_H */