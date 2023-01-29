#include "return_indices.h"

/*constant values for all experiments initialised here*/
uint8_t EXP_NUM_ROW = 1;
uint8_t EXP_NUM_COL = 1;
uint8_t ETX_ROW = 14;
uint8_t ETX_COL = 2;
uint8_t CRC_ROW = 14;
uint8_t CRC_COL = 3;
uint8_t CR_ROW = 15;
uint8_t CR_COL = 0;
uint8_t ETX1_ROW = 15;
uint8_t ETX1_COL = 1;
uint8_t PAGE_NUM_ROW = 15;
uint8_t PAGE_NUM_COL = 2;
uint8_t CR1_ROW = 15;
uint8_t CR1_COL = 3;


/*Variables.. Should be initialised to 0 and taken dynamicaly depending on the experiment*/
uint8_t NUM_STEPS_ROW = 1;
uint8_t NUM_STEPS_COL = 2;
uint8_t VOLTAGE1 = 2;
uint8_t VOLTAGE_DURATION1 = 3;
uint8_t VOLTAGE2 = 5;
uint8_t VOLTAGE_DURATION2 = 6;
uint8_t VOLTAGE3 = 8;
uint8_t VOLTAGE_DURATION3 = 9;
uint8_t VOLTAGE4 = 11;
uint8_t VOLTAGE_DURATION4 = 12;
uint8_t OCP1_ROW = 4;
uint8_t OCP1_COL = 0;
uint8_t OCP1_COLLECT_ROW = 4;
uint8_t OCP1_COLLECT_COL = 1;
uint8_t OCP2_ROW = 7;
uint8_t OCP2_COL = 0;
uint8_t OCP2_COLLECT_ROW = 7;
uint8_t OCP2_COLLECT_COL = 1;
uint8_t OCP3_ROW = 10;
uint8_t OCP3_COL = 0;
uint8_t OCP3_COLLECT_ROW = 10;
uint8_t OCP3_COLLECT_COL = 1;
uint8_t OCP4_ROW = 13;
uint8_t OCP4_COL = 0;
uint8_t OCP4_COLLECT_ROW = 13;
uint8_t OCP4_COLLECT_COL = 1;
uint8_t SAMPLING_RATE_ROW = 14;
uint8_t SAMPLING_RATE_COL = 0;
uint8_t NUM_CYCLES_ROW = 14;
uint8_t NUM_CYCLES_COL = 1;

uint8_t OCP_MEAS_MODE_ROW = 1;
uint8_t OCP_MEAS_MODE_COL = 2;
uint8_t OCP_MEAS_PRD_ROW = 1;
uint8_t OCP_MEAS_PRD_COL = 3;
uint8_t OCP_MEAS_SLOPE = 2;
uint8_t WE_NUM_ROW = 3;
uint8_t WE_NUM_COL = 0;
uint8_t INITIAL_VOLTAGE = 4;
uint8_t VS_OCP_ROW = 5;
uint8_t VS_OCP_COL = 0;
uint8_t INITIAL_VOLT_DURATION = 6;
uint8_t CURRENT_RANGE_ROW = 7;
uint8_t CURRENT_RANGE_COL = 0;
uint8_t AUTO_RANGE_ROW = 7;
uint8_t AUTO_RANGE_COL = 1;
uint8_t POST_CELL_EXP_STATE_ROW = 7;
uint8_t POST_CELL_EXP_STATE_COL = 2;
uint8_t	HOURS_ROW = 1;
uint8_t	HOURS_COL = 1;
uint8_t	MINUTES_ROW = 1;
uint8_t	MINUTES_COL = 2;
uint8_t	SECONDS_ROW = 1;
uint8_t	SECONDS_COL = 3;
uint8_t SWEEP_POTENTIAL1 = 0;
uint8_t SWEEP_POTENTIAL2 = 0;
uint8_t SWEEP_POTENTIAL3 = 0;
uint8_t SWEEP_POTENTIAL4 = 0;
uint8_t VSOCP_SWEEP1 = 0;
uint8_t VSOCP_SWEEP2 = 0;
uint8_t VSOCP_SWEEP1_ROW = 0;
uint8_t VSOCP_SWEEP1_COL = 0;
uint8_t VSOCP_SWEEP2_ROW = 0;
uint8_t VSOCP_SWEEP2_COL = 0;
uint8_t VSOCP_SWEEP3_ROW = 0;
uint8_t VSOCP_SWEEP3_COL = 0;
uint8_t VSOCP_SWEEP4_ROW = 0;
uint8_t VSOCP_SWEEP4_COL = 0;
uint8_t SWEEP_RATE = 0;
uint8_t VOLTS_PER_POINT = 0;
uint8_t STEP_TIME = 0;
uint8_t PULSE_HEIGHT = 0;
uint8_t PULSE_WIDTH = 0;
uint8_t SWEEP_INCREMENT = 0;
uint8_t FREQUENCY = 0;

