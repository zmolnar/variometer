/**
 * @file SimulatorThread.c
 * @brief Thread to simulate processed data.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "SignalProcessorThread.h"
#include "hal.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(SimulatorThread, arg)
{
    (void)arg;

    chEvtObjectInit(&signalProcessorEvent);

    chThdSleepMilliseconds(2000);

    float vario = 0;
    while(1) {
        while(vario < 7) {
            vario += 1.0/100.0;
            chMtxLock(&SignalProcessorMutex);
            SignalProcessingOutputData.vario = vario;
            chMtxUnlock(&SignalProcessorMutex);

            chEvtBroadcastFlags(&signalProcessorEvent, CALCULATION_FINISHED);
            chThdSleepMilliseconds(20);
        }
#if 1
        while((-6) < vario) {
            vario -= 1.0/100.0;
            chMtxLock(&SignalProcessorMutex);
            SignalProcessingOutputData.vario = vario;
            chMtxUnlock(&SignalProcessorMutex);

            chEvtBroadcastFlags(&signalProcessorEvent, CALCULATION_FINISHED);
            chThdSleepMilliseconds(20);
        }
#endif
    }
}


/******************************* END OF FILE ***********************************/

