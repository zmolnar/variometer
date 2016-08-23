/* ButtonHandler.c --- 
 * 
 * @file ButtonHandler.c
 * @brief Button handler module.
 * @author Molnár Zoltán
 * @date Mon Aug 22 19:51:16 2016 (+0200)
 * Version: 1.0.0
 * Last-Updated: Mon Aug 22 21:10:42 2016 (+0200)
 *           By: Molnár Zoltán
 * 
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ch.h"
#include "hal.h"

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
static SEMAPHORE_DECL(sem_button, 0);

systime_t start, end;

static void extcb1(EXTDriver *extp, expchannel_t channel) {

        (void)extp;
        (void)channel;

        if (PAL_LOW == palReadPad(GPIOB,GPIOB_BUTTON)) {
                start = chVTGetSystemTime();
        } else {
                end = chVTGetSystemTime();
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
                {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, extcb1},                
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
        
        chSemObjectInit(&sem_button, 0);
        extStart(&EXTD1, &extcfg);

        while(1) {
               chSemWait(&sem_button); 
               chThdSleepMilliseconds(500);
        }
}
/******************************* END OF FILE ***********************************/