void chronoamperometry_page0(void){
   NUM_STEPS_ROW = 1;
   NUM_STEPS_COL = 2;
   VOLTAGE1 = 2;
   VOLTAGE_DURATION1 = 3;
   VOLTAGE2 = 5;
   VOLTAGE_DURATION2 = 6;
   VOLTAGE3 = 8;
   VOLTAGE_DURATION3 = 9;
   VOLTAGE4 = 11;
   VOLTAGE_DURATION4 = 12;
   OCP1_ROW = 4;
   OCP1_COL = 0;
   OCP1_COLLECT_ROW = 4;
   OCP1_COLLECT_COL = 1;
   OCP2_ROW = 7;
   OCP2_COL = 0;
   OCP2_COLLECT_ROW = 7;
   OCP2_COLLECT_COL = 1;
   OCP3_ROW = 10;
   OCP3_COL = 0;
   OCP3_COLLECT_ROW = 10;
   OCP3_COLLECT_COL = 1;
   OCP4_ROW = 13;
   OCP4_COL = 0;
   OCP4_COLLECT_ROW = 13;
   OCP4_COLLECT_COL = 1;
//   SAMPLING_RATE_ROW = 14;		//don't have to modify in esp code OR send 0
//   SAMPLING_RATE_COL = 0;
   NUM_CYCLES_ROW = 14;
   NUM_CYCLES_COL = 1;
   ETX_ROW = 14;
   ETX_COL = 2;
   CRC_ROW = 14;
   CRC_COL = 3;
   CR_ROW = 15;
   CR_COL = 0;
   ETX1_ROW = 15;
   ETX1_COL = 1;
   PAGE_NUM_ROW = 15;
   PAGE_NUM_COL = 2;
   CR1_ROW = 15;
   CR1_COL = 3;
}

void chronoamperometry_page1(void){
   OCP_MEAS_MODE_ROW = 1;
   OCP_MEAS_MODE_COL = 2;
   OCP_MEAS_PRD_ROW = 1;
   OCP_MEAS_PRD_COL = 3;
   OCP_MEAS_SLOPE = 2;
   WE_NUM_ROW = 3;
   WE_NUM_COL = 0;
   INITIAL_VOLTAGE = 4;
   VS_OCP_ROW = 5;
   VS_OCP_COL = 0;
   INITIAL_VOLT_DURATION = 6;
   CURRENT_RANGE_ROW = 7;
   CURRENT_RANGE_COL = 0;
   AUTO_RANGE_ROW = 7;
   AUTO_RANGE_COL = 1;
   POST_CELL_EXP_STATE_ROW = 7;
   POST_CELL_EXP_STATE_COL = 2;
	// [7][3] needs to be sent just as a dummy valu, not used here
   SAMPLING_RATE_ROW = 8;//this needs to be float
   ETX_ROW = 9;
   ETX_COL = 1;
   CRC_ROW = 9;
   CRC_COL = 2;
   CR_ROW = 9;
   CR_COL = 3;
   ETX1_ROW = 10;
   ETX1_COL = 0;
   PAGE_NUM_ROW = 10;
   PAGE_NUM_COL = 1;
   CR1_ROW = 10;
   CR1_COL = 2;
}


