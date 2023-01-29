#include "parameters.h"
experiment_settings exp_new;
experiment_settings_chrono_ocp exp_chrono_ocp;
experiment_settings_linear_pulse exp_linear_pulse;
experiment_settings_cyclic_voltammetry exp_cyclic_voltammetry;
timers CV;
experiment_settings_normal_pulse_voltammetry exp_normal_pulse_voltammetry;
experiment_settings_squarewave_voltammetry exp_squarewave_voltammetry;
experiment_settings_differential_pulse_voltammetry exp_diff_pulse_voltammetry;
common_fields common;
svTimers SV;
misc_fields misc;

static uint8_t pass1 = 0;
uint8_t Data_for_try[3],ret_for_try[3]={0};

uint8_t check_header_footer ( test_parameters *table ){
uint8_t ret_val = 0;
	get_exp_indices(table);
	if (table[0]._char[0] == SOH && table[0]._char[1] == SOH && table[0]._char[2] == SOH2 && 
		table[0]._char[3] == STX && table[1]._char[0] == DEV_ID){
		if(table[ETX_ROW]._char[ETX_COL] == ETX && table[CRC_ROW]._char[CRC_COL] == _CRC && 
			table[CR_ROW]._char[CR_COL] == _CR && table[ETX1_ROW]._char[ETX1_COL] == ETX && table[CR1_ROW]._char[CR1_COL] == _CR){
			ret_val = 1;
			if(table[PAGE_NUM_ROW]._char[PAGE_NUM_COL] == 0){
				pass1 = 1;
			}
		}
	}
  return ret_val;
}

void get_exp_indices(test_parameters *table){
    switch (table[EXP_NUM_ROW]._char[EXP_NUM_COL]){
	case CHRONOAMPEROMETRY:
	case CHRONOPOTENTIOMETRY: 
		if (!pass1){
			chronoamperometry_page0();
		}
		else{
			chronoamperometry_page1();
			pass1 = 0;
		}
	break;
	case CHRONO_OCP:
		chrono_ocp_page0();
    break;
	case LINEAR_PULSE_VOLTAMMETRY:
		linear_pulse_voltammetry_page0();
	break;
	case CYCLIC_VOLTAMMETRY:
		cyclic_voltammetry_page0();
	break;
	case SQUAREWAVE_VOLTAMMETRY:
		squarewave_voltammetry_page0();
	break;
	case NORMAL_PULSE_VOLTAMMETRY:
		normal_pulse_voltammetry_page0();
	break;
	case DIFFERENTIAL_PULSE_VOLTAMMETRY:
		differential_pulse_voltammetry_page0();
    }
}

