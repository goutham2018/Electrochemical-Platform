#ifndef _COMMON_STRUCT_H
#define _COMMON_STRUCT_H

#include "stm32l4xx.h"

#define FRAME_SIZE		                    64

//Experiment numbers 
#define CHRONOAMPEROMETRY                 1
#define CHRONOPOTENTIOMETRY               2
#define NORMAL_PULSE_VOLTAMMETRY          3
#define SQUAREWAVE_VOLTAMMETRY            4
#define CHRONO_OCP                        5
#define CHRONO_VAUX                       6
#define LINEAR_PULSE_VOLTAMMETRY          7
#define DIFFERENTIAL_PULSE_VOLTAMMETRY    8
#define CYCLIC_VOLTAMMETRY                9
#define SEC_TO_MILLIS					  1000
#define	ONE_BY_2P5						  0.4

//template for setting gains (SS, SS7, SS8, SS10, SS11)
#define CHRONOAMPEROMETRY_GAINS    GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define CHRONOPOTENTIOMETRY_GAINS  GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET
#define NORMAL_PULSE_GAINS         GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define SQUAREWAVE_GAINS           GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define CHRONO_OCP_GAINS           GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET
#define CHRONO_VAUX_GAINS          GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define LINEAR_PULSE_GAINS         GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define DIFFERENTIAL_PULSE_GAINS   GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define CYCLIC_GAINS               GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_RESET
#define RESET_GAINS                GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET
#define MEASURE_OFFSET			   GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_RESET


typedef union {
	unsigned char _char[4];
	float _float;
	uint32_t _int_32;
}test_parameters;
extern test_parameters parameter_table[32];

typedef union {
  uint32_t hex_page[64];
  float float_page[64];
}ram_page;
extern ram_page page1, page2/*, page2esp*/;

typedef struct {
    uint8_t exp_num;
    uint8_t num_steps;
    uint8_t ocp1;
    uint8_t ocp2;
    uint8_t ocp3;
    uint8_t ocp4;
    uint8_t ocp_collect1;
    uint8_t ocp_collect2;
    uint8_t ocp_collect3;
    uint8_t ocp_collect4;
    float sampling_rate;
    uint8_t num_cycles;
    uint8_t ocp_meas_mode;
    uint8_t ocp_meas_period;
    uint8_t we_num;
    uint8_t vs_ocp;
    float voltage1;
    float voltage2;
    float voltage3;
    float voltage4;
    float voltage_duration1;
    float voltage_duration2;
    float voltage_duration3;
    float voltage_duration4;
    float ocp_meas_slope;
    float initial_voltage;
    float initial_voltage_duration;
    float total_duration;
}experiment_settings;
extern experiment_settings exp_new;


typedef struct {
    uint8_t exp_num;
    float sampling_rate;
	uint32_t duration;
}experiment_settings_chrono_ocp;
extern experiment_settings_chrono_ocp exp_chrono_ocp;

typedef struct {
	float sweep_potential1;
	float sweep_potential2;
	float vsocp_sweep1;
	float vsocp_sweep2;
	float sweep_rate;
	float volts_per_point;
	uint32_t sweepDuration;
	float convertedInitPot;
	float convertedSweepPot1;
	float convertedSweepPot2;
	float delTime;
}experiment_settings_linear_pulse;
extern experiment_settings_linear_pulse exp_linear_pulse;

typedef struct {
	uint8_t num_cycles;
	float sweep_potential1;
	float sweep_potential2;
	float sweep_potential3;
	float sweep_potential4;
	uint8_t vsocp_sweep1;
	uint8_t vsocp_sweep2;
	uint8_t vsocp_sweep3;
	uint8_t	vsocp_sweep4;
	float sweep_rate;
	float volts_per_point;
	uint32_t sweepDuration1;
	uint32_t sweepDuration2;
	uint32_t sweepDuration3;
	float convertedInitPot;
	float convertedSweepPot1;
	float convertedSweepPot2;
	float convertedSweepPot3;
	float convertedSweepPot4;
	float delTime;
	int8_t sign1;
	int8_t sign2;
	int8_t sign3;
}experiment_settings_cyclic_voltammetry;
extern experiment_settings_cyclic_voltammetry exp_cyclic_voltammetry;

typedef struct {
	uint32_t timer1;
	uint32_t timer2;
	uint32_t timer3;
	uint32_t timer4;
	uint32_t timer5;
}timers;
extern timers CV;

typedef struct {
	float sweep_potential1;
	float sweep_potential2;
	uint8_t vsocp_sweep1;
	uint8_t vsocp_sweep2;
	float sweep_rate;
	float sweep_increment;
	float step_time;
	float pulse_width;
	float convertedInitPot;
	float convertedSweepPot1;
	float convertedSweepPot2;
}experiment_settings_normal_pulse_voltammetry;
extern experiment_settings_normal_pulse_voltammetry exp_normal_pulse_voltammetry;

typedef struct {
	float sweep_potential1;
	float sweep_potential2;
	uint8_t vsocp_sweep1;
	uint8_t vsocp_sweep2;
	float sweep_rate;
	float sweep_increment;
	float step_time;
	float pulse_height;
	float pulse_width;
	float convertedInitPot;
	float convertedSweepPot1;
	float convertedSweepPot2;
}experiment_settings_differential_pulse_voltammetry;
extern experiment_settings_differential_pulse_voltammetry exp_diff_pulse_voltammetry;

typedef struct {
	float sweep_potential1;
	float sweep_potential2;
	uint8_t vsocp_sweep1;
	uint8_t vsocp_sweep2;
	float pulse_height;
	float frequency;
	float sweep_increment;
	float convertedInitPot;
	float convertedSweepPot1;
	float convertedSweepPot2;
}experiment_settings_squarewave_voltammetry;
extern experiment_settings_squarewave_voltammetry exp_squarewave_voltammetry;


typedef struct {
	uint8_t current_range;
	uint8_t auto_range;
	uint8_t we_num;	
	uint8_t post_exp_cell_state;
}misc_fields;
extern misc_fields misc;

typedef struct {
	uint8_t exp_num;
	uint8_t ocp_meas_mode;
	uint16_t ocp_meas_prd;
	float ocp_meas_slope;
	float initial_voltage;
	float initial_volt_duration;
    uint8_t vs_ocp;

}common_fields;
extern common_fields common;

typedef struct {
	uint32_t timer1;
	uint32_t timer2;
	uint32_t expDuration;
	float OnTime;
	float OffTime;
	uint32_t num_points;
}svTimers;
extern svTimers SV;
#endif