void chrono_ocp_page0(void){
	EXP_NUM_ROW = 1;
	EXP_NUM_COL = 1;
//	SAMPLING_RATE_ROW = 1; //don't have to modify in esp code OR send 0
//	SAMPLING_RATE_COL = 2;
	HOURS_ROW = 1;
	HOURS_COL = 3;
	MINUTES_ROW = 2;
	MINUTES_COL = 0;
	SECONDS_ROW = 2;
	SECONDS_COL = 1;
	//send dummy values for [2][2] and [2][3] and actual sampling rate for [3]..
	SAMPLING_RATE_ROW = 3;
}

void linear_pulse_voltammetry_page0(){
	OCP_MEAS_MODE_ROW = 1;
	OCP_MEAS_MODE_COL = 2;
	OCP_MEAS_PRD_ROW = 1;
	OCP_MEAS_PRD_COL = 3;
	OCP_MEAS_SLOPE = 2;
	INITIAL_VOLTAGE = 3;
	INITIAL_VOLT_DURATION = 4;
	SWEEP_POTENTIAL1 = 5;
	SWEEP_POTENTIAL2 = 6;
	VSOCP_SWEEP1_ROW = 7;
	VSOCP_SWEEP1_COL = 0;
	VSOCP_SWEEP2_ROW = 7;
	VSOCP_SWEEP2_COL = 1;
	SWEEP_RATE = 8;
	VOLTS_PER_POINT = 9;
	VS_OCP_ROW = 10;
	VS_OCP_COL = 0;
	CURRENT_RANGE_ROW = 10;
	CURRENT_RANGE_COL = 1;
	AUTO_RANGE_ROW = 10;
	AUTO_RANGE_COL = 2;
	WE_NUM_ROW = 10;
	WE_NUM_COL = 3;	
	POST_CELL_EXP_STATE_ROW = 11;
	POST_CELL_EXP_STATE_COL = 0;
}

void cyclic_voltammetry_page0(){
	NUM_CYCLES_ROW = 1;
	NUM_CYCLES_COL = 2;
	OCP_MEAS_MODE_ROW = 1;
	OCP_MEAS_MODE_COL = 3;
	OCP_MEAS_PRD_ROW = 2;
	OCP_MEAS_PRD_COL = 0;
	OCP_MEAS_SLOPE = 3;
	INITIAL_VOLTAGE = 4;
	INITIAL_VOLT_DURATION = 5;
	SWEEP_POTENTIAL1 = 6;
	SWEEP_POTENTIAL2 = 7;
	SWEEP_POTENTIAL3 = 8;
	SWEEP_POTENTIAL4 = 9;
	VSOCP_SWEEP1_ROW = 10;
	VSOCP_SWEEP1_COL = 0;
	VSOCP_SWEEP2_ROW = 10;
	VSOCP_SWEEP2_COL = 1;
	VSOCP_SWEEP3_ROW = 10;
	VSOCP_SWEEP3_COL = 2;
	VSOCP_SWEEP4_ROW = 10;
	VSOCP_SWEEP4_COL = 3;
	SWEEP_RATE = 11;
	VOLTS_PER_POINT = 12;
	VS_OCP_ROW = 13;
	VS_OCP_COL = 0;
	AUTO_RANGE_ROW = 13;
	AUTO_RANGE_COL = 1;
	WE_NUM_ROW = 13;
	WE_NUM_COL = 2;
	CURRENT_RANGE_ROW = 13;
	CURRENT_RANGE_COL = 3;	
	POST_CELL_EXP_STATE_ROW = 14;
	POST_CELL_EXP_STATE_COL = 0;
}

