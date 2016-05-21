/**
 * @file PressureReaderThread.c
 * @brief Thread that periodically reads measurement data from MS5611 sensor. 
 * @author Zoltán Molnár
 * @date Wed Dec 23 15:24:40 2015 (+0100)
 * Version: 
 * Last-Updated: Sun Jan 17 18:21:08 2016 (+0100)
 *           By: Molnár Zoltán
*/

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "PressureReaderThread.h"
#include "ms5611.h"

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
extern thread_t *pSignalProcessorThread;

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/


/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/


/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(PressureReaderThread, arg)
{
        (void)arg;
        chRegSetThreadName("PressureReaderThread");   
        
        MS5611_Init ();
        MS5611_Start ();
        
        while (1) {
                struct PressureData_s data = {0};
                MS5611_Measure (&data.p_raw, &data.t_raw);
                data.t = chVTGetSystemTime();
                chMsgSend (pSignalProcessorThread, (msg_t)&data);
        }
}


/******************************* END OF FILE ***********************************/
