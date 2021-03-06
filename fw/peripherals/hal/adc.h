/*
 * adc.h
 *
 *  Created on: 16 Dec 2015
 *      Author: raffael
 */

#ifndef FW_PERIPHERALS_HAL_ADC_H_
#define FW_PERIPHERALS_HAL_ADC_H_

#include <stdint.h>

#define ADC_SUCCESS 	1
#define ADC_ERROR	0
#define N_ADC_AVG_WHEEL	10 //number of samples to average
#define N_ADC_AVG_STRUT	5


void adc_isr();
uint8_t adc_read_motor_sensors(uint16_t wheel_sensor_values[]);
uint8_t adc_read_strut_sensor_values(uint16_t strut_sensor_values[]);


void adc_init(void);


#endif /* FW_PERIPHERALS_HAL_ADC_H_ */
