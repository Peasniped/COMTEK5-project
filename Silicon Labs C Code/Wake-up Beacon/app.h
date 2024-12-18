/***************************************************************************//**
 * @file
 * @brief app_process.h
 *******************************************************************************/
#ifndef APP_PROCESS_H
#define APP_PROCESS_H

#include "rail.h"

/**************************************************************************//**
 * The function is used for Application logic.
 *
 * @param rail_handle RAIL handle
 * @returns None
 *
 * The function is used for Application logic.
 * It is called infinitely.
 *****************************************************************************/

void app_init(void);

void app_process_action(void);

void sendWakeUp(void);

void flashErrorCode(int error_code);

#endif  // APP_PROCESS_H
