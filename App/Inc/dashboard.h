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