void extract_exp_values(test_parameters *table, uint8_t page_num) {
  
	if (!page_num){
		switch(table[EXP_NUM_ROW]._char[EXP_NUM_COL]){
        case CHRONOAMPEROMETRY:
        case CHRONOPOTENTIOMETRY:
            exp_new.exp_num = table[EXP_NUM_ROW]._char[EXP_NUM_COL];
            exp_new.num_steps = table[NUM_STEPS_ROW]._char[NUM_STEPS_COL];
            exp_new.ocp1 = table[OCP1_ROW]._char[OCP1_COL];
            exp_new.ocp2 = table[OCP2_ROW]._char[OCP2_COL];
            exp_new.ocp3 = table[OCP3_ROW]._char[OCP3_COL];
            exp_new.ocp4 = table[OCP4_ROW]._char[OCP4_COL];
            exp_new.ocp_collect1 = table[OCP1_COLLECT_ROW]._char[OCP1_COLLECT_COL];
            exp_new.ocp_collect2 = table[OCP2_COLLECT_ROW]._char[OCP2_COLLECT_COL];
            exp_new.ocp_collect3 = table[OCP3_COLLECT_ROW]._char[OCP3_COLLECT_COL];
            exp_new.ocp_collect4 = table[OCP4_COLLECT_ROW]._char[OCP4_COLLECT_COL];
//            exp_new.sampling_rate = table[SAMPLING_RATE_ROW]._char[SAMPLING_RATE_COL];
            exp_new.num_cycles = table[NUM_CYCLES_ROW]._char[NUM_CYCLES_COL];

            exp_new.voltage1 = table[VOLTAGE1]._float;
            exp_new.voltage2 = table[VOLTAGE2]._float;
            exp_new.voltage3 = table[VOLTAGE3]._float;
            exp_new.voltage4 = table[VOLTAGE4]._float;
            exp_new.voltage_duration1 = (uint32_t)table[VOLTAGE_DURATION1]._float * SEC_TO_MILLIS;
            exp_new.voltage_duration2 = (uint32_t)table[VOLTAGE_DURATION2]._float * SEC_TO_MILLIS;
            exp_new.voltage_duration3 = (uint32_t)table[VOLTAGE_DURATION3]._float * SEC_TO_MILLIS;
            exp_new.voltage_duration4 = (uint32_t)table[VOLTAGE_DURATION4]._float * SEC_TO_MILLIS;
            exp_new.total_duration = exp_new.ocp_meas_period + exp_new.initial_voltage_duration + (exp_new.voltage_duration1 + 
                                        exp_new.voltage_duration2 + exp_new.voltage_duration3 + exp_new.voltage_duration4);
        break;
        case NORMAL_PULSE_VOLTAMMETRY:
            get_common_exp_data();
            exp_normal_pulse_voltammetry.sweep_potential1 = table[SWEEP_POTENTIAL1]._float;
            exp_normal_pulse_voltammetry.sweep_potential2 = table[SWEEP_POTENTIAL2]._float;
            exp_normal_pulse_voltammetry.vsocp_sweep1 = table[VSOCP_SWEEP1_ROW]._char[VSOCP_SWEEP1_COL];
            exp_normal_pulse_voltammetry.vsocp_sweep2 = table[VSOCP_SWEEP2_ROW]._char[VSOCP_SWEEP2_COL];
            exp_normal_pulse_voltammetry.sweep_rate = table[SWEEP_RATE]._float;
            exp_normal_pulse_voltammetry.sweep_increment = table[SWEEP_INCREMENT]._float;
            exp_normal_pulse_voltammetry.step_time = table[STEP_TIME]._float;
            exp_normal_pulse_voltammetry.pulse_width  = table[PULSE_WIDTH]._float;
            misc.current_range = table[CURRENT_RANGE_ROW]._char[CURRENT_RANGE_COL];
            misc.we_num = table[WE_NUM_ROW]._char[WE_NUM_COL];	
            misc.post_exp_cell_state = table[POST_CELL_EXP_STATE_ROW]._char[POST_CELL_EXP_STATE_COL];
            exp_normal_pulse_voltammetry.convertedInitPot = (common.initial_voltage + 2.5) / 2;
            exp_normal_pulse_voltammetry.convertedSweepPot1 = (exp_normal_pulse_voltammetry.sweep_potential1 + 2.5) / 2;
            exp_normal_pulse_voltammetry.convertedSweepPot2 = (exp_normal_pulse_voltammetry.sweep_potential2 + 2.5) / 2;
            SV.timer1 = (uint32_t)(common.ocp_meas_prd );
//            SV.num_points = (uint32_t)((exp_normal_pulse_voltammetry.sweep_potential2 - exp_normal_pulse_voltammetry.sweep_potential1) / exp_normal_pulse_voltammetry.sweep_increment);
//            SV.expDuration = (uint32_t)((exp_normal_pulse_voltammetry.step_time * SV.num_points)*SEC_TO_MILLIS);
            SV.timer2 = (uint32_t)( (SV.timer1) + ((common.initial_volt_duration) ) );
            SV.OnTime = exp_normal_pulse_voltammetry.pulse_width * SEC_TO_MILLIS;
            SV.OffTime = (exp_normal_pulse_voltammetry.step_time - exp_normal_pulse_voltammetry.pulse_width) * SEC_TO_MILLIS;
        break;
        case SQUAREWAVE_VOLTAMMETRY:
            get_common_exp_data();
            exp_squarewave_voltammetry.sweep_potential1 = table[SWEEP_POTENTIAL1]._float;
            exp_squarewave_voltammetry.sweep_potential2 = table[SWEEP_POTENTIAL2]._float;
            exp_squarewave_voltammetry.vsocp_sweep1 = table[VSOCP_SWEEP1_ROW]._char[VSOCP_SWEEP1_COL];
            exp_squarewave_voltammetry.vsocp_sweep2 = table[VSOCP_SWEEP2_ROW]._char[VSOCP_SWEEP2_COL];
            exp_squarewave_voltammetry.pulse_height  = table[PULSE_HEIGHT]._float;
            exp_squarewave_voltammetry.frequency = table[FREQUENCY]._float;
            exp_squarewave_voltammetry.sweep_increment = table[SWEEP_INCREMENT]._float;
            misc.current_range = table[CURRENT_RANGE_ROW]._char[CURRENT_RANGE_COL];
            misc.auto_range = table[AUTO_RANGE_ROW]._char[AUTO_RANGE_COL];
            misc.we_num = table[WE_NUM_ROW]._char[WE_NUM_COL];	
            misc.post_exp_cell_state = table[POST_CELL_EXP_STATE_ROW]._char[POST_CELL_EXP_STATE_COL];
            exp_squarewave_voltammetry.convertedInitPot = (common.initial_voltage + 2.5) / 2;
            exp_squarewave_voltammetry.convertedSweepPot1 = (exp_squarewave_voltammetry.sweep_potential1 + 2.5) / 2;
            exp_squarewave_voltammetry.convertedSweepPot2 = (exp_squarewave_voltammetry.sweep_potential2 + 2.5) / 2;
            SV.timer1 = (uint32_t)(common.ocp_meas_prd );
            SV.expDuration = (uint32_t)(((exp_squarewave_voltammetry.sweep_potential2 - exp_squarewave_voltammetry.sweep_potential1
                                + exp_squarewave_voltammetry.pulse_height)/exp_squarewave_voltammetry.sweep_increment)*(1/exp_squarewave_voltammetry.frequency)*SEC_TO_MILLIS);
            SV.timer2 = (uint32_t)( (SV.timer1) + ((common.initial_volt_duration) ) );
            SV.OnTime = SV.OffTime = (2/exp_squarewave_voltammetry.frequency) * SEC_TO_MILLIS;
        break;
        case CHRONO_OCP:
            exp_chrono_ocp.exp_num = table[EXP_NUM_ROW]._char[EXP_NUM_COL];
//            exp_chrono_ocp.sampling_rate = table[SAMPLING_RATE_ROW]._char[SAMPLING_RATE_COL];
            exp_chrono_ocp.duration = ((table[HOURS_ROW]._char[HOURS_COL] * 3600) + (table[MINUTES_ROW]._char[MINUTES_COL] * 60) + table[SECONDS_ROW]._char[SECONDS_COL])*1000;
			exp_chrono_ocp.sampling_rate = table[SAMPLING_RATE_ROW]._float;
        break;
//        case CHRONO_VAUX:

//        break;
        case LINEAR_PULSE_VOLTAMMETRY:
            get_common_exp_data();
            exp_linear_pulse.sweep_potential1 = table[SWEEP_POTENTIAL1]._float;
            exp_linear_pulse.sweep_potential2 = table[SWEEP_POTENTIAL2]._float;
            exp_linear_pulse.vsocp_sweep1 = table[VSOCP_SWEEP1_ROW]._char[VSOCP_SWEEP1_COL];
            exp_linear_pulse.vsocp_sweep2 = table[VSOCP_SWEEP2_ROW]._char[VSOCP_SWEEP2_COL];
            exp_linear_pulse.sweep_rate = table[SWEEP_RATE]._float;
            exp_linear_pulse.volts_per_point = ((table[VOLTS_PER_POINT]._float * 1000) + 0.5)/1000;
            misc.current_range = table[CURRENT_RANGE_ROW]._char[CURRENT_RANGE_COL];
            misc.auto_range = table[AUTO_RANGE_ROW]._char[AUTO_RANGE_COL];
            misc.we_num = table[WE_NUM_ROW]._char[WE_NUM_COL];
            misc.post_exp_cell_state = table[POST_CELL_EXP_STATE_ROW]._char[POST_CELL_EXP_STATE_COL];
            exp_linear_pulse.sweepDuration = (uint32_t)(absVal((exp_linear_pulse.sweep_potential2 - exp_linear_pulse.sweep_potential1)/exp_linear_pulse.sweep_rate)) * SEC_TO_MILLIS;
            exp_linear_pulse.convertedInitPot = (common.initial_voltage + 2.5) / 2;
            exp_linear_pulse.convertedSweepPot1 = (exp_linear_pulse.sweep_potential1 + 2.5) / 2;
            exp_linear_pulse.convertedSweepPot2 = (exp_linear_pulse.sweep_potential2 + 2.5) / 2;
            exp_linear_pulse.delTime =  ((((1/(exp_linear_pulse.sweep_rate / exp_linear_pulse.volts_per_point)) * 1000) - 1.0)/1000) * SEC_TO_MILLIS;

        break;
        case DIFFERENTIAL_PULSE_VOLTAMMETRY:
            get_common_exp_data();
            exp_diff_pulse_voltammetry.sweep_potential1 = table[SWEEP_POTENTIAL1]._float;
            exp_diff_pulse_voltammetry.sweep_potential2 = table[SWEEP_POTENTIAL2]._float;
            exp_diff_pulse_voltammetry.vsocp_sweep1 = table[VSOCP_SWEEP1_ROW]._char[VSOCP_SWEEP1_COL];
            exp_diff_pulse_voltammetry.vsocp_sweep2 = table[VSOCP_SWEEP2_ROW]._char[VSOCP_SWEEP2_COL];
            exp_diff_pulse_voltammetry.sweep_rate = table[SWEEP_RATE]._float;
            exp_diff_pulse_voltammetry.sweep_increment = table[SWEEP_INCREMENT]._float;
            exp_diff_pulse_voltammetry.pulse_height = table[PULSE_HEIGHT]._float;
            exp_diff_pulse_voltammetry.step_time = table[STEP_TIME]._float;
            exp_diff_pulse_voltammetry.pulse_width = table[PULSE_WIDTH]._float;
            misc.current_range = table[CURRENT_RANGE_ROW]._char[CURRENT_RANGE_COL];
            misc.we_num = table[WE_NUM_ROW]._char[WE_NUM_COL];	
            misc.post_exp_cell_state = table[POST_CELL_EXP_STATE_ROW]._char[POST_CELL_EXP_STATE_COL];
            exp_diff_pulse_voltammetry.convertedInitPot = (common.initial_voltage + 2.5) / 2;
            exp_diff_pulse_voltammetry.convertedSweepPot1 = (exp_diff_pulse_voltammetry.sweep_potential1 + 2.5) / 2;
            exp_diff_pulse_voltammetry.convertedSweepPot2 = (exp_diff_pulse_voltammetry.sweep_potential2 + 2.5) / 2;
            SV.timer1 = (uint32_t)(common.ocp_meas_prd);
//            SV.num_points = (uint32_t)((exp_diff_pulse_voltammetry.sweep_potential2 - exp_diff_pulse_voltammetry.sweep_potential1) / exp_diff_pulse_voltammetry.sweep_increment);				
//            SV.expDuration = (uint32_t)(exp_diff_pulse_voltammetry.step_time * SV.num_points *SEC_TO_MILLIS);
            SV.timer2 = (uint32_t)( (SV.timer1) + ((common.initial_volt_duration)) );
            SV.OnTime = (exp_diff_pulse_voltammetry.pulse_width) * SEC_TO_MILLIS;
            SV.OffTime = (exp_diff_pulse_voltammetry.step_time*SEC_TO_MILLIS - SV.OnTime);
        break;
        case CYCLIC_VOLTAMMETRY:
            get_common_exp_data();
            exp_cyclic_voltammetry.num_cycles = table[NUM_CYCLES_ROW]._char[NUM_CYCLES_COL];
            exp_cyclic_voltammetry.sweep_potential1 = table[SWEEP_POTENTIAL1]._float;
            exp_cyclic_voltammetry.sweep_potential2 = table[SWEEP_POTENTIAL2]._float;
            exp_cyclic_voltammetry.sweep_potential3 = table[SWEEP_POTENTIAL3]._float;
            exp_cyclic_voltammetry.sweep_potential4 = table[SWEEP_POTENTIAL4]._float;
            exp_cyclic_voltammetry.vsocp_sweep1 = table[VSOCP_SWEEP1_ROW]._char[VSOCP_SWEEP1_COL];
            exp_cyclic_voltammetry.vsocp_sweep2 = table[VSOCP_SWEEP2_ROW]._char[VSOCP_SWEEP2_COL];
            exp_cyclic_voltammetry.vsocp_sweep3 = table[VSOCP_SWEEP3_ROW]._char[VSOCP_SWEEP3_COL];
            exp_cyclic_voltammetry.vsocp_sweep4 = table[VSOCP_SWEEP4_ROW]._char[VSOCP_SWEEP4_COL];
            exp_cyclic_voltammetry.sweep_rate = table[SWEEP_RATE]._float;
            exp_cyclic_voltammetry.volts_per_point = ((table[VOLTS_PER_POINT]._float * 1000) + 0.5)/1000;
            misc.current_range = table[CURRENT_RANGE_ROW]._char[CURRENT_RANGE_COL];
            misc.auto_range = table[AUTO_RANGE_ROW]._char[AUTO_RANGE_COL];
            misc.we_num = table[WE_NUM_ROW]._char[WE_NUM_COL];
            misc.post_exp_cell_state = table[POST_CELL_EXP_STATE_ROW]._char[POST_CELL_EXP_STATE_COL];
            
            exp_cyclic_voltammetry.sweepDuration1 = (uint32_t)absVal((exp_cyclic_voltammetry.sweep_potential2 - exp_cyclic_voltammetry.sweep_potential1)/exp_cyclic_voltammetry.sweep_rate) * SEC_TO_MILLIS;
            exp_cyclic_voltammetry.sweepDuration2 = (uint32_t)absVal((exp_cyclic_voltammetry.sweep_potential3 - exp_cyclic_voltammetry.sweep_potential2)/exp_cyclic_voltammetry.sweep_rate) * SEC_TO_MILLIS;
            exp_cyclic_voltammetry.sweepDuration3 = (uint32_t)absVal((exp_cyclic_voltammetry.sweep_potential4 - exp_cyclic_voltammetry.sweep_potential3)/exp_cyclic_voltammetry.sweep_rate) * SEC_TO_MILLIS;
            exp_cyclic_voltammetry.convertedInitPot = (common.initial_voltage + 2.5) / 2;
            exp_cyclic_voltammetry.convertedSweepPot1 = (exp_cyclic_voltammetry.sweep_potential1 + 2.5) / 2;
            exp_cyclic_voltammetry.convertedSweepPot2 = (exp_cyclic_voltammetry.sweep_potential2 + 2.5) / 2;
            exp_cyclic_voltammetry.convertedSweepPot3 = (exp_cyclic_voltammetry.sweep_potential3 + 2.5) / 2;
            exp_cyclic_voltammetry.convertedSweepPot4 = (exp_cyclic_voltammetry.sweep_potential4 + 2.5) / 2;
            exp_cyclic_voltammetry.delTime = ((((1/(exp_cyclic_voltammetry.sweep_rate / exp_cyclic_voltammetry.volts_per_point)) * 1000) - 1.0)/1000) * SEC_TO_MILLIS;	
            exp_cyclic_voltammetry.sign1 = (exp_cyclic_voltammetry.convertedSweepPot2 > exp_cyclic_voltammetry.convertedSweepPot1)? 1: -1;
            exp_cyclic_voltammetry.sign2 = (exp_cyclic_voltammetry.convertedSweepPot3 > exp_cyclic_voltammetry.convertedSweepPot2)? 1: -1;
            exp_cyclic_voltammetry.sign3 = (exp_cyclic_voltammetry.convertedSweepPot4 > exp_cyclic_voltammetry.convertedSweepPot3)? 1: -1;

            CV.timer1 = (uint32_t)(common.ocp_meas_prd);
            CV.timer2 = (uint32_t)( (CV.timer1) + ((common.initial_volt_duration)) );
            CV.timer3 = (uint32_t)( (CV.timer2) + ((exp_cyclic_voltammetry.sweepDuration1 ) ) );
            CV.timer4 = (uint32_t)( (CV.timer3) + ((exp_cyclic_voltammetry.sweepDuration2 ) ) );
            CV.timer5 = (uint32_t)( (CV.timer4) + ((exp_cyclic_voltammetry.sweepDuration3 ) ) );
        break;
		}
	}
	else{
		switch(exp_new.exp_num){
        case CHRONOAMPEROMETRY:
        case CHRONOPOTENTIOMETRY:
            exp_new.ocp_meas_mode = table[OCP_MEAS_MODE_ROW]._char[OCP_MEAS_MODE_COL];
            exp_new.ocp_meas_period = (uint32_t)table[OCP_MEAS_PRD_ROW]._char[OCP_MEAS_PRD_COL] * SEC_TO_MILLIS;
            exp_new.we_num = table[WE_NUM_ROW]._char[WE_NUM_COL];
            exp_new.vs_ocp = table[VS_OCP_ROW]._char[VS_OCP_COL];
            misc.current_range = table[CURRENT_RANGE_ROW]._char[CURRENT_RANGE_COL];
            misc.auto_range = table[AUTO_RANGE_ROW]._char[AUTO_RANGE_COL];
            misc.post_exp_cell_state = table[POST_CELL_EXP_STATE_ROW]._char[POST_CELL_EXP_STATE_COL];
			exp_new.sampling_rate = table[SAMPLING_RATE_ROW]._float;
            exp_new.ocp_meas_slope = table[OCP_MEAS_SLOPE]._float;
            exp_new.initial_voltage = table[INITIAL_VOLTAGE]._float;
            exp_new.initial_voltage_duration = (uint32_t) table[INITIAL_VOLT_DURATION]._float  * SEC_TO_MILLIS;
        break;
        default:
        break;
		}
	}
}

