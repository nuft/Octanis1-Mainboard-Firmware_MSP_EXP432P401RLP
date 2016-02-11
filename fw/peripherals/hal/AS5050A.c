/*
 * AS5050A.c
 *
 *  Created on: 04 Feb 2016
 *      Author: raffael
 *      based on AS5050-AB-v1.1 Adapterboard OPERATION MANUAL, 2010, ams AG
 */

#include "../../../Board.h"
#include "spi_helper.h"
#include "AS5050A.h"

/*! *****************************************************************************
* Reads out chip data via SPI interface
*
* This function is used to read out cordic value from chips supporting SPI
* interface. ***************************************************************************** */
#define SPI_CMD_NOP		0x0000
#define SPI_CMD_READ 	0x8000  /*!< flag indicating read attempt when using SPI interface */
#define SPI_REG_DATA 	0x7ffe	/*!< data register when using SPI */
#define SPI_REG_AGC  	0x7ff0	/*!< agc register when using SPI */
#define SPI_REG_CLRERR 	0x6700	/*!< clear error register when using SPI */


#define CMD_NOP				0x0000
#define CMD_READ 			0x8000

#define REG_POR_OFF			0x3F22 // 7:0, R/W, Reset=0x0000 Power On Reset OFF
#define REG_SOFTWARE_RESET	0x3C00 // 13:0, W, Reset=0x0000 Software Reset
#define REG_MASTER_RESET	0x33A5 // 13:0, W, Reset=0x0000 Master Reset
#define REG_CLEAR_EF		0x3380 // 13:0, R, Reset=0x0000 Clear Error Flag
#define REG_NOP				0x0000 // 13:0, W, Reset=0x0000 No Operation
#define REG_AGC				0x3FF8 // 5:0, R/W, Reset=0x0020 Automatic Gain Control
#define REG_DATA			0x3FFF // 13:0, R, Reset=0x0000 Measured Angle
#define REG_ERROR			0x335A // 13:0, R, Reset=0x0000 Error Status Register
#define REG_CONFIG			0x3F20 // 9:13, R/W, Reset=0x000 System Configuration Register 1

#define ERROR_FLAG 			0x0002

static uint8_t spiCalcEvenParity(uint16_t value);

#if 0

// as5050 data exchange, with parity bit check
static bool as5050_exchange(uint16_t *txp, uint16_t *rxp) {
    uint8_t tx[2], rx[2];
	uint16_t data = CMD_NOP;
	if (txp != NULL) {
		data = *txp;
	}
    tx[0] = (data>>8) & 0xff;
    tx[1] = data & 0xff | spiCalcEvenParity(data);
    spi_helper_transfer(sizeof(tx), tx, rx, Board_M1_ANGLE_ENCODER_CS);
    if (rxp != NULL) {
		*rxp = (uint16_t)rx[1] | ((uint16_t)rx[0]<<8);
		// if (spiCalcEvenParity(*rxp) != 0) {
		// 	return false;
		// }
	}
    return true;
}

static void as5050_command(uint16_t command) {
	as5050_exchange(&command, NULL);
}

static bool as5050_read(uint16_t *ptr) {
	return as5050_exchange(NULL, ptr);
}

static bool as5050_read_reg(uint16_t addr, uint16_t *data) {
	uint16_t command, rx;
	command = (addr<<1) | CMD_READ;
	if (!as5050_exchange(&command, &rx)) {
		return false;
	}
	command = CMD_NOP;
	if (!as5050_exchange(&command, &rx)) {
		return false;
	} else if (rx & ERROR_FLAG) {
		// clear error flag
		cli_printf("error flag ");
		as5050_command(CMD_READ | REG_CLEAR_EF);
		return false;
	}
	*data = rx>>2;
	return true;
}

static bool as5050_write_reg(uint16_t addr, uint16_t data) {
	uint16_t command, dummy;
	if (addr > 0x3fff) {
		return false;
	}
	command = (addr<<1) & 0x7fff;
	if (!as5050_exchange(&command, &dummy)) {
		return false;
	}
	command = (data<<2);
	if (!as5050_exchange(&command, &dummy)) {
		return false;
	}
	return true;
}

