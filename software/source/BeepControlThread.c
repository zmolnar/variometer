/* thrBeepControl.c --- 
 * 
 * @file thrBeepControl.c
 * @brief Beep controller thread.
 * @author Zoltán Molnár
 * @date szo dec 19 14:11:02 2015 (+0100)
 * Version: 
 * Last-Updated: Mon Aug 22 19:08:21 2016 (+0200)
 *           By: Molnár Zoltán
 * 
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "BeepControlThread.h"
#include "hal.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/


/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/
#define TIM_BASE_CLK                                                    (1000000)
#define FREQ_TO_TICK(freq)                 ((freq) ? (TIM_BASE_CLK / (freq)) : 0)

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/


/*******************************************************************************/
/* DEFINITIONS OF GLOBAL CONSTANTS AND VARIABLES                               */
/*******************************************************************************/
static PWMConfig pwmcfg = {
        TIM_BASE_CLK,    
        0,      
        NULL,
        {
                {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                {PWM_OUTPUT_ACTIVE_HIGH, NULL}
        },
        /* HW dependent part.*/
        0,
        0,
};

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/


/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(BeepControlThread, arg)
{
        (void) arg;
        pwmcfg.period = FREQ_TO_TICK(2000);
        pwmStop (&PWMD4);                
        pwmStart (&PWMD4, &pwmcfg);
        pwmEnableChannelI (&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 5000));
        
        chThdSleepMilliseconds (50);

        pwmcfg.period = FREQ_TO_TICK(0);
        pwmStop (&PWMD4);                
        pwmStart (&PWMD4, &pwmcfg);
        pwmEnableChannelI (&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 5000));

        chThdSleepMilliseconds (50);

        pwmcfg.period = FREQ_TO_TICK(2000);
        pwmStop (&PWMD4);                
        pwmStart (&PWMD4, &pwmcfg);
        pwmEnableChannelI (&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 5000));
        
        chThdSleepMilliseconds (50);

        pwmcfg.period = FREQ_TO_TICK(0);
        pwmStop (&PWMD4);                
        pwmStart (&PWMD4, &pwmcfg);
        pwmEnableChannelI (&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, 5000));

        while (1) {
                chThdSleepMilliseconds (300);
        }
}


/******************************* END OF FILE ***********************************/

