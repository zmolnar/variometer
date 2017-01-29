/**
 * @file ms5611.h
 * @brief MS5611 barometric pressure sensor driver interface.
 * @author Molnar Zoltan
 */

#ifndef MS5611_H
#define MS5611_H

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ch.h"
#include "hal.h"

/*******************************************************************************/
/* DEFINED CONSTANTS                                                           */
/*******************************************************************************/
#define MS5611_SPI                                                         &SPID1
#define MS5611_SPI_PORT                                                     GPIOA
#define MS5611_SPI_NSS                                       GPIOA_MS5611_SPI_NSS

/*******************************************************************************/
/* MACRO DEFINITIONS                                                           */
/*******************************************************************************/

/*******************************************************************************/
/* TYPE DEFINITIONS                                                            */
/*******************************************************************************/

/*******************************************************************************/
/* DECLARATIONS OF GLOBAL VARIABLES                                           */
/*******************************************************************************/

/*******************************************************************************/
/* DECLARATION OF GLOBAL FUNCTIONS                                             */
/*******************************************************************************/
/**
 * Initialize MS5611 serial interface.
 */
void MS5611_Init(void);

/**
 * Send reset command and read calibration constants from MS5611.
 */
void MS5611_Start(void);

/**
 * Read raw pressure and temperature values from MS5611.
 * @param[in] pP Pointer to the variable to store temperature compensated 
 *               raw pressure value.
 * @param[in] pT Pointer to the variable to store raw temperature value.
 */
void MS5611_Measure(uint32_t *pP, int32_t *pT);

#endif

/******************************* END OF FILE ***********************************/