bool as5050_read_data(uint16_t* angle) {
	uint16_t agc;
	if (!as5050_read_reg(REG_AGC, &agc)) {
		cli_printf("AGC ");
		return false;
	}
	as5050_command(CMD_READ | REG_DATA);
	Task_sleep(1);
	uint16_t data;
	if (!as5050_read(&data)) {
		cli_printf("adc data ");
		return false;
	}
	if (data & ERROR_FLAG) {
		cli_printf("error flag ");
		as5050_command(CMD_READ | REG_CLEAR_EF);
		return false;
	}
	data = data>>2;


	*angle = data & 0x3ff;
	uint16_t signal_too_low = data & (1<<13);
	uint16_t signal_too_high = data & (1<<12);
	if (signal_too_low) {
		cli_printf("signal too low, %u\n", *angle);
	} else if (signal_too_high) {
		cli_printf("signal too high, %u\n", *angle);
	}
	return true;
}

#else

static uint16_t as5050_exchange(uint16_t tx_dat)
{
    uint8_t tx[2], rx[2];
    tx_dat |= spiCalcEvenParity(tx_dat);
    tx[0] = (tx_dat>>8) & 0xff;
    tx[1] = tx_dat & 0xff;
    spi_helper_transfer(sizeof(tx), tx, rx, Board_M1_ANGLE_ENCODER_CS);
    return (uint16_t)rx[1] | ((uint16_t)rx[0]<<8);
}

bool as5050_read_data(uint16_t* angle) {
	uint16_t rx_dat; // 16-bit data buffer for SPI communication
    uint16_t agcreg;
    uint16_t agc;
    uint16_t value;
    uint16_t alarmHi, alarmLo;

/* Send READ AGC command. Received data is thrown away: this data comes from the precedent command (unknown)*/
    as5050_exchange(SPI_CMD_READ | SPI_REG_AGC);

/* Send READ ANGLE command. Received data is the AGC value, from the precedent command */
    agcreg = as5050_exchange(SPI_CMD_READ | SPI_REG_DATA);

    Task_sleep(1);
/* Send NOP command. Received data is the ANGLE value, from the precedent command */
    rx_dat = as5050_exchange(SPI_CMD_NOP);

    if (((rx_dat >> 1) & 0x1) || ((agcreg >> 1) & 0x1))
    {
		as5050_exchange(SPI_CMD_READ | (REG_ERROR<<1));
		uint16_t err = as5050_exchange(SPI_CMD_NOP);
		cli_printf("err reg %x\n", err);
		/* error flag set - need to reset it */
		as5050_exchange(SPI_CMD_READ | SPI_REG_CLRERR);
		// cli_printf("error flag\n");
		return false;
    }
    else
    {
		agc = (agcreg >> 2) & 0x3f; 		// AGC value (0..63)
		value = (rx_dat >> 2) & 0x3ff;		// Angle value (0..4095 for AS5055)
		*angle = ((uint32_t)value * 360) / 0x3ff; 	// Angle value in degree (0..359.9°)
		alarmLo = (rx_dat >> 15) & 0x1;
		alarmHi = (rx_dat >> 14) & 0x1;
		if (alarmLo && alarmHi) {
			as5050_exchange(SPI_CMD_READ | (REG_ERROR<<1));
			uint16_t err = as5050_exchange(SPI_CMD_NOP);
			as5050_exchange(SPI_CMD_READ | SPI_REG_CLRERR);
			cli_printf("sys err %x %x\n", rx_dat, err>>2);
		} else if (alarmLo) {
			cli_printf("signal too low, %u\n", value);
		} else if (alarmHi) {
			cli_printf("signal too high, %u\n", value);
		}
    }
	return true;
}
#endif

/*! *****************************************************************************
* Calculate even parity of a 16 bit unsigned integer
*
* This function is used by the SPI interface to calculate the even parity
* of the data which will be sent via SPI to the encoder.
*
* \param[in] value : 16 bit unsigned integer whose parity shall be calculated *
* \return : Even parity
* ***************************************************************************** */
static uint8_t spiCalcEvenParity(uint16_t value) {
	uint8_t cnt = 0;
	uint8_t i;

    for (i = 0; i < 16; i++)
    {
        if (value & 0x1)
        {
        		cnt++;
        }
        value = value >> 1;
    }
    return (cnt & 0x1);
}