void set_gains(uint8_t exp_num){

    switch (exp_num){
    case CHRONOAMPEROMETRY:
        write_mux_gains(CHRONOAMPEROMETRY_GAINS);
    break;
    case CHRONOPOTENTIOMETRY:
        write_mux_gains(CHRONOPOTENTIOMETRY_GAINS);
        HAL_GPIO_WritePin(Sense_Mode_SC_GPIO_Port, Sense_Mode_SC_Pin, GPIO_PIN_RESET);
    break;
    case NORMAL_PULSE_VOLTAMMETRY:
        write_mux_gains(NORMAL_PULSE_GAINS);
    break;
    case SQUAREWAVE_VOLTAMMETRY:
        write_mux_gains(SQUAREWAVE_GAINS);
    break;
    case CHRONO_OCP:
        write_mux_gains(CHRONO_OCP_GAINS);
        HAL_GPIO_WritePin(Sense_Mode_SC_GPIO_Port, Sense_Mode_SC_Pin, GPIO_PIN_RESET);
    break;
    case CHRONO_VAUX:
        write_mux_gains(CHRONO_VAUX_GAINS);
        HAL_GPIO_WritePin(Sense_Mode_SC_GPIO_Port, Sense_Mode_SC_Pin, GPIO_PIN_RESET);
    break;
    case LINEAR_PULSE_VOLTAMMETRY:
        write_mux_gains(LINEAR_PULSE_GAINS);	
    break;
    case DIFFERENTIAL_PULSE_VOLTAMMETRY:
        write_mux_gains(DIFFERENTIAL_PULSE_GAINS);
    break;
    case CYCLIC_VOLTAMMETRY:
        write_mux_gains(CYCLIC_GAINS);
    break;
    }
    set_adc_meas_gains(0x00, 0x01, 0x04);
}