void differential_pulse_voltammetry_page0(){
	OCP_MEAS_MODE_ROW = 1;
	OCP_MEAS_MODE_COL = 2;
	OCP_MEAS_PRD_ROW = 1;
	OCP_MEAS_PRD_COL = 3;
	OCP_MEAS_SLOPE = 2;
	INITIAL_VOLTAGE = 3;
	INITIAL_VOLT_DURATION = 4;
	SWEEP_POTENTIAL1 = 5;
	SWEEP_POTENTIAL2 = 6;
	VSOCP_SWEEP1_ROW = 7;
	VSOCP_SWEEP1_COL = 0;
	VSOCP_SWEEP2_ROW = 7;
	VSOCP_SWEEP2_COL = 1;
	VS_OCP_ROW = 7;
	VS_OCP_COL = 2;
	SWEEP_RATE = 8;
	SWEEP_INCREMENT = 9;
	STEP_TIME = 10;
	PULSE_HEIGHT = 11;
	PULSE_WIDTH = 12;
	WE_NUM_ROW = 13;
	WE_NUM_COL = 0;
	CURRENT_RANGE_ROW = 13;
	CURRENT_RANGE_COL = 1;	
	POST_CELL_EXP_STATE_ROW = 13;
	POST_CELL_EXP_STATE_COL = 2;
}

void normal_pulse_voltammetry_page0(){
	OCP_MEAS_MODE_ROW = 1;
	OCP_MEAS_MODE_COL = 2;
	OCP_MEAS_PRD_ROW = 1;
	OCP_MEAS_PRD_COL = 3;
	OCP_MEAS_SLOPE = 2;
	INITIAL_VOLTAGE = 3;
	INITIAL_VOLT_DURATION = 4;
	SWEEP_POTENTIAL1 = 5;
	SWEEP_POTENTIAL2 = 6;
	VSOCP_SWEEP1_ROW = 7;
	VSOCP_SWEEP1_COL = 0;
	VSOCP_SWEEP2_ROW = 7;
	VSOCP_SWEEP2_COL = 1;
	VS_OCP_ROW = 7;
	VS_OCP_COL = 2;
	SWEEP_RATE = 8;
	SWEEP_INCREMENT = 9;
	STEP_TIME = 10;
	PULSE_WIDTH = 11;
	WE_NUM_ROW = 12;
	WE_NUM_COL = 0;
	CURRENT_RANGE_ROW = 12;
	CURRENT_RANGE_COL = 1;	
	POST_CELL_EXP_STATE_ROW = 12;
	POST_CELL_EXP_STATE_COL = 2;
}

void squarewave_voltammetry_page0(){
	OCP_MEAS_MODE_ROW = 1;
	OCP_MEAS_MODE_COL = 2;
	OCP_MEAS_PRD_ROW = 1;
	OCP_MEAS_PRD_COL = 3;
	OCP_MEAS_SLOPE = 2;
	INITIAL_VOLTAGE = 3;
	INITIAL_VOLT_DURATION = 4;
	SWEEP_POTENTIAL1 = 5;
	SWEEP_POTENTIAL2 = 6;
	VSOCP_SWEEP1_ROW = 7;
	VSOCP_SWEEP1_COL = 0;
	VSOCP_SWEEP2_ROW = 7;
	VSOCP_SWEEP2_COL = 1;
	VS_OCP_ROW = 7;
	VS_OCP_COL = 2;
	PULSE_HEIGHT = 8;
	FREQUENCY = 9;
	SWEEP_INCREMENT = 10;
	AUTO_RANGE_ROW = 11;
	AUTO_RANGE_COL = 0;
	WE_NUM_ROW = 11;
	WE_NUM_COL = 1;
	CURRENT_RANGE_ROW = 11;
	CURRENT_RANGE_COL = 2;	
	POST_CELL_EXP_STATE_ROW = 11;
	POST_CELL_EXP_STATE_COL = 3;
}