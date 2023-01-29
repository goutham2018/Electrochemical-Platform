#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "main.h"
#include "stm32l4xx.h"
#include "spi.h"
#include "flash.h"
#include "parameters.h"
#include "common_struct.h"

enum states {STANDBY = 0, TEST_SET = 1, ACTIVE = 2, FAILURE = 3};
void state_machine (void);
void state_standby(void);
void state_test_set(void);
void state_active(void);
void state_failure(void);
void set_operating_state(uint8_t);
uint8_t get_operating_state(void);
float get_updated_values(uint8_t, uint8_t);
void put_ocp(float);
float get_ocp(uint8_t);
float measure_ocp(void);
float chrono_amp_pot(float ocp_measured, uint8_t step, uint8_t index);
float get_dac_value(float, const float, uint8_t, float, uint8_t);
void run_chrono_amp_pot(void);
void run_chrono_ocp(void);
void run_linear_pulse(void);
void set_output(float);
void run_cyclic_voltammetry(void);
void run_squarewave_voltammetry(void);
void run_normal_pulse_voltammetry(void);
void run_diff_pulse_voltammetry(void);
uint16_t local_min_sat(uint16_t x, uint16_t y);
void default_output_state(void);
float get_ocp_value(uint8_t ocp_needed, float ocp_value);
void comms(void);
void get_ocp_offset();
uint32_t get_exp_duration(float, float, float, float, float, float, float, float);




extern int flag_esp_write;
extern int flag_ocp_acq_inProgress;
extern _Bool flag_flash_write;
extern uint8_t completed_page;
extern uint32_t adc_sampling_time;
extern bool measActive;
extern uint32_t ocp_offset;
extern bool measure_ocp_offset;
#endif
