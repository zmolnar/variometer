/* ButtonHandler.c --- 
 * 
 * @file ButtonHandler.c
 * @brief Button handler module.
 * @author Molnár Zoltán
 * @date Mon Aug 22 19:51:16 2016 (+0200)
 * Version: 1.0.0
<<<<<<< HEAD
 * Last-Updated: Sun Aug 28 20:17:28 2016 (+0200)
=======
 * Last-Updated: Mon Aug 22 21:10:42 2016 (+0200)
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
 *           By: Molnár Zoltán
 * 
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ch.h"
#include "hal.h"
<<<<<<< HEAD
#include "BeepControlThread.h"
=======
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/
<<<<<<< HEAD
#define BUTTON_PRESS_STEP_VOLUME_MIN                                           50
#define BUTTON_PRESS_STEP_VOLUME_MAX                                         1000
#define BUTTON_PRESS_SHUTDOWN_MIN                                            3000
=======

>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

<<<<<<< HEAD
=======

>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/

<<<<<<< HEAD
=======

>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
/*******************************************************************************/
/* DEFINITION OF GLOBAL CONSTANTS AND VARIABLES                                */
/*******************************************************************************/
static SEMAPHORE_DECL(sem_button, 0);
<<<<<<< HEAD
static virtual_timer_t vt;
static systime_t start, end;

static void button_timeout_cb (void *arg)
{
        (void)arg;
        extChannelDisable(&EXTD1, 8);
        end = start + MS2ST(BUTTON_PRESS_SHUTDOWN_MIN);
        chSemSignal(&sem_button);
}

static void extcb(EXTDriver *extp, expchannel_t channel) {
=======

systime_t start, end;

static void extcb1(EXTDriver *extp, expchannel_t channel) {
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299

        (void)extp;
        (void)channel;

<<<<<<< HEAD
        if (PAL_HIGH == palReadPad(GPIOB,GPIOB_BUTTON)) {
                start = chVTGetSystemTime();
                chSysLockFromISR();
                chVTSetI(&vt, MS2ST(5000), button_timeout_cb, NULL);
                chSysUnlockFromISR();
        } else {
                end = chVTGetSystemTime();
                chSysLockFromISR();
                chVTResetI(&vt);
                chSysUnlockFromISR();
=======
        if (PAL_LOW == palReadPad(GPIOB,GPIOB_BUTTON)) {
                start = chVTGetSystemTime();
        } else {
                end = chVTGetSystemTime();
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
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
<<<<<<< HEAD
                {EXT_CH_MODE_BOTH_EDGES | EXT_MODE_GPIOB, extcb}, 
=======
                {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, extcb1},                
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
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

<<<<<<< HEAD
=======

>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(ButtonHandlerThread, arg)
{
        (void)arg;
        
        chSemObjectInit(&sem_button, 0);
        extStart(&EXTD1, &extcfg);
<<<<<<< HEAD
        extChannelEnable(&EXTD1, 8);

        while(1) {
               chSemWait(&sem_button); 
               systime_t dt = end - start;
               if (end < start)
                       dt += (systime_t)(-1);

               uint32_t t = ST2MS(dt);

               if ((BUTTON_PRESS_STEP_VOLUME_MIN <= t) && (t < BUTTON_PRESS_STEP_VOLUME_MAX)) {
                       chEvtBroadcastFlags (&beeper_event_source, STEP_BEEP_VOLUME);
               } else if ((BUTTON_PRESS_SHUTDOWN_MIN <= t)) {
                       chEvtBroadcastFlags (&beeper_event_source, SYSTEM_SHUTDOWN);
               } else {
                       ;
               }
=======

        while(1) {
               chSemWait(&sem_button); 
               chThdSleepMilliseconds(500);
>>>>>>> a9b4e0d05510d08daf4e2076668174f91ebb4299
        }
}
/******************************* END OF FILE ***********************************/

