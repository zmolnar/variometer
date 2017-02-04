/**
 * @file SerialHandlerThread.c
 * @brief Thread to handle serial communication interfaces.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "NmeaGeneratorThread.h"
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

    /* Start serial interface to Kobo.*/
    static SerialConfig koboConfig = {9600,0,0,0};
    sdStart(&SD1, &koboConfig);

    /* Start serial interface to GPS module.*/
    static SerialConfig gpsConfig  = {9600,0,0,0};
    sdStart(&SD2, &gpsConfig);

    event_listener_t gpsListener;
    eventflags_t flags;

    chEvtRegisterMaskWithFlags(
            (event_source_t *)chnGetEventSource(&SD2),
            &gpsListener,
            EVENT_MASK(0),
            CHN_INPUT_AVAILABLE);

    event_listener_t nmeaListener;
    chEvtRegisterMask(&nmeaMessageReady, &nmeaListener, EVENT_MASK(1));

    while (1) {
        eventmask_t evt = chEvtWaitAny(ALL_EVENTS);

        if (evt & EVENT_MASK(0)) {
            flags = chEvtGetAndClearFlags(&gpsListener);
            if (flags & CHN_INPUT_AVAILABLE) {
                msg_t c;
                do {
                    c = chnGetTimeout(&SD2, TIME_IMMEDIATE);
                    if ( c != STM_TIMEOUT )
                        chprintf((BaseSequentialStream*)&SD1, "%c", (char)c);
                }
                while (c != STM_TIMEOUT);
            }
        }
        if (evt & EVENT_MASK(1)) {
            chprintf((BaseSequentialStream*)&SD1, "%s\n\r", nmea);
            chSemSignal(&nmeaMessageSent);
        }
    }
}

/******************************* END OF FILE ***********************************/

