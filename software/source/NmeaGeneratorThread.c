/**
 * @file NmeaGeneratorThread.c
 * @brief Thread to generate NMEA messages.
 * @author Zoltan, Molnar
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
struct Lk8ex1_s {
    int32_t p_raw;
    int32_t altitude;
    int32_t vario;
    int32_t temp;
    int32_t battery_voltage;
};

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/
SEMAPHORE_DECL(nmea_message_sent, 0);
char nmea[150] = {0};

static SEMAPHORE_DECL(nmea_sem, 0);
static virtual_timer_t vt;

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
static void vtcb(void *arg)
{
    (void)arg;
    chSemSignal(&nmea_sem);
}

static uint32_t calc_nmea_crc(const char *p, size_t length)
{
    uint32_t crc = 0;

    size_t i;
    for (i = 0; i < length; i++)
        crc ^=p[i];

    return crc;
}

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(NmeaGeneratorThread, arg)
{
    (void)arg;

    chSemObjectInit(&nmea_sem, 0);

    while(1) {
        chSemWait(&nmea_sem);
        chVTSet(&vt, S2ST(1), vtcb, NULL);

        chMtxLock(&dsp_mutex);
        float v = dsp_vario;
        float p = dsp_pfil;
        chMtxUnlock(&dsp_mutex);

        struct Lk8ex1_s data;
        data.p_raw = (int32_t)(p*100);
        data.altitude = 999999;
        data.vario = (int32_t)(v*100);
        data.temp = 99;
        data.battery_voltage = 999;
#if 1
        chsnprintf (nmea, sizeof(nmea),
                "$LK8EX1,%d,%d,%d,%d,%d\r\n",
                data.p_raw,
                data.altitude,
                data.vario,
                data.temp,
                data.battery_voltage);

        uint32_t crc = calc_nmea_crc(nmea+1, strlen(nmea)-1);
        chsnprintf (
                nmea + strlen(nmea),
                sizeof(nmea) - strlen(nmea),
                "*%x\r\n", crc);

        chEvtBroadcastFlags(&serial_event_source, LK8EX1_READY_TO_SEND);
        chSemWait(&nmea_message_sent);
#endif
    }
}


/******************************* END OF FILE ***********************************/

