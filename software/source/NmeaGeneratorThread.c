/**
 * @file NmeaGeneratorThread.c
 * @brief Thread to generate NMEA messages.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "NmeaGeneratorThread.h"
#include "SignalProcessorThread.h"
#include "SerialHandlerThread.h"
#include "chprintf.h"
#include "string.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/
struct NmeaData_s {
    float baroAltitude;
    float vario;
};

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/
SEMAPHORE_DECL(nmeaMessageSent, 0);
char nmea[150];

EVENTSOURCE_DECL(nmeaMessageReady);

static struct NmeaData_s nmeaData;

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
static uint32_t calculateCrc(const char *pdata, size_t length)
{
    uint32_t crc = 0;
    size_t i;
    for (i = 0; i < length; i++)
        crc ^= pdata[i];

    return crc;
}

static void readMeasurementData(void) {
    chMtxLock(&SignalProcessorMutex);
    nmeaData.baroAltitude = SignalProcessingOutputData.baroAltitude;
    nmeaData.vario = SignalProcessingOutputData.vario;
    chMtxUnlock(&SignalProcessorMutex);
}

static void createNmeaMessage(void) {
    memset(nmea, 0, sizeof(nmea));
    chsnprintf(nmea, sizeof(nmea), "$LXWP0,N,,%.2f,%.2f,,,,,,,,",
            nmeaData.baroAltitude, nmeaData.vario);
    uint32_t crc = calculateCrc(nmea+1, strlen(nmea)-1);
    chsnprintf(nmea + strlen(nmea), sizeof(nmea) - strlen(nmea), "*%02X", crc);

    nmea[strlen(nmea)] = '\0';
}

static void sendMessage(void) {
    chEvtBroadcast(&nmeaMessageReady);
}

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(NmeaGeneratorThread, arg)
{
    (void)arg;

    chSemObjectInit(&nmeaMessageSent, 0);

    while(1) {
        chThdSleepMilliseconds(1000);
        readMeasurementData();
        createNmeaMessage();
        sendMessage();
        chSemWait(&nmeaMessageSent);
    }
}


/******************************* END OF FILE ***********************************/