void reset_gains()
{
    write_mux_gains(RESET_GAINS);
    HAL_GPIO_WritePin(Sense_Mode_SC_GPIO_Port, Sense_Mode_SC_Pin, GPIO_PIN_RESET);
    set_adc_meas_gains(0x00, 0x00, 0x82);
}

void write_mux_gains(GPIO_PinState ss_pin, GPIO_PinState ss7_pin, GPIO_PinState ss8_pin,  
    GPIO_PinState ss10_pin, GPIO_PinState ss11_pin){
        
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, ss_pin);
    HAL_GPIO_WritePin(SS7_GPIO_Port, SS7_Pin, ss7_pin);
    HAL_GPIO_WritePin(SS8_GPIO_Port, SS8_Pin, ss8_pin);
    HAL_GPIO_WritePin(SS10_GPIO_Port, SS10_Pin, ss10_pin);
    HAL_GPIO_WritePin(SS11_GPIO_Port, SS11_Pin, ss11_pin);       
}

void set_adc_meas_gains(uint8_t byte0, uint8_t byte1, uint8_t byte2){
    Data_for_try[0] = byte0;
    Data_for_try[1] = byte1;
    Data_for_try[2] = byte2;
    spi_write_read(&hspi3, Data_for_try, ret_for_try, SPI3_Chip_Select_GPIO_Port, SPI3_Chip_Select_Pin, 0, 3, 500);
}


void get_common_exp_data(){
    common.exp_num = parameter_table[EXP_NUM_ROW]._char[EXP_NUM_COL];
    common.ocp_meas_mode = parameter_table[OCP_MEAS_MODE_ROW]._char[OCP_MEAS_MODE_COL];
    common.ocp_meas_prd = parameter_table[OCP_MEAS_PRD_ROW]._char[OCP_MEAS_PRD_COL] * SEC_TO_MILLIS;
    common.ocp_meas_slope = parameter_table[OCP_MEAS_SLOPE]._float;
    common.initial_voltage = parameter_table[INITIAL_VOLTAGE]._float;
    common.initial_volt_duration = parameter_table[INITIAL_VOLT_DURATION]._float * SEC_TO_MILLIS;
    common.vs_ocp = parameter_table[VS_OCP_ROW]._char[VS_OCP_COL];
    
}