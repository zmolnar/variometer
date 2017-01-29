/*
 * @file BeepControlThread.c
 * @brief Beep controller thread.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "BeepControlThread.h"
#include "SignalProcessorThread.h"
#include "ch.h"
#include "hal.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/
#define PWM_TIM_BASE_CLK                                                (1000000)
#define FREQ_TO_TICK(freq)             ((freq) ? (PWM_TIM_BASE_CLK / (freq)) : 0)
#define BEEP_TIM_BASE_CLK                                                (200000)
#define MS2TIMTICK(x)                            ((x) * BEEP_TIM_BASE_CLK / 1000)

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/
typedef enum {
    VOLUME_ZERO = 0,
    VOLUME_LOW  = 200,
    VOLUME_MED  = 500,
    VOLUME_HIGH = 5000
} BeepVolume_t;

typedef enum {
    BEEP_LIFTING,
    BEEP_SINKING,
    BEEP_DISABLED
} BeepControlState_t;

typedef enum {
    BEEP_ON,
    BEEP_OFF
} BeepState_t;

/*******************************************************************************/
/* DEFINITIONS OF GLOBAL CONSTANTS AND VARIABLES                               */
/*******************************************************************************/
static PWMConfig pwmcfg = {
        PWM_TIM_BASE_CLK,
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

static void timerCallback(GPTDriver *gptp);

static GPTConfig beepTimerConfig =
{
        BEEP_TIM_BASE_CLK,           /* Timer clock.*/
        timerCallback,               /* Timer callback function.*/
        /* HW dependent part.*/
        0,
        0
};

EVENTSOURCE_DECL(beeperEvent);

static float liftThreshold = 0.6;
static float liftOffThreshold = 0.2;
static float sinkThreshold = -1;
static float sinkOffThreshold = -0.6;

static float maximumLift = 6;
static float liftFreqBase = 600;
static float liftFreqMax = 1500;
static float maximumSink = (-6);
static float sinkFreqBase = 400;
static float sinkFreqMin = 150;

static float beepDurationLiftMin = 350;
static float beepDurationLiftMax = 100;
static float silenceDurationLiftMin = 230;
static float silenceDurationLiftMax = 60;

static BeepControlState_t beepControlState = BEEP_DISABLED;
static BeepState_t beepState = BEEP_OFF;
static BeepVolume_t beepVolume = VOLUME_MED;
static uint32_t beepFrequency;
static uint32_t beepDuration;
static uint32_t silenceDuration;
static float actualVario;

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/
static void startBeep(void);

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
static void setPwmFreqAndDutyCycleI(uint32_t freq, BeepVolume_t volume)
{
    pwmChangePeriodI(&PWMD4, FREQ_TO_TICK(freq));
    pwmEnableChannelI(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, volume));
}

static void setPwmFreqAndDutyCycle(uint32_t freq, BeepVolume_t volume)
{
    pwmChangePeriod(&PWMD4, FREQ_TO_TICK(freq));
    pwmEnableChannel(&PWMD4, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD4, volume));
}

static void enableBeepI(void) {
    setPwmFreqAndDutyCycleI(beepFrequency, beepVolume);
    beepState = BEEP_ON;
}

static void disableBeepI(void) {
    setPwmFreqAndDutyCycleI(0, VOLUME_ZERO);
    beepState = BEEP_OFF;
}

static void readMeasurementData(void) {
    chMtxLock(&SignalProcessorMutex);
    actualVario = SignalProcessingOutputData.vario;
    chMtxUnlock(&SignalProcessorMutex);
}

static void calculateLiftFrequency(void) {
    float vario = actualVario;
    if (maximumLift < vario)
        vario = maximumLift;

    float range = liftFreqMax - liftFreqBase;
    float unit = range / maximumLift;
    float offset = vario * unit;
    beepFrequency = (uint32_t)(liftFreqBase + offset);
}

static void calculateSinkFrequency(void) {
    float vario = actualVario;
    if (vario < maximumSink)
        vario = maximumSink;

    float range = sinkFreqBase - sinkFreqMin;
    float unit = range / maximumSink;
    float offset = vario * unit;
    beepFrequency = (uint32_t)(sinkFreqBase - offset);
}

static void calculateBeepFrequency(void) {
    if (0 < actualVario)
        calculateLiftFrequency();
    else
        calculateSinkFrequency();
}

static void calculateBeepDuration(void) {
    float vario = actualVario;
    if (vario < 0)
        vario *= (-1);

    float range = beepDurationLiftMax - beepDurationLiftMin;
    float unit = range / maximumLift;
    float offset = vario * unit;
    beepDuration = (uint32_t)(beepDurationLiftMin + offset);
}

static void calculateSilenceDuration(void) {
    if (0 < actualVario) {
        float vario = actualVario;
        if (maximumLift < vario)
            vario = maximumLift;

        float range = silenceDurationLiftMax - silenceDurationLiftMin;
        float unit = range / maximumLift;
        float offset = vario * unit;
        silenceDuration = (uint32_t)(silenceDurationLiftMin + offset);
    } else {
        silenceDuration = 0;
    }
}

static void startBeep(void) {
    calculateBeepDuration();
    chSysLock();
    gptStartOneShotI(&GPTD3, MS2TIMTICK(beepDuration));
    enableBeepI();
    chSysUnlock();
}

