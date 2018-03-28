/**
 * @file ButtonHandler.h
 * @brief Button handler module.
 * @author Molnar Zoltan
 */

#ifndef BUTTONHANDLERTHREAD_H
#define BUTTONHANDLERTHREAD_H

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

/*******************************************************************************/
/* DECLARATION OF GLOBAL VARIABLES                                             */
/*******************************************************************************/
extern semaphore_t shutdownBeepFinishedSemaphore;

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(ButtonHandlerThread, arg);

#endif

/******************************* END OF FILE ***********************************/

