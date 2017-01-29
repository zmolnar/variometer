/**
 * @file SignalProcessorThread.h
 * @brief Thread to perform processing of raw pressure data.
 * @author Zoltán, Molnár
*/

#ifndef SIGNALPROCESSORTHREAD_H
#define SIGNALPROCESSORTHREAD_H

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
struct SignalProcessingOutputData_s {
    float vario;
    float baroAltitude;
    float filteredPressure;
};

typedef enum {
    CALCULATION_FINISHED = (1 << 0)
} SignalProcessorEventFlags_t;

/*******************************************************************************/
/* DECLARATION OF GLOBAL VARIABLES                                             */
/*******************************************************************************/
extern mutex_t SignalProcessorMutex;
extern struct SignalProcessingOutputData_s SignalProcessingOutputData;
extern event_source_t signalProcessorEvent;

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
THD_FUNCTION(SignalProcessorThread, arg);

#endif

/******************************* END OF FILE ***********************************/

