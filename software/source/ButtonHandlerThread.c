/* ButtonHandler.c --- 
 * 
 * @file ButtonHandler.c
 * @brief Button handler module.
 * @author Zoltán, MOLNÁR
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
#define BUTTON_PRESS_STEP_VOLUME_MIN                                           50
#define BUTTON_PRESS_STEP_VOLUME_MAX                                         1000
#define BUTTON_PRESS_SHUTDOWN_MIN                                            3000

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/

/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/
static SEMAPHORE_DECL(sem_button, 0);
static virtual_timer_t vt;
static systime_t start, end;

static void button_timeout_cb(void *arg)
{
    (void)arg;

    chSysLockFromISR();
    extChannelDisableI(&EXTD1, 8);
    chEvtBroadcastFlagsI(&beeper_event_source, SYSTEM_SHUTDOWN);
    chSysUnlockFromISR();
}

static void extcb(EXTDriver *extp, expchannel_t channel)
{
    (void)extp;
    (void)channel;

    if (PAL_HIGH == palReadPad(GPIOB,GPIOB_BUTTON)) {
        start = chVTGetSystemTimeX();
        chSysLockFromISR();
        chVTSetI(&vt, MS2ST(BUTTON_PRESS_SHUTDOWN_MIN), button_timeout_cb, NULL);
        chSysUnlockFromISR();
    } else {
        end = chVTGetSystemTimeX();
        chSysLockFromISR();
        chVTResetI(&vt);
        chSysUnlockFromISR();
        chSemSignal(&sem_button);
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
                {EXT_CH_MODE_BOTH_EDGES | EXT_MODE_GPIOB, extcb}, 
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

    chThdSleepMilliseconds(2000);

    chSemObjectInit(&sem_button, 0);
    extStart(&EXTD1, &extcfg);
    extChannelEnable(&EXTD1, 8);

    while(1) {
        chSemWait(&sem_button);
        systime_t dt = end - start;
        if (end < start)
            dt += (systime_t)(-1);

        uint32_t t = ST2MS(dt);

        if ((BUTTON_PRESS_STEP_VOLUME_MIN <= t) &&
            (t < BUTTON_PRESS_STEP_VOLUME_MAX)) {
            chEvtBroadcastFlags(&beeper_event_source, STEP_VOLUME);
        }
    }
}

/******************************* END OF FILE ***********************************/

