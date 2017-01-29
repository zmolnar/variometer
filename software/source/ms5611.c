/**
 * @file ms5611.c
 * @brief MS5611 barometric pressure sensor driver interface.
 * @author Molnar Zoltan
 */

/*******************************************************************************/
/* INCLUDES                                                                    */
/*******************************************************************************/
#include "ms5611.h"
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
/**
 * Enumeration to select which parameter to read from MS5611.
 */
typedef enum ms5611_data {
    MS5611_PRESSURE,
    MS5611_TEMP
} ms5611_data_t;

/**
 * MS5611 command set.
 * @defgroup MS5611 pressure sensor command set.
 * @{
 */
typedef enum ms5611_command {
    MS5611_CMD_RESET              = 0x1e, /**< Reset command. */
    MS5611_CMD_CONV_D1_OSR_256    = 0x40, /**< Convert P OSR=256. */
    MS5611_CMD_CONV_D1_OSR_512    = 0x42, /**< Convert P OSR=512. */
    MS5611_CMD_CONV_D1_OSR_1024   = 0x44, /**< Convert P OSR=1024. */
    MS5611_CMD_CONV_D1_OSR_2048   = 0x46, /**< Convert P OSR=2048. */
    MS5611_CMD_CONV_D1_OSR_4096   = 0x48, /**< Convert P OSR=4096. */
    MS5611_CMD_CONV_D2_OSR_256    = 0x50, /**< Convert T OSR=256. */
    MS5611_CMD_CONV_D2_OSR_512    = 0x52, /**< Convert T OSR=512. */
    MS5611_CMD_CONV_D2_OSR_1024   = 0x54, /**< Convert T OSR=1024. */
    MS5611_CMD_CONV_D2_OSR_2048   = 0x56, /**< Convert T OSR=2048. */
    MS5611_CMD_CONV_D2_OSR_4096   = 0x58, /**< Convert T OSR=4096. */
    MS5611_CMD_ADC_READ           = 0x00, /**< Read conversation result. */
    MS5611_CMD_PROM_READ_BASE     = 0xa0  /**< Read PROM register. */
} ms5611_command_t;
/** @} */

/**
 * MS5611 PROM internal registers containing calibration data.
 */
typedef enum ms5611_prom_register {
    MS5611_PROM_FACT = 0x00,
    MS5611_PROM_C1   = 0x01,
    MS5611_PROM_C2   = 0x02,
    MS5611_PROM_C3   = 0x03,
    MS5611_PROM_C4   = 0x04,
    MS5611_PROM_C5   = 0x05,
    MS5611_PROM_C6   = 0x06,
    MS5611_PROM_CRC  = 0x07
} ms5611_prom_register_t;

/**
 * Macro to calculate command byte for reading specific PROM register.
 */
#define MS5611_READ_PROM_REGISTER_CMD(reg) (MS5611_CMD_PROM_READ_BASE | ((reg)<<1))

/*******************************************************************************/
/* DEFINITIONS OF GLOBAL CONSTANTS AND VARIABLES                               */
/*******************************************************************************/
/**
 * SPI driver configuration.
 * @brief MS5611 SPI configuration structure.
 */
const SPIConfig ms5611_spi_cfg = {
        NULL,
        MS5611_SPI_PORT,
        MS5611_SPI_NSS,
        (((0x2 << 3) & SPI_CR1_BR)    |
           SPI_CR1_MSTR               |
           SPI_CR1_CPOL               |
           SPI_CR1_CPHA)
};

/**
 * Calibration constants for pressure calculation.
 * @{
 */
static uint16_t C1 = 0;  /**< Pressure sensitivity | SENS T1 */
static uint16_t C2 = 0;  /**< Pressure offset | OFF T1 */
static uint16_t C3 = 0;  /**< Temperature coefficient of pressure sensitivity | TCS */
static uint16_t C4 = 0;  /**< Temperature coefficient of pressure offset | TCO */
static uint16_t C5 = 0;  /**< Reference temperature | T REF */
static uint16_t C6 = 0;  /**< Temperature coefficient of the temperature | TEMPSENS */
/** @} */

/*******************************************************************************/
/* DEFINITION OF LOCAL FUNCTIONS                                               */
/*******************************************************************************/
/**
 * Send reset sequence to the MS5611.
 */
static void ms5611Reset(void)
{
    uint8_t cmd = MS5611_CMD_RESET;

    spiSelect(MS5611_SPI);
    spiSend(MS5611_SPI, sizeof(cmd), (void *)&cmd);
    spiUnselect(MS5611_SPI);
}

/**
 * Read PROM register from MS5611.
 *
 * @param[in] Reg Identifier of the register to read
 * @retval Register value
 */
