/* SerialHandlerThread.c --- 
 * 
 * @file SerialHandlerThread.c
 * @brief Thread to handle serial communication interfaces.
 * @author Molnár Zoltán
 * @date Mon May 16 21:08:09 2016 (+0200)
 * Version: 1.0.0
<<<<<<< HEAD
 * Last-Updated: Sun Aug 28 15:56:43 2016 (+0200)
=======
 * Last-Updated: Mon Aug 22 19:07:12 2016 (+0200)
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
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
EVENTSOURCE_DECL(serial_event_source);

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(SerialHandlerThread, arg)
{
        (void)arg;

        /* Start serial interface to Kobo.*/
        static SerialConfig kobocfg = {9600,0,0,0};
        sdStart (&SD1, &kobocfg);

        /* Start serial interface to GPS module.*/
        static SerialConfig gpscfg  = {9600,0,0,0};
        sdStart (&SD2, &gpscfg);

        event_listener_t gps_listener;
        eventflags_t flags;
        chEvtRegisterMaskWithFlags((event_source_t *)chnGetEventSource(&SD2), 
                                   &gps_listener, 
<<<<<<< HEAD
                                   EVENT_MASK(0),
=======
                                   EVENT_MASK(SERIAL_EVENT_GPS_DATA_RECEIVED),
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
                                   CHN_INPUT_AVAILABLE);

        event_listener_t serial_listener;
        chEvtRegisterMask (&serial_event_source, 
                           &serial_listener, 
<<<<<<< HEAD
                           EVENT_MASK(0));
=======
                           EVENT_MASK(SERIAL_EVENT_LK8EX1_READY));
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299

        while (1) {
                eventmask_t evt = chEvtWaitAny(ALL_EVENTS);
                                
<<<<<<< HEAD
                if (evt & EVENT_MASK(0)) {
=======
                if (evt & EVENT_MASK(SERIAL_EVENT_GPS_DATA_RECEIVED)) {
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
                        flags = chEvtGetAndClearFlags(&gps_listener);
                        if (flags & CHN_INPUT_AVAILABLE)
                        {
                                msg_t c;
                                do
                                {
                                        c = chnGetTimeout(&SD2, TIME_IMMEDIATE);
                                        if ( c != STM_TIMEOUT )
                                                chprintf((BaseSequentialStream*)&SD1, "%c", (char)c);
                                } 
                                while (c != STM_TIMEOUT);
                        } 
                }
<<<<<<< HEAD
                if (evt & EVENT_MASK(1)) {
=======
                if (evt & EVENT_MASK(SERIAL_EVENT_LK8EX1_READY)) {
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
                        chprintf((BaseSequentialStream*)&SD1, "\n\rLK8EX1 ready\n\r");
                }
        }
}







/******************************* END OF FILE ***********************************/

