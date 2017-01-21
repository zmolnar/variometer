/* BeepControlThread.c --- 
 * 
 * @file BeepControlThread.c
 * @brief Beep controller thread.
 * @author Zoltán, Molnár
 * 
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "BeepControlThread.h"
#include "ch.h"
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
typedef enum {
    VOLUME_ZERO = 0,
    VOLUME_LOW  = 200,
    VOLUME_MED  = 500,
    VOLUME_HIGH = 5000
} BeepVolume_t;

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

EVENTSOURCE_DECL(beeper_event_source);

static BeepVolume_t volume = VOLUME_HIGH;

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
static void set_pwm(uint32_t freq, BeepVolume_t volume)
{
    pwmcfg.period = FREQ_TO_TICK(freq);
    pwmStop(&PWMD4);
    pwmStart(&PWMD4, &pwmcfg);
    pwmEnableChannelI(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, volume));
}

static void step_volume(void)
{
    if (VOLUME_ZERO == volume) volume = VOLUME_LOW;
    else if (VOLUME_LOW == volume) volume = VOLUME_MED;
    else if (VOLUME_MED == volume) volume = VOLUME_HIGH;
    else if (VOLUME_HIGH == volume) volume = VOLUME_ZERO;

    set_pwm(0, volume);
    chThdSleepMilliseconds(50);
    set_pwm(2000, volume);
    chThdSleepMilliseconds(200);
    set_pwm(0, volume);
    chThdSleepMilliseconds(50);
}

static void play_startup_signal(void)
{
    size_t i;
    for (i = 0; i < 2; i++) {
        set_pwm(0, VOLUME_HIGH);
        chThdSleepMilliseconds(50);

        set_pwm(2000, VOLUME_HIGH);
        chThdSleepMilliseconds(50);
    }
}

static void play_shutdown_signal(void)
{
    size_t i;
    for (i = 0; i < 5; i++) {
        set_pwm(0, VOLUME_HIGH);
        chThdSleepMilliseconds(50);

        set_pwm(2000, VOLUME_HIGH);
        chThdSleepMilliseconds(50);
    }
}

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(BeepControlThread, arg)
{
    (void)arg;

    play_startup_signal();

    chEvtObjectInit(&beeper_event_source);

    event_listener_t beeper_listener;
    chEvtRegisterMaskWithFlags(
            &beeper_event_source,
            &beeper_listener,
            EVENT_MASK(0),
            START_BEEPER     |
            STOP_BEEPER      |
            STEP_VOLUME |
            SYSTEM_SHUTDOWN);

    while (1) {
        eventmask_t evt = chEvtWaitAny(EVENT_MASK(0));

        if (evt & EVENT_MASK(0)) {
            eventflags_t flags = chEvtGetAndClearFlags(&beeper_listener);

            if (flags & START_BEEPER) {
                chThdSleepMilliseconds(100);
            }
            if (flags & STOP_BEEPER) {
                chThdSleepMilliseconds(100);
            }
            if (flags & STEP_VOLUME) {
                step_volume();
            }
            if (flags & SYSTEM_SHUTDOWN) {
                play_shutdown_signal();
                palClearPad(GPIOA, GPIOA_SHUTDOWN);
            }
        }
    }
}


/******************************* END OF FILE ***********************************/

