/* SerialHandlerThread.c --- 
 * 
 * @file SerialHandlerThread.c
 * @brief Thread to handle serial communication interfaces.
 * @author Molnár Zoltán
 * @date Mon May 16 21:08:09 2016 (+0200)
 * Version: 1.0.0
 * Last-Updated: Tue May 17 22:13:04 2016 (+0200)
 *           By: Molnár Zoltán
 * 
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "SerialHandlerThread.h"
#include "hal.h"
#include "chprintf.h"

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
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(SerialHandlerThread, arg)
{
        (void)arg;

        event_listener_t gps_data_listener;
        eventflags_t flags;
        chEvtRegisterMaskWithFlags((event_source_t *)chnGetEventSource(&SD2), 
                                   &gps_data_listener, EVENT_MASK(1),
                                   CHN_INPUT_AVAILABLE);



        while (1) {
                chEvtWaitOne(EVENT_MASK(1));
                flags = chEvtGetAndClearFlags(&gps_data_listener);
                
                if (flags & CHN_INPUT_AVAILABLE)
                {
                        msg_t charbuf;
                        do
                        {
                                charbuf = chnGetTimeout(&SD2, TIME_IMMEDIATE);
                                if ( charbuf != STM_TIMEOUT )
                                {
                                        chprintf((BaseSequentialStream*)&SD1, "%c", (char)charbuf);
                                }
                        } 
                        while (charbuf != STM_TIMEOUT);
                } 
        }
}


/******************************* END OF FILE ***********************************/

