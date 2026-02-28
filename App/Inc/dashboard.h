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
// Dashboard Status Bar Text
#define SYSTEM_TEXT         "[SYSTEM: STM32F407VG]"     // Text to display for system information
#define MAIN_PAGE_TEXT      "[MAIN MENU]"               // Text to display for the dashboard page
#define FPS_TEXT            "[FPS: ??]"                 // Placeholder text for FPS display

// Dashboard Status Bar Text sizes
#define STATUS_BAR_BUFFER_SIZE  (TERMINAL_WIDTH + ANSI_TEXT_LEN + 1)    // Total size of the status bar buffer, including ANSI escape codes and null terminator
#define ANSI_TEXT_LEN           8                                       // Length of the ANSI escape codes used for setting and resetting styling
#define SYSTEM_TEXT_LEN         sizeof(SYSTEM_TEXT) - 1                 // Length of the system text without null terminator
#define MAIN_PAGE_TEXT_LEN      sizeof(MAIN_PAGE_TEXT) - 1              // Length of the main page text without null terminator
#define FPS_TEXT_LEN            sizeof(FPS_TEXT) - 1                    // Length of the FPS text without null terminator

// Dashboard Status Bar Text positions
#define SYSTEM_TEXT_POSITION        1       // Starting column for system information text
#define MAIN_PAGE_TEXT_POSITION     34      // Middle of she screen
#define FPS_TEXT_POSITION           70      // Starting column for FPS text
#define FPS_VALUE_POSITION          77      // Position immediately after the FPS label

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
void MainPageInit(void);

void DashboardHeader(void);
void DashboardMenuList(void);
void DashboardFooter(void);

void DashboardFPSRefresh(uint32_t fps, uint8_t fps_range);

void DashboardMenuSelection(uint32_t global_frame);

#endif /* __DASHBOARD_H */