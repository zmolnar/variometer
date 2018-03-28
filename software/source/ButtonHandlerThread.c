/**
 * @file ButtonHandler.c
 * @brief Button handler module.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ch.h"
#include "hal.h"
#include "BeepControlThread.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/
#define STEP_VOLUME_MIN                                                        50
#define STEP_VOLUME_MAX                                                      1000
#define SHUTDOWN_TIMEOUT                                                     3000

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/
static SEMAPHORE_DECL(buttonSemaphore, 0);
SEMAPHORE_DECL(shutdownBeepFinishedSemaphore, 0);
static virtual_timer_t vt;
static systime_t start, end;

static void shutdownTimeoutCallback(void *arg)
{
    (void)arg;

    end = chVTGetSystemTimeX();
    chSysLockFromISR();
    extChannelDisableI(&EXTD1, 8);
    chSysUnlockFromISR();
    chSemSignal(&buttonSemaphore);
}

static void buttonInterruptCallback(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    if (PAL_HIGH == palReadPad(GPIOB,GPIOB_BUTTON)) {
        start = chVTGetSystemTimeX();
        chSysLockFromISR();
        chVTSetI(&vt, MS2ST(SHUTDOWN_TIMEOUT), shutdownTimeoutCallback, NULL);
        chSysUnlockFromISR();
    } else {
        end = chVTGetSystemTimeX();
        chSysLockFromISR();
        chVTResetI(&vt);
        chSysUnlockFromISR();
        chSemSignal(&buttonSemaphore);
    }
}

static const EXTConfig extcfg = {
        {
                {EXT_CH_MODE_DISABLED, NULL},                
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_BOTH_EDGES | EXT_MODE_GPIOB, buttonInterruptCallback},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL},
                {EXT_CH_MODE_DISABLED, NULL}
        }
};

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(ButtonHandlerThread, arg)
{
    (void)arg;

    chSemObjectInit(&buttonSemaphore, 0);
    chSemObjectInit(&shutdownBeepFinishedSemaphore, 0);
    extStart(&EXTD1, &extcfg);
    extChannelEnable(&EXTD1, 8);

    while(1) {
        chSemWait(&buttonSemaphore);
        systime_t dt = end - start;
        if (end < start)
            dt += (systime_t)(-1);

        uint32_t pressDuration = ST2MS(dt);

        if ((STEP_VOLUME_MIN <= pressDuration) && (pressDuration < STEP_VOLUME_MAX)) {
            chEvtBroadcastFlags(&beeperEvent, STEP_VOLUME);
        } else if (SHUTDOWN_TIMEOUT <= pressDuration) {
                chEvtBroadcastFlags(&beeperEvent, SYSTEM_SHUTDOWN);
                chSemWait(&shutdownBeepFinishedSemaphore);
                palClearPad(GPIOA, GPIOA_SHUTDOWN);
        }
    }
}

/******************************* END OF FILE ***********************************/