static void timerCallback(GPTDriver *gptp) {

    (void)gptp;

    if(BEEP_DISABLED == beepControlState) {
        chSysLockFromISR();
        disableBeepI();
        chSysUnlockFromISR();
        return;
    }

    calculateSilenceDuration();
    calculateBeepDuration();

    switch(beepState) {
    case BEEP_ON:
        if(BEEP_SINKING == beepControlState) {
            beepState = BEEP_ON;
            gptStartOneShotI(&GPTD3, MS2TIMTICK(beepDuration));
            chSysUnlockFromISR();
        } else {
            beepState = BEEP_OFF;
            chSysLockFromISR();
            disableBeepI();
            gptStartOneShotI(&GPTD3, MS2TIMTICK(silenceDuration));
            chSysUnlockFromISR();
        }
        break;
    case BEEP_OFF:
        beepState = BEEP_ON;
        chSysLockFromISR();
        enableBeepI();
        gptStartOneShotI(&GPTD3, MS2TIMTICK(beepDuration));
        chSysUnlockFromISR();
        break;
    default:
        break;
    }
}

static void updateBeeperStateMachine(void) {
    switch(beepControlState) {
    case BEEP_LIFTING:
        if (actualVario < liftOffThreshold) {
            beepControlState = BEEP_DISABLED;
        }
        break;
    case BEEP_SINKING:
        if (sinkOffThreshold < actualVario)
            beepControlState = BEEP_DISABLED;
        break;
    case BEEP_DISABLED:
        if (liftThreshold < actualVario)
            beepControlState = BEEP_LIFTING;
        if (actualVario < sinkThreshold)
            beepControlState = BEEP_SINKING;

        if(BEEP_DISABLED != beepControlState)
            startBeep();

        break;
    default:
        break;
    }
}

static void stepVolume(void) {
    switch(beepVolume) {
    case VOLUME_ZERO:
        beepVolume = VOLUME_LOW;
        break;
    case VOLUME_LOW:
        beepVolume = VOLUME_MED;
        break;
    case VOLUME_MED:
        beepVolume = VOLUME_HIGH;
        break;
    case VOLUME_HIGH:
        beepVolume = VOLUME_ZERO;
        break;
    default:
        beepVolume = VOLUME_HIGH;
        break;
    }
}

static void playStartupSignal(void) {
    size_t i;
    for (i = 0; i < 2; i++) {
        setPwmFreqAndDutyCycle(2000, VOLUME_HIGH);
        chThdSleepMilliseconds(50);

        setPwmFreqAndDutyCycle(0, VOLUME_ZERO);
        chThdSleepMilliseconds(50);
    }
}

static void playShutdownSignal(void) {
    size_t i;

    chThdSleepMicroseconds(500);

    for (i = 0; i < 5; i++) {
        setPwmFreqAndDutyCycle(2000, VOLUME_HIGH);
        chThdSleepMilliseconds(50);

        setPwmFreqAndDutyCycle(0, VOLUME_ZERO);
        chThdSleepMilliseconds(50);
    }
}

static void playVolumeSetSignal(void) {
    setPwmFreqAndDutyCycle(0, beepVolume);
    chThdSleepMilliseconds(50);

    setPwmFreqAndDutyCycle(2000, beepVolume);
    chThdSleepMilliseconds(200);

    setPwmFreqAndDutyCycle(0, beepVolume);
    chThdSleepMilliseconds(50);
}

static void updateBeepFrequency(void) {
    if ((BEEP_ON == beepState) && (BEEP_DISABLED != beepControlState)) {
        chSysLock();
        setPwmFreqAndDutyCycleI(beepFrequency, beepVolume);
        chSysUnlock();
    }
}

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
THD_FUNCTION(BeepControlThread, arg)
{
    (void)arg;

    chEvtObjectInit(&beeperEvent);

    pwmStart(&PWMD4, &pwmcfg);
    pwmEnableChannel(&PWMD4, 0, VOLUME_ZERO);

    event_listener_t beeperListener;
    chEvtRegisterMaskWithFlags(
            &beeperEvent,
            &beeperListener,
            EVENT_MASK(0),
            STEP_VOLUME  |
            SYSTEM_SHUTDOWN);

    event_listener_t signalProcessorListener;
    chEvtRegisterMaskWithFlags(
            &signalProcessorEvent,
            &signalProcessorListener,
            EVENT_MASK(1),
            CALCULATION_FINISHED);

    gptStart(&GPTD3, &beepTimerConfig);

    while (1) {
        eventmask_t event = chEvtWaitAny(ALL_EVENTS);

        if (event & EVENT_MASK(0)) {
            eventflags_t flags = chEvtGetAndClearFlags(&beeperListener);
            if (flags & STEP_VOLUME) {
                stepVolume();
            }
            if (flags & SYSTEM_SHUTDOWN) {
                chSysLock();
                disableBeepI();
                gptStopTimerI(&GPTD3);
                chSysUnlock();
                playShutdownSignal();
                palClearPad(GPIOA, GPIOA_SHUTDOWN);
            }
        }
        if (event & EVENT_MASK(1)) {
            eventflags_t flags = chEvtGetAndClearFlags(&signalProcessorListener);

            if (flags & CALCULATION_FINISHED) {
                readMeasurementData();
                calculateBeepFrequency();
                updateBeeperStateMachine();
                updateBeepFrequency();
            }
        }
    }
}

/******************************* END OF FILE ***********************************/

