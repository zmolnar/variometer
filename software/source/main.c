/*
  ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 */

#include "ch.h"
#include "hal.h"
#include "BeepControlThread.h"
#include "PressureReaderThread.h"
#include "SignalProcessorThread.h"
#include "SimulatorThread.h"
#include "SerialHandlerThread.h"
#include "ButtonHandlerThread.h"
#include "NmeaGeneratorThread.h"

#undef USE_SIMULATED_DATA

/*
 * Thread working area definitions.
 */
static THD_WORKING_AREA(waBeepControl, 1024);
#ifndef USE_SIMULATED_DATA
static THD_WORKING_AREA(waPressureReader, 128);
static THD_WORKING_AREA(waSignalProcessor, 2048);
#else
static THD_WORKING_AREA(waSimulator, 128);
#endif
static THD_WORKING_AREA(waSerialHandler, 128);
static THD_WORKING_AREA(waButtonHandler, 1024);
static THD_WORKING_AREA(waNmeaGenerator, 1024);

/*
 * Thread references.
 */
thread_t *pBeepControlThread;
#ifndef USE_SIMULATED_DATA
thread_t *pPressureReaderThread;
thread_t *pSignalProcessorThread;
#else
thread_t *pSimulatorThread;
#endif
thread_t *pSerialHandlerThread;
thread_t *pButtonHandlerThread;
thread_t *pNmeaGeneratorThread;

/**
 * Watchdog configuration.
 */
static const WDGConfig watchDogConfig = {
    (STM32_IWDG_PR_MASK & STM32_IWDG_PR_8),
    0x0fff
};

/*
 * Application entry point.
 */
int main(void) {

    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured device drivers
     *   and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread and the
     *   RTOS is active.
     */
    halInit();
    chSysInit();

    /*
     * Wait 2 seconds here to prevent false startups caused by
     * pushing the button accidentally.
     */
    palClearPad(GPIOA, GPIOA_SHUTDOWN);
    chThdSleepMilliseconds(2000);
    palSetPad(GPIOA, GPIOA_SHUTDOWN);

    /*
     * Create threads.
     */
#ifndef USE_SIMULATED_DATA
    pPressureReaderThread = chThdCreateStatic (
            waPressureReader,
            sizeof(waPressureReader),
            NORMALPRIO + 5,
            PressureReaderThread,
            NULL);

    pSignalProcessorThread = chThdCreateStatic (
            waSignalProcessor,
            sizeof (waSignalProcessor),
            NORMALPRIO + 4,
            SignalProcessorThread,
            NULL);
#else
    pSimulatorThread = chThdCreateStatic (
            waSimulator,
            sizeof(waSimulator),
            NORMALPRIO + 4,
            SimulatorThread,
            NULL);
#endif
#if 1
    pSerialHandlerThread = chThdCreateStatic (
            waSerialHandler,
            sizeof (waSerialHandler),
            NORMALPRIO + 3,
            SerialHandlerThread,
            NULL);
#endif
#if 1
    pBeepControlThread = chThdCreateStatic (
            waBeepControl,
            sizeof (waBeepControl),
            NORMALPRIO + 2,
            BeepControlThread,
            NULL);
#endif
#if 1
    pNmeaGeneratorThread = chThdCreateStatic (
            waNmeaGenerator,
            sizeof(waNmeaGenerator),
            NORMALPRIO + 3,
            NmeaGeneratorThread,
            NULL);
#endif
#if 1
    pButtonHandlerThread = chThdCreateStatic (
            waButtonHandler,
            sizeof (waButtonHandler),
            NORMALPRIO + 1,
            ButtonHandlerThread,
            NULL);
#endif

    wdgStart(&WDGD1, &watchDogConfig);

    /*
     * Blink heartbeat LED.
     */
    while (true) {
        palClearPad(GPIOB, GPIOB_LED);
        chThdSleepMilliseconds(500);
        wdgReset(&WDGD1);
        palSetPad(GPIOB, GPIOB_LED);
        chThdSleepMilliseconds(500);
        wdgReset(&WDGD1);
    }
}
