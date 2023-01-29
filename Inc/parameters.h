#ifndef _PARAMETERS_H
#define _PARAMETERS_H

#include "main.h"
#include "stm32l4xx.h"
#include "common_struct.h"
#include "return_indices.h"
#include "spi.h"

uint8_t check_header_footer ( test_parameters *table );
void get_exp_indices(test_parameters *table);
void extract_exp_values(test_parameters *table, uint8_t);
void set_gains(uint8_t);
void reset_gains(void);
void write_mux_gains(GPIO_PinState, GPIO_PinState, GPIO_PinState, GPIO_PinState, GPIO_PinState);
void set_adc_meas_gains(uint8_t byte0, uint8_t byte1, uint8_t byte2);
void get_common_exp_data(void);

#endif
