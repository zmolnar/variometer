/**
 * @file NmeaGeneratorThread.h
 * @brief Thread to generate NMEA messages.
 * @author Molnar Zoltan
*/

#ifndef NMEAGENERATORTHREAD_H
#define NMEAGENERATORTHREAD_H

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
extern char nmea[];
extern semaphore_t nmea_message_sent;

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(NmeaGeneratorThread, arg);

#endif

/******************************* END OF FILE ***********************************/

