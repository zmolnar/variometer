/**
 * @file SignalProcessorThread.c
 * @brief Thread to perform processing of raw pressure data.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "PressureReaderThread.h"
#include "SignalProcessorThread.h"
#include "chprintf.h"
#include "hal.h"

#include <math.h>
#include <stdint.h>

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/
#define ALPHA                                                               (0.2)
#define BETA                                                              (0.004)
#define BUFLENGTH                                                             100

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/
MUTEX_DECL(SignalProcessorMutex);
EVENTSOURCE_DECL(signalProcessorEvent);

struct SignalProcessingOutputData_s SignalProcessingOutputData;

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
static void waitForMeasurementData(struct PressureData_s *pdata) {
    thread_t *psender = chMsgWait();
    msg_t msg = chMsgGet(psender);
    *pdata = *(struct PressureData_s *)msg;
    chMsgRelease(psender, 0);
}

static systime_t calculateElapsedTimeInMs(systime_t t1, systime_t t2) {
    systime_t dt = t2 - t1;
    if (t2 < t1)
        dt += (systime_t)(-1);

    return ST2MS(dt);
}

static float convertPressureToAltitude(float pressure) {
    return 44330 * (1 - pow ((pressure / 101325.0), 0.1902));
}

static float ab_filter(
        float alpha,
        float beta,
        float *xk_1,
        float *vk_1,
        int32_t x_raw,
        float samplingTime) {
    float xk = *xk_1 + (*vk_1 * samplingTime);
    float vk = *vk_1;

    float rk = x_raw - xk;

    xk += alpha * rk;
    vk += beta * rk / samplingTime;

    *xk_1 = xk;
    *vk_1 = vk;

    return xk;
}

static float calculateSlope(
        float *buffer,
        size_t bufferLength,
        size_t startIndex,
        size_t sampleCount,
        float dt) {
    float x_avg = 0;
    float y_avg = 0;
    size_t i;
    for (i = 0; i < sampleCount; i++) {
        size_t j;
        if (startIndex + i < bufferLength)
            j = startIndex + i;
        else
            j = i - bufferLength + startIndex;

        x_avg += i * dt;
        y_avg += buffer[j];
    }

    x_avg /= sampleCount;
    y_avg /= sampleCount;

    float num = 0;
    float den = 0;
    for (i = 0; i < sampleCount; i++) {
        size_t j;
        if (startIndex + i < bufferLength)
            j = startIndex + i;
        else
            j = i - bufferLength + startIndex;
        num += (i*dt - x_avg) * (buffer[j] - y_avg);
        den += (i*dt - x_avg) * (i*dt - x_avg);
    }

    return num/den;
}

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(SignalProcessorThread, arg)
{
    (void)arg;

    chEvtObjectInit(&signalProcessorEvent);

    while (1) {
        static float lastPressure = 0;
        static float lastPressureChangingSpeed = 0;
        static systime_t lastTimestamp = 0;
        static float altitudeBuffer[BUFLENGTH] = {0};
        static size_t sampleCount = 0;
        static size_t sampleIndex = 0;

        struct PressureData_s rawData;
        waitForMeasurementData(&rawData);

        if (0 == sampleCount) {
            lastPressure = rawData.pressure;
            lastTimestamp = rawData.timestamp;
            lastPressureChangingSpeed = 0;
            sampleCount++;
            continue;
        }

        float rawPressure = rawData.pressure;
        systime_t actualTimestamp = rawData.timestamp;
        systime_t samplingTime = calculateElapsedTimeInMs(lastTimestamp, actualTimestamp);

        float filteredPressure = ab_filter(
                ALPHA,
                BETA,
                &lastPressure,
                &lastPressureChangingSpeed,
                rawPressure,
                samplingTime);

        float altitude = convertPressureToAltitude(filteredPressure);

        altitudeBuffer[sampleIndex++ % BUFLENGTH] = altitude;

        if (sampleCount < BUFLENGTH) {
            sampleCount++;
            continue;
        }

        float vario = calculateSlope(
                altitudeBuffer,
                BUFLENGTH,
                sampleIndex % BUFLENGTH,
                sampleCount,
                samplingTime / 1000.0);

        lastTimestamp = rawData.timestamp;

        chMtxLock(&SignalProcessorMutex);
        SignalProcessingOutputData.vario = vario;
        SignalProcessingOutputData.baroAltitude = altitude;
        SignalProcessingOutputData.filteredPressure = filteredPressure;
        chMtxUnlock(&SignalProcessorMutex);

        chEvtBroadcastFlags(&signalProcessorEvent, CALCULATION_FINISHED);
    }
}

/******************************* END OF FILE ***********************************/