static uint32_t ms5611ReadRegister(const ms5611_prom_register_t Reg)
{
    uint8_t cmd = MS5611_READ_PROM_REGISTER_CMD(Reg);
    uint8_t tmp[2];

    spiSelect(MS5611_SPI);
    spiSend(MS5611_SPI, sizeof(cmd), &cmd);
    spiReceive(MS5611_SPI, sizeof(tmp), tmp);
    spiUnselect(MS5611_SPI);

    /* Swap bytes */
    return (((uint16_t)tmp[0]) << 8) + (uint16_t)tmp[1];
}

/**
 * Read measurement data from MS5611.
 * @brief Start conversation and read result from the sensor.
 * @param[in] data Identifier of the desired parameter to read
 *            MS5611_PRESSURE : read uncompensated pressure
 *            MS5611_TEMP     : read uncompensated temperature
 * @return The result of the conversation.
 */
static uint32_t ms5611Convert(ms5611_data_t data)
{
    uint8_t cmd = 0;
    uint8_t tmp[3];

    switch (data) {
    case MS5611_PRESSURE: {
        cmd = MS5611_CMD_CONV_D1_OSR_4096;
        break;
    }
    case MS5611_TEMP: {
        cmd = MS5611_CMD_CONV_D2_OSR_4096;
        break;
    }
    default:
        return 0;
    }

    /* Start conversation. */
    spiSelect(MS5611_SPI);
    spiSend(MS5611_SPI, sizeof(cmd), (void *)&cmd);
    spiUnselect(MS5611_SPI);
    chThdSleepMilliseconds(10);

    /* Read result. */
    cmd = MS5611_CMD_ADC_READ;
    spiSelect(MS5611_SPI);
    spiSend(MS5611_SPI, sizeof(cmd), &cmd);
    spiReceive(MS5611_SPI, sizeof(tmp), tmp);
    spiUnselect(MS5611_SPI);

    /* Swap byte order */
    return (((uint32_t)tmp[0]) << 16) + (((uint32_t)tmp[1]) << 8) + (uint32_t)tmp[2];
}

/*******************************************************************************/
/* DEFINITION OF GLOBAL FUNCTIONS                                              */
/*******************************************************************************/
void MS5611_Init(void)
{
    palSetPad(MS5611_SPI_PORT, MS5611_SPI_NSS);
    spiObjectInit(MS5611_SPI);
    spiStart(MS5611_SPI, &ms5611_spi_cfg);
}

void MS5611_Start (void)
{
    ms5611Reset();
    chThdSleepMilliseconds(250);
    C1 = ms5611ReadRegister(MS5611_PROM_C1);
    C2 = ms5611ReadRegister(MS5611_PROM_C2);
    C3 = ms5611ReadRegister(MS5611_PROM_C3);
    C4 = ms5611ReadRegister(MS5611_PROM_C4);
    C5 = ms5611ReadRegister(MS5611_PROM_C5);
    C6 = ms5611ReadRegister(MS5611_PROM_C6);
}

void MS5611_Measure(uint32_t *pP, int32_t *pT)
{
    uint32_t D1 = ms5611Convert(MS5611_PRESSURE);
    uint32_t D2 = ms5611Convert(MS5611_TEMP);
    int64_t dT = (int64_t)D2 - ((uint64_t)C5 << 8);
    int64_t TEMP = 2000 + ((dT * (int64_t)C6) >> 23);
    int64_t OFF = ((uint64_t)C2 << 16) + (((int64_t)C4 * dT) >> 7);
    int64_t SENS = ((int64_t)C1 << 15) + ((dT * (int64_t)(C3) >> 8));
    int64_t T2 = 0;;
    int64_t OFF2 = 0;
    int64_t SENS2 = 0;

    /* Second order temperature compensation. */
    if (TEMP < 2000) {
        T2 = ((dT * dT) >> 31);
        OFF2 = (5 * (TEMP - 2000) * (TEMP - 2000)) >> 1;
        SENS2 = (5 * (TEMP - 2000) * (TEMP - 2000)) >> 2;

        /* Very low temperature. */
        if( TEMP < (-15)) {
            OFF2 = OFF2 + (7 * (TEMP + 1500) * (TEMP + 1500));
            SENS2 = SENS2 + ((11 * (TEMP + 1500) * (TEMP + 1500)) >> 2);
        }
    }
    else {
        T2 = 0;
        OFF2 = 0;
        SENS2 = 0;
    }

    TEMP -= T2;
    OFF -= OFF2;
    SENS -= SENS2;

    /* Calculate temperature and temperature compensated pressure */
    *pT = (int32_t)TEMP;
    *pP = (uint32_t)(((((int64_t)D1 * SENS) >> 21) - OFF) >> 15);
}

/******************************* END OF FILE ***********************************/

