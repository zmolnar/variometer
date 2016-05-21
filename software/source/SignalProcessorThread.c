/**
 * @file SignalProcessorThread.c
 * @brief Thread to perform processing of raw pressure data.
 * @author Zoltán Molnár
 * @date Wed Dec 23 16:10:51 2015 (+0100)
 * Version: 
 * Last-Updated: Tue May 17 22:19:56 2016 (+0200)
 *           By: Molnár Zoltán
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
extern mutex_t mtx_dsp;
float dsp_vario = 0;
float dsp_pfil = 0;

event_source_t test_source;

/*******************************************************************************/
/* DECLARATION OF LOCAL FUNCTIONS                                              */
/*******************************************************************************/


/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
static float ab_filter (float a, float b, 
                        float *xk_1, float *vk_1, 
                        int32_t x, float dt)
{
        float xk = *xk_1 + (*vk_1 * dt);
        float vk = *vk_1;

        float rk = x - xk;

        xk += a * rk;
        vk += b * rk / dt;

        *xk_1 = xk;
        *vk_1 = vk;

        return xk;
}

static float calc_slope (float *buf, size_t blength, 
                         size_t start, size_t dlength, float dt)
{
        float x_avg = 0;
        float y_avg = 0;
        size_t i;
        for (i = 0; i < dlength; i++) {
                size_t j;
                if (start + i < blength)
                        j = start + i;
                else
                        j = i - blength + start;
                
                x_avg += i * dt;
                y_avg += buf[j];
        }

        x_avg /= dlength;
        y_avg /= dlength;

        float num = 0;
        float den = 0;
        for (i = 0; i < dlength; i++) {
                size_t j;
                if (start + i < blength)
                        j = start + i;
                else
                        j = i - blength + start;
                num += (i*dt - x_avg) * (buf[j] - y_avg);
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

        float pi_1;
        float vi_1;
        systime_t ti_1;
        float hbuf[BUFLENGTH] = {0};
        size_t dlength = 0;
        size_t i = 0;

        chEvtObjectInit (&test_source);

        // Wait for initial data.
        thread_t *psender = chMsgWait ();
        msg_t msg = chMsgGet (psender);
        struct PressureData_s data = *(struct PressureData_s *) msg;
        chMsgRelease (psender, 0);        

        pi_1 = (float)data.p_raw;
        ti_1 = data.t;
        vi_1 = 0;

        while (1) {
                psender = chMsgWait ();
                msg = chMsgGet (psender);
                data = *(struct PressureData_s *) msg;
                chMsgRelease (psender, 0);

                systime_t dt = data.t - ti_1;
                if (data.t < ti_1) 
                        dt += (systime_t)(-1);
                               
                float p_raw = data.p_raw;
                float p = ab_filter (ALPHA, BETA, &pi_1, &vi_1, p_raw, ST2MS(dt));
                float h = 44330 * (1 - pow ((p / 101325.0), 0.1902));
                hbuf[i++ % BUFLENGTH] = h;
                
                if (dlength < BUFLENGTH) {
                        dlength++;
                        continue;
                }               

                float v = calc_slope (hbuf, BUFLENGTH, i % BUFLENGTH, dlength, 
                                      ST2MS(dt) / 1000.0);

                ti_1 = data.t;

                dsp_vario = v;
                dsp_pfil  = p;

                //chprintf ((BaseSequentialStream *)&SD1, "dt=%d p=%f v=%f h=%f\n\r", ST2MS(dt), p, v, h);

        }
}


/******************************* END OF FILE ***********************************/

