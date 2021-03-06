/**
 * @file SerialHandlerThread.h
 * @brief Thread to handle serial communication interfaces.
 * @author Molnar Zoltan
 */

#ifndef SERIALHANDLERTHREAD_H
#define SERIALHANDLERTHREAD_H

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ch.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/
typedef enum {
    LK8EX1_READY_TO_SEND = (1 << 0)
} SerialEvent_t;

/*******************************************************************************/
/* DECLARATION OF GLOBAL VARIABLES                                             */
/*******************************************************************************/
extern event_source_t serialEvent;

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(SerialHandlerThread, arg);

#endif

/******************************* END OF FILE ***********************************/

