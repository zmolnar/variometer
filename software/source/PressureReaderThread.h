/**
 * @file PressureReaderThread.h
 * @brief Thread that periodically reads measurement data from MS5611 sensor. 
 * @author Molnar Zoltan
 */

#ifndef PRESSUREREADERTHREAD_H
#define PRESSUREREADERTHREAD_H

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
struct PressureData_s {
    systime_t timestamp;
    uint32_t pressure;
    int32_t temperature;
};

/*******************************************************************************/
/* DECLARATION OF GLOBAL VARIABLES                                             */
/*******************************************************************************/

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(PressureReaderThread, arg);

#endif

/******************************* END OF FILE ***********************************/

