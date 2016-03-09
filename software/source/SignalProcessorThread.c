/**
 * @file SignalProcessorThread.c
 * @brief Thread to perform processing of raw pressure data.
 * @author Zoltán Molnár
 * @date Wed Dec 23 16:10:51 2015 (+0100)
 * Version: 
 * Last-Updated: Sun Jan 17 20:59:08 2016 (+0100)
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
        for ( i = 0; i < dlength; i++) {
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

        static float pi_1;
        static float vi_1;
        static systime_t ti_1;
        float hbuf[BUFLENGTH] = {0};
        size_t dlength = 0;
        size_t i = 0;

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

                float p_raw = data.p_raw;
                float dt = (ST2MS(data.t) - ST2MS(ti_1));
                float p = ab_filter (ALPHA, BETA, &pi_1, &vi_1, p_raw, dt);
                float h = 44330 * (1 - pow ((p / 101325.0), 0.1902));

                hbuf[i] = h;
                if (i < (BUFLENGTH - 1))
                        i++;
                else
                        i = 0;
                
                if (dlength < BUFLENGTH) {
                        dlength++;
                        chprintf ((BaseSequentialStream *)&SD1,"continue\n");
                        continue;
                }               

                float v = calc_slope (hbuf, BUFLENGTH, i, dlength, dt);

                ti_1 = data.t;

//                chMtxLock (&mtx_dsp);
                dsp_vario = v;
                dsp_pfil  = p;
//                chMtxUnlock (&mtx_dsp);

                chprintf ((BaseSequentialStream *)&SD1, "%f %f %d\n\r", 
                          h, p, data.p_raw);
        }
}


/******************************* END OF FILE ***********************************/

