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
#include "SerialHandlerThread.h"

/*
 * Thread working area definitions.
 */
static THD_WORKING_AREA(waBeepControl, 128);
static THD_WORKING_AREA(waPressureReader, 128);
static THD_WORKING_AREA(waSignalProcessor, 2048);
static THD_WORKING_AREA(waSerialHandler, 128);

/*
 * Thread references.
 */
thread_t *pBeepControlThread;
thread_t *pPressureReaderThread;
thread_t *pSignalProcessorThread;
thread_t *pSerialHandlerThread;

/*
 * Mutex for protecting calculated pressure and vario data.
 */
mutex_t mtx_dsp;

/*
 * Stream references.
 */
BaseSequentialStream *kobo;

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

        chThdSleepMilliseconds(1000);
        palSetPad(GPIOA, GPIOA_SHUTDOWN);
        /*
         * Activates the serial driver 1 using the driver default configuration.
         * PA9(TX) and PA10(RX) are routed to USART1. This interface is used to 
         * change data with Kobo.
         */
        sdStart(&SD1, NULL);
#if 1
        static SerialConfig sd2conf = {9600};
        sdStart (&SD2, &sd2conf);
#endif
        /*
         * Create threads.
         */

        pPressureReaderThread = chThdCreateStatic (waPressureReader, sizeof(waPressureReader), 
                                                   NORMALPRIO+4, PressureReaderThread, NULL);
        pSignalProcessorThread = chThdCreateStatic (waSignalProcessor, sizeof (waSignalProcessor),
                                                    NORMALPRIO+3, SignalProcessorThread, NULL);
#if 0
        pBeepControlThread = chThdCreateStatic (waBeepControl, sizeof (waBeepControl),
                                                NORMALPRIO+1, BeepControlThread, NULL);
#endif
#if 1
        pSerialHandlerThread = chThdCreateStatic (waSerialHandler, sizeof (waSerialHandler),
                                                  NORMALPRIO+2, SerialHandlerThread, NULL);
#endif
        /*
         * Blink heartbeat LED.
         */
        while (true) {
                palClearPad(GPIOB, GPIOB_LED);
                chThdSleepMilliseconds(500);
                palSetPad(GPIOB, GPIOB_LED);
                chThdSleepMilliseconds(500);
        }
}
