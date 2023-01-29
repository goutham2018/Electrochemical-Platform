#include "state_machine.h"

_Bool flag_flash_write = RESET;
bool measure_ocp_offset = false;

uint8_t current_state = STANDBY;	
uint8_t num_cycles = 0;
uint8_t flag = 0;
uint8_t flag_end_test = 0;

uint8_t request_test_parameters[64] = {'S','T','A','R','T'};
uint8_t request_test_parameters1[64] = {'N','E','X','T','!'};
uint8_t esp_ready[64] = {'R','E','A','D','Y'};
uint8_t stop_command[64] = {'S','T','O','P','!'};
uint8_t cmd_from_esp[64] = {0};

uint32_t flash_page_count = 0xFFF;
uint32_t esp_page_count = 0xFFF;
uint32_t dac_update_time = 10;
uint32_t adc_sampling_time = 10;
uint32_t time_bw_retry = 0;
uint32_t send_count = 0;

uint32_t ocp_offset = 0.;

uint32_t data_read_from_flash[64];
extern uint32_t millis;
static uint32_t ocp_timer = 0;
double deltaPotential = 0;
bool measActive = false;
bool enquire_esp = false;
test_parameters parameter_table[32];

ram_page page1, page2/*, page2esp*/;

void state_machine() {
	switch (current_state){
    default:
    case STANDBY:
        state_standby();
    break;
    case TEST_SET:
        state_test_set();
    break;
    case ACTIVE:
        state_active();
    break;
    case FAILURE:
        state_failure();
    break;
	}
}


void state_standby( ) {
    uint8_t check_ack = 0;
	if (!enquire_esp){
		if (time_bw_retry == 0){
			time_bw_retry = get_millis();
		}
		else if(get_millis() - time_bw_retry > 200) {
			check_ack = spi_write_read(&hspi1, esp_ready, cmd_from_esp, SPI1_Chip_Select_ESP_GPIO_Port, SPI1_Chip_Select_ESP_Pin, 50, FRAME_SIZE, 1000);
			HAL_Delay(100);  
		}
		if (check_ack){
			check_ack = 0;
			if (cmd_from_esp[0] == 'Y' && cmd_from_esp[1] == 'E' && cmd_from_esp[2] == 'S' && cmd_from_esp[3] == '!' && cmd_from_esp[4] == '!') {
				set_operating_state(TEST_SET);
			}
			else{
				time_bw_retry = 0;
			}
		}
	}
}

void state_test_set(void){
    static uint8_t read_2nd_page;
    HAL_Delay(1);
    HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_RESET);
    HAL_Delay(1);
    if((HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)request_test_parameters,(uint8_t*)parameter_table,64,5))!=HAL_OK && (flag==0)){
    Error_Handler();
    }
    else{
        if((parameter_table[0]._char[0] == SOH) && (parameter_table[15]._char[3] == _CR)){
            HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_SET);
            HAL_Delay(1);
            read_2nd_page = check_header_footer(parameter_table);
            flag=1;
			//CS set was here, moved it on top.
            extract_exp_values(parameter_table,0);
            HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_RESET);
            if((HAL_SPI_TransmitReceive(&hspi1,(uint8_t*)request_test_parameters1,(uint8_t*)parameter_table+64,64,5))!=HAL_OK){
              Error_Handler();
            }
            else{
                if (((parameter_table[27]._char[2])==0x01) && (parameter_table[27]._char[3])==0x0D){
                    HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_SET);
                    read_2nd_page = check_header_footer(parameter_table + 16);
                    extract_exp_values(parameter_table+16,1);
					measActive = true;
					set_operating_state(ACTIVE);
					//get_ocp_offset();
                }
                else{
                    set_operating_state(TEST_SET);
                }
            }				
        }
    }
}

  
double ocp_value = 0, ocp_value_sum = 0;
int flag_ocp_acq_inProgress = 0;
uint32_t count=0;

void put_ocp(float x){
	ocp_value = x;//(exp_new.ocp_meas_period * adc_sampling_time);
	ocp_value_sum += x;
	count++;
}
#define IMMEDIATE	0
#define AVERAGE		1
#define DISABLE		3
float get_ocp(uint8_t mode){
	if (mode==IMMEDIATE){
		return (float) ocp_value;
	}
	else if (mode==AVERAGE){
		return (float) (ocp_value_sum/local_min_sat(count, 1));//ocp_value;
	}
	else {
		return (float) 0.0;
	}
}

uint16_t local_min_sat(uint16_t x, uint16_t y){	//Avoid divide by 1
	if (x < y){
		return y;
    }
	else{
		return x;
    }
}

float measure_ocp(){
    static float ocp_measured;
	if (!ocp_timer){
        ocp_timer = get_millis();
	}
	else if (get_millis() - ocp_timer <= exp_new.ocp_meas_period){
        reset_gains();
        flag_ocp_acq_inProgress = 1;
        ocp_measured = get_ocp(exp_new.ocp_meas_mode);
	}
	else{
		flag_ocp_acq_inProgress = 0;
//		set_gains(exp_new.exp_num);
	}
	return ocp_measured;
}


uint8_t step_num = 0;
#define merge(a,b) a##b

float get_updated_values(uint8_t step, uint8_t index){
    float return_val = 0;
    static float ocp_measured;

	ocp_measured = measure_ocp();
    if(!flag_ocp_acq_inProgress){
		switch(exp_new.exp_num){
        case CHRONOAMPEROMETRY:
            return_val = chrono_amp_pot(ocp_measured, step, index);
        break;
        case CHRONOPOTENTIOMETRY:
            return_val = chrono_amp_pot(ocp_measured, step, index);
        break;
        default:
            return_val = 0;
        break;
		}
	}
    return return_val;
}
const float REF = 2.5;

float get_dac_value(float voltage, float ref, uint8_t vs_ocp, float ocp_measured, uint8_t exp_num){
	float retval = 0.;
	if (exp_num == 1){
		retval = ((voltage /*- 0.0033*/ + ref + (vs_ocp * ocp_measured)) / 2);
	}
	else if (exp_num == 2){
		retval = ((((voltage*82)) - 0.0033 - 0.000037 - 0.000006 - 0.0000100 - 0.000148 - 0.0000020 - 0.000003 - 0.000008 + ref + (vs_ocp * ocp_measured)) / 2); // offset adjust: - 0.000625/* 0.000016 */ -0.0005 -0.0016 -0.0009
	}
	return retval;
}


float chrono_amp_pot(float ocp_measured, uint8_t step, uint8_t index){
	float return_val = 0;

    switch(step){
    case 0: //initial potential
        if (!index){
            return_val = exp_new.initial_voltage_duration;
        }
        else {
            return_val = get_dac_value(exp_new.initial_voltage, REF, exp_new.vs_ocp, ocp_measured, exp_new.exp_num);
        }
    break;			
    case 1:
        if (!index){
			return_val = exp_new.merge(voltage_duration,1);
        }
        else{
			return_val = get_dac_value(exp_new.merge(voltage,1), REF, exp_new.merge(ocp,1), ocp_measured, exp_new.exp_num);
        }
    break;
    case 2:
        if (!index){
			return_val = exp_new.merge(voltage_duration,2);
        }
        else{
			return_val = get_dac_value(exp_new.merge(voltage,2), REF, exp_new.merge(ocp,2), ocp_measured, exp_new.exp_num);
        }
    break;
    case 3:
        if (!index){
			return_val = exp_new.merge(voltage_duration,3);
        }
        else{
			return_val = get_dac_value(exp_new.merge(voltage,3), REF, exp_new.merge(ocp,3), ocp_measured, exp_new.exp_num);
        }
    break;
    case 4:
        if (!index){
			return_val = exp_new.merge(voltage_duration,4);
        }
        else{
			return_val = get_dac_value(exp_new.merge(voltage,4), REF, exp_new.merge(ocp,4), ocp_measured, exp_new.exp_num);
        }  
    break;
    }
  return return_val;
}
  
uint32_t *page_to_write;

void state_active(void){
    switch(parameter_table[EXP_NUM_ROW]._char[EXP_NUM_COL]){
    case CHRONOAMPEROMETRY:
    case CHRONOPOTENTIOMETRY:
        adc_sampling_time = (uint32_t) (SEC_TO_MILLIS / exp_new.sampling_rate);
        run_chrono_amp_pot();
    break;
    case NORMAL_PULSE_VOLTAMMETRY:
        adc_sampling_time = (uint32_t)((exp_normal_pulse_voltammetry.step_time * (0.1*0.1))*SEC_TO_MILLIS);
        run_normal_pulse_voltammetry();			
    break;
    case SQUAREWAVE_VOLTAMMETRY:
        adc_sampling_time = (uint32_t)((SEC_TO_MILLIS)/exp_squarewave_voltammetry.frequency)/5;
        run_squarewave_voltammetry();
    break;
    case CHRONO_OCP:
        adc_sampling_time = (uint32_t)SEC_TO_MILLIS/exp_chrono_ocp.sampling_rate;
        run_chrono_ocp();
    break;
//    case CHRONO_VAUX:

//    break;
    case LINEAR_PULSE_VOLTAMMETRY:
        adc_sampling_time = (uint32_t)100 * (exp_linear_pulse.volts_per_point/exp_linear_pulse.sweep_rate);
        run_linear_pulse();
    break;
    case DIFFERENTIAL_PULSE_VOLTAMMETRY:
        adc_sampling_time = (uint32_t)((exp_diff_pulse_voltammetry.step_time * (0.1))*SEC_TO_MILLIS);
        run_diff_pulse_voltammetry();
    break;
    case CYCLIC_VOLTAMMETRY:
        adc_sampling_time = (uint32_t)100 * (exp_cyclic_voltammetry.volts_per_point/exp_cyclic_voltammetry.sweep_rate);//(uint32_t)SEC_TO_MILLIS * (exp_cyclic_voltammetry.volts_per_point/exp_cyclic_voltammetry.sweep_rate);
        run_cyclic_voltammetry();
    break;
    }
		
	
}

void comms(){
	static uint8_t state101 = 1;
	if (flag_flash_write && measActive){
		flag_flash_write = RESET;
		uint8_t delay = 0;
		HAL_Delay(2);
		page_to_write = (completed_page?page1.hex_page:page2.hex_page);
		HAL_Delay(2);
		for (volatile uint16_t i = 0; i < 4; i++){
			HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_RESET);
			HAL_Delay(delay);
			if(HAL_SPI_Transmit(&hspi1, (uint8_t*) (page_to_write + (i * 16)), FRAME_SIZE,100)!= HAL_OK) {
				Error_Handler();
			}
			HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_SET);
			HAL_Delay(delay);
		}

		send_count++;
		if(flash_page_count <= 0xFFF){
			if (state101){
				W25qxx_WritePage((uint8_t*) (completed_page?page1.hex_page:page2.hex_page), flash_page_count ,0x00 ,256);
				flash_page_count ++;
				if (flash_page_count >= 0xFFF){
					flash_page_count = 0xFFF;
				}
				state101 = 0;
			}
			else{
				state101 = 1;
				//W25qxx_ReadBytes((uint8_t*)data_read_from_flash, 0x00, 256);
			}
		}
		if (get_operating_state() == STANDBY){
            measActive = false;
			spi_write_read(&hspi1, stop_command, cmd_from_esp, SPI1_Chip_Select_ESP_GPIO_Port, SPI1_Chip_Select_ESP_Pin, 50, FRAME_SIZE, 1000);
			enquire_esp = false;
		}
	}
		
	HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_RESET);
	if(HAL_SPI_Receive_DMA(&hspi1, (uint8_t*)cmd_from_esp, FRAME_SIZE)!= HAL_OK) {
		Error_Handler();
	}
	else{
		if (cmd_from_esp[0] == 'A' && cmd_from_esp[1] == 'B' && cmd_from_esp[2] == 'O' && cmd_from_esp[3] == 'R' && cmd_from_esp[4] == 'T'){
			set_operating_state(STANDBY);
		}		
	}
	HAL_GPIO_WritePin(SPI1_Chip_Select_ESP_GPIO_Port,SPI1_Chip_Select_ESP_Pin,GPIO_PIN_SET);
}

void run_chrono_amp_pot(){
    uint32_t var = 0;
    static uint32_t dac_update_time_ms = 0;
    static uint32_t dac_data = 0;
    static uint8_t num_cycles = 0;
	static uint32_t test_timer = 0;
	static bool flag_gains_set = false;
    
	if (!test_timer){
		reset_gains();
		test_timer = get_millis();
		num_cycles = exp_new.num_cycles;
		flag_ocp_acq_inProgress	= 1;
	}
	if(get_millis()- test_timer > ( exp_new.ocp_meas_period + exp_new.initial_voltage_duration + (exp_new.voltage_duration1 + 
		exp_new.voltage_duration2 + exp_new.voltage_duration3 + exp_new.voltage_duration4)*exp_new.num_cycles) ) 
	{
		default_output_state();
	}		
	else{
        if (!flag_ocp_acq_inProgress){
			if(!flag_gains_set)
			{
				set_gains(exp_new.exp_num);
				flag_gains_set = true;
			}
            if (dac_update_time_ms == 0) {
                dac_update_time_ms = get_millis();
				if (num_cycles > 0) {
					if (step_num > exp_new.num_steps) {
						step_num = 1;
						if (num_cycles > 0) {
							num_cycles --;
						}
					}
				dac_update_time = (float)get_updated_values(step_num,0);
                }
                else {
                    num_cycles = 0;
                    step_num = 0;
                }
            }

            else if(get_millis() - dac_update_time_ms <= dac_update_time) {
                dac_data = (uint32_t)((get_updated_values(step_num,1) / 2.5) * 0x3FFFF);
                var = dac_data << LSB_UNUSED;
                uint8_t data_on_spi[3],dummy_read[3];
                data_on_spi[2]= (uint8_t)(var & MASK_LOW_BYTE);
                data_on_spi[1]= (uint8_t)((var & MASK_MID_BYTE)>>8);
                data_on_spi[0]= (uint8_t)((var & MASK_HIGH_BYTE) >> 16) | DIN_REG;
                spi_write_read( &hspi3, data_on_spi, dummy_read, SPI3_Chip_Select_DAC_GPIO_Port, SPI3_Chip_Select_DAC_Pin, 0, 3, 1 );
            }
            else{
                dac_update_time_ms = 0;
                step_num ++;
            }
        }
        else{
            get_updated_values(0,0);
		}
	}
}

void run_chrono_ocp(){
	static float var = 0;
	static float ocp_measured = 0.;
	static uint32_t test_timer = 0;
	static bool flag_gains_set = false;
    
	if (!test_timer){
		test_timer = get_millis();
		reset_gains();
		flag_ocp_acq_inProgress	= 1;
	}
	else if (get_millis() - test_timer <= exp_chrono_ocp.duration ){
		var = 1.25;
		set_output(var);
		
		if(!flag_gains_set)
		{
			set_gains(common.exp_num);
			flag_gains_set = true;
		}
        get_ocp(AVERAGE);
	}
	else if (get_millis() - test_timer > exp_chrono_ocp.duration){
        default_output_state();
	}
}

void run_linear_pulse(){
	static float var = 0;
	static uint32_t test_time = 0;
	static uint32_t test_time_2 = 0;
	static uint32_t sweepTime = 0;
	static float ocp_measured = 0.;
	static bool flag_gains_set = false;
	
	if (!test_time){
		reset_gains();
		test_time = get_millis();	
		flag_ocp_acq_inProgress	= 1;
		get_ocp_offset();
	}
	else if ((get_millis() - test_time) <= (common.ocp_meas_prd) ){
			ocp_measured = get_ocp(common.ocp_meas_mode);
//			test_time_2 = get_millis();
	}
	else if(!flag_gains_set){
		flag_ocp_acq_inProgress	= 0;
		set_gains(common.exp_num);
		flag_gains_set = true;
		exp_linear_pulse.sweepDuration = get_exp_duration(exp_linear_pulse.sweep_potential2,exp_linear_pulse.sweep_potential1,exp_linear_pulse.sweep_rate,
										exp_linear_pulse.vsocp_sweep1,exp_linear_pulse.vsocp_sweep2,ocp_measured, 1, 0);
//		test_time_2 = get_millis();
	}
	else if((get_millis() - test_time) <= (common.ocp_meas_prd + common.initial_volt_duration)){

		var = exp_linear_pulse.convertedInitPot + get_ocp_value(common.vs_ocp,ocp_measured);
		set_output(var);
	}
	else if ( (get_millis() - test_time) <=  (common.ocp_meas_prd + exp_linear_pulse.sweepDuration + common.initial_volt_duration ))
	{
		if(!flag_gains_set){
			set_gains(common.exp_num);
			flag_gains_set = true;
			exp_linear_pulse.sweepDuration = get_exp_duration(exp_linear_pulse.sweep_potential2,exp_linear_pulse.sweep_potential1,exp_linear_pulse.sweep_rate,
											exp_linear_pulse.vsocp_sweep1,exp_linear_pulse.vsocp_sweep2,ocp_measured, 1, 0);
		}
		if (sweepTime == 0){
			var = ( exp_linear_pulse.convertedSweepPot1 + (deltaPotential/2)) + get_ocp_value(exp_linear_pulse.vsocp_sweep1,ocp_measured);
			sweepTime = get_millis();
			
		}
		else if ( var <= (exp_linear_pulse.convertedSweepPot2 + get_ocp_value(exp_linear_pulse.vsocp_sweep2,ocp_measured))){
			if(get_millis() - sweepTime >= exp_linear_pulse.delTime ){
				sweepTime = 0;
				deltaPotential += exp_linear_pulse.volts_per_point;
				set_output(var);
			}
		}	
	}
	else{
        default_output_state();
	}
}

void run_cyclic_voltammetry(){
	static float var = 0;
	static bool flag = 0;
	static uint32_t test_time = 0;
	static uint32_t sweepTime = 0;
	static uint8_t num_cycles = 1;
	static float ocp_measured = 0.;
	static bool flag_gains_set = false;
	bool time_offset = 0;

	if(num_cycles > 1){
		 time_offset = 1;
    }
	
	if (num_cycles < (exp_cyclic_voltammetry.num_cycles + 1)){
		if (!test_time){
			test_time = get_millis();
            reset_gains();
            flag_ocp_acq_inProgress	= 1;
		}
		else if (((get_millis() - test_time) <= CV.timer1) && num_cycles == 1){
			ocp_measured = get_ocp(common.ocp_meas_mode);
		}
		else if((get_millis() - test_time) <= (CV.timer2) && num_cycles == 1)
		{
			flag_ocp_acq_inProgress	= 0;
			if(!flag_gains_set){
				set_gains(common.exp_num);
				flag_gains_set = true;
			}
			var = exp_cyclic_voltammetry.convertedInitPot + get_ocp_value(common.vs_ocp,ocp_measured);
			set_output(var);
		}
		else if ( (get_millis() - test_time) <=  ((CV.timer3 - (CV.timer2)*time_offset)) ){
			if(!flag_gains_set){
				set_gains(common.exp_num);
				flag_gains_set = true;
			}
			if (sweepTime == 0){
				var = (exp_cyclic_voltammetry.convertedSweepPot1 + (deltaPotential/2)) + get_ocp_value(exp_cyclic_voltammetry.vsocp_sweep1,ocp_measured);
				sweepTime = get_millis();
				
			}
			else if ( (get_millis() - sweepTime) >= exp_cyclic_voltammetry.delTime  /*&&
				( (exp_linear_pulse.convertedSweepPot1 + deltaPotential/2) < exp_linear_pulse.convertedSweepPot2)*/ ){
				sweepTime = 0;
				deltaPotential += exp_cyclic_voltammetry.sign1*exp_cyclic_voltammetry.volts_per_point;
				set_output(var);
			}
		}
		else if ( (get_millis() - test_time) <=  (CV.timer4 - (CV.timer2)*time_offset ) ){
			if(flag == 0){
				deltaPotential = 0;
				sweepTime = 0;
                flag = 1;
			}
			if (sweepTime == 0){
				var = ( exp_cyclic_voltammetry.convertedSweepPot2 + (deltaPotential/2)) + get_ocp_value(exp_cyclic_voltammetry.vsocp_sweep2,ocp_measured);
				sweepTime = get_millis();
			}
			else if ( (get_millis() - sweepTime) >= exp_cyclic_voltammetry.delTime /*&&
				( (exp_linear_pulse.convertedSweepPot1 + deltaPotential/2) < exp_linear_pulse.convertedSweepPot2) */){
				sweepTime = 0;
				deltaPotential += exp_cyclic_voltammetry.sign2 * exp_cyclic_voltammetry.volts_per_point;
				set_output(var);
			}
		}
		else if ( (get_millis() - test_time) <=  (CV.timer5 - (CV.timer2)*time_offset) ){
			if(flag == 1){
				deltaPotential = 0;
				sweepTime = 0;
			}
			flag = 0;
			
			if (sweepTime == 0){
				var = ( exp_cyclic_voltammetry.convertedSweepPot3 + (deltaPotential/2)) + get_ocp_value(exp_cyclic_voltammetry.vsocp_sweep3,ocp_measured);
				sweepTime = get_millis();
				
			}
			else if ( (get_millis() - sweepTime) >=  exp_cyclic_voltammetry.delTime  /*&&
				( (exp_linear_pulse.convertedSweepPot1 + deltaPotential/2) < exp_linear_pulse.convertedSweepPot2) */){
				sweepTime = 0;
				deltaPotential += exp_cyclic_voltammetry.sign3 * exp_cyclic_voltammetry.volts_per_point;
				set_output(var);
			}
		}
		else {
			test_time = get_millis();
			num_cycles++;
			deltaPotential = 0;
		}
	}
	else{
        default_output_state();
	}
}

void run_squarewave_voltammetry(){
	static float var = 0;
	static uint32_t test_time = 0;
	static uint32_t test_time_2 = 0;
	static uint32_t sweepTime = 0;
	static float ocp_measured = 0.;
	static uint32_t num_cycle = 1;
	static bool flag_gains_set = false;
	
	if (!test_time){
		test_time = get_millis();
		deltaPotential = 0;
		reset_gains();
		flag_ocp_acq_inProgress	= 1;
	}
	else if ((get_millis() - test_time) <= SV.timer1){
		ocp_measured = get_ocp(common.ocp_meas_mode);
//		test_time_2 = get_millis();
	}
	else if(!flag_gains_set){
		flag_ocp_acq_inProgress	= 0;
		set_gains(common.exp_num);
		flag_gains_set = true;
		SV.expDuration = get_exp_duration(exp_squarewave_voltammetry.sweep_potential2,exp_squarewave_voltammetry.sweep_potential1,exp_squarewave_voltammetry.sweep_increment,
										exp_squarewave_voltammetry.vsocp_sweep1,exp_squarewave_voltammetry.vsocp_sweep2, ocp_measured, (1/exp_squarewave_voltammetry.frequency), exp_squarewave_voltammetry.pulse_height); 
	}
	else if((get_millis() - test_time) <= (SV.timer2)){

		var = exp_squarewave_voltammetry.convertedInitPot + get_ocp_value(common.vs_ocp,ocp_measured);
		set_output(var);
	}
	else if ((get_millis() - test_time) <= (SV.timer2 + (SV.expDuration)*2)){
		if (sweepTime == 0){
			var = (uint32_t)( ( ( (exp_squarewave_voltammetry.convertedSweepPot1)/2.5 ) )* 0x3FFFF )  + get_ocp_value(exp_squarewave_voltammetry.vsocp_sweep1,ocp_measured);
			sweepTime = get_millis();
		}
		else if ( (get_millis() - sweepTime) <= SV.OnTime ){
			var	= exp_squarewave_voltammetry.convertedSweepPot1 + exp_squarewave_voltammetry.pulse_height + (deltaPotential) + get_ocp_value(exp_squarewave_voltammetry.vsocp_sweep1,ocp_measured);
			set_output(var);
		}
		else if ((get_millis() - sweepTime) <= (SV.OnTime + SV.OffTime) ){
			var	= exp_squarewave_voltammetry.convertedSweepPot1 - exp_squarewave_voltammetry.pulse_height + (deltaPotential) + get_ocp_value(exp_squarewave_voltammetry.vsocp_sweep1,ocp_measured);
			set_output(var);
		}
//		if ((get_millis() - sweepTime) >= (SV.OnTime + SV.OffTime)){
		else{
//			num_cycle++;
			deltaPotential += exp_squarewave_voltammetry.sweep_increment;
			sweepTime = get_millis();
		}
    }
	else{
        default_output_state();
	}	
}


void run_normal_pulse_voltammetry(){
	static float var = 0;
	static uint32_t test_time = 0;
	static uint32_t test_time_2 = 0;
	static uint32_t sweepTime = 0;
	static float ocp_measured = 0.;
	static bool flag_gains_set = false;
	
	if (!test_time){
		test_time = get_millis();
		deltaPotential = 0;
		test_time = get_millis();
		reset_gains();
		flag_ocp_acq_inProgress	= 1;
	}
	else if ((get_millis() - test_time) <= SV.timer1){
		ocp_measured = get_ocp(common.ocp_meas_mode);
		test_time_2 = get_millis();
	}
	else if((get_millis() - test_time_2) <= (SV.timer2)){
		if(!flag_gains_set){
			flag_ocp_acq_inProgress	= 0;
            set_gains(common.exp_num);
            flag_gains_set = true;
			SV.expDuration = get_exp_duration(exp_normal_pulse_voltammetry.sweep_potential2, exp_normal_pulse_voltammetry.sweep_potential1, exp_normal_pulse_voltammetry.sweep_increment,
							 exp_linear_pulse.vsocp_sweep1,exp_linear_pulse.vsocp_sweep2, ocp_measured, exp_normal_pulse_voltammetry.step_time, 0);
		}
		var = exp_normal_pulse_voltammetry.convertedInitPot + get_ocp_value(common.vs_ocp,ocp_measured);
		set_output(var);
	}
	else if ((get_millis() - test_time_2) <= (SV.timer2 + SV.expDuration)){
		if (sweepTime == 0){
			var = (uint32_t)( ( ( (exp_normal_pulse_voltammetry.convertedSweepPot1)/2.5 ) )* 0x3FFFF ) + get_ocp_value(exp_normal_pulse_voltammetry.vsocp_sweep1,ocp_measured);
			sweepTime = get_millis();
			deltaPotential += exp_normal_pulse_voltammetry.sweep_increment;
		}
		else if ( (get_millis() - sweepTime) <= SV.OnTime ){
			var	= exp_normal_pulse_voltammetry.convertedSweepPot1 + (deltaPotential/2) + get_ocp_value(exp_normal_pulse_voltammetry.vsocp_sweep1,ocp_measured);
			set_output(var);
		}
		else if ((get_millis() - sweepTime) < (SV.OnTime + SV.OffTime)){
			var	= exp_normal_pulse_voltammetry.convertedSweepPot1 + get_ocp_value(exp_normal_pulse_voltammetry.vsocp_sweep1,ocp_measured);
			set_output(var);
		}
		if ((get_millis() - sweepTime) >= (SV.OnTime + SV.OffTime)){
			deltaPotential += exp_normal_pulse_voltammetry.sweep_increment;
			sweepTime = get_millis();
		}
	}
	else{
        default_output_state();
	}	
}


void run_diff_pulse_voltammetry(){
	static float var = 0;
	static uint32_t test_time = 0;
	static uint32_t test_time_2 = 0;
	static uint32_t sweepTime = 0;
	static float ocp_measured = 0.;
	static uint32_t num_cycle = 1;
	static bool flag_gains_set = false;
	static float ocp_init = 0.;
	static float ocp_sweep1 = 0.;
	static float ocp_sweep2 = 0.;


	if (!test_time){
		test_time = get_millis();
		deltaPotential = 0;
		test_time = get_millis();
		reset_gains();
		flag_ocp_acq_inProgress	= 1;
	}
	else if ((get_millis() - test_time) <= SV.timer1){
		ocp_measured = get_ocp(common.ocp_meas_mode);
		test_time_2 = get_millis();
	}
	else if((get_millis() - test_time_2) <= (SV.timer2))
	{
		if(!flag_gains_set){
			flag_ocp_acq_inProgress	= 0;
            set_gains(common.exp_num);
            flag_gains_set = true;
			SV.expDuration = get_exp_duration(exp_diff_pulse_voltammetry.sweep_potential2, exp_diff_pulse_voltammetry.sweep_potential1, exp_diff_pulse_voltammetry.sweep_increment,
							 exp_diff_pulse_voltammetry.vsocp_sweep1,exp_diff_pulse_voltammetry.vsocp_sweep2, ocp_measured, exp_diff_pulse_voltammetry.step_time, 0);
		}
		var = exp_diff_pulse_voltammetry.convertedInitPot + get_ocp_value(common.vs_ocp,ocp_measured);
		set_output(var);
	}
	else if ((get_millis() - test_time_2) <= (SV.timer2 + SV.expDuration)){
		if (sweepTime == 0){
			var = (uint32_t)( ( ( (exp_diff_pulse_voltammetry.convertedSweepPot1)/2.5 ) )* 0x3FFFF ) + get_ocp_value(exp_diff_pulse_voltammetry.vsocp_sweep1,ocp_measured);
			sweepTime = get_millis();
		}
		else if ( (get_millis() - sweepTime) <= SV.OnTime ){
			var	= exp_diff_pulse_voltammetry.convertedSweepPot1 + exp_diff_pulse_voltammetry.pulse_height + (deltaPotential/2) + get_ocp_value(exp_diff_pulse_voltammetry.vsocp_sweep1,ocp_measured);
			set_output(var);
		}
		else if ((get_millis() - sweepTime) < (SV.OnTime + SV.OffTime) ){
			var	= exp_diff_pulse_voltammetry.convertedSweepPot1  + (deltaPotential/2) + get_ocp_value(exp_diff_pulse_voltammetry.vsocp_sweep1,ocp_measured);
			set_output(var);
		}
		if ((get_millis() - sweepTime) >= (SV.OnTime + SV.OffTime)){
			num_cycle++;
			deltaPotential += exp_diff_pulse_voltammetry.sweep_increment;
			sweepTime = get_millis();
		}
	}
	else{
        default_output_state();
	}	
}


void set_output(float value){
	static uint32_t dac_data;
	uint32_t var = 0;
	uint8_t data_on_spi[3],dummy_read[3];
	
	dac_data = (uint32_t) (value * ONE_BY_2P5 * 0x3FFFF);
	var = dac_data << LSB_UNUSED;
	data_on_spi[2]= (uint8_t)(var & MASK_LOW_BYTE);
	data_on_spi[1]= (uint8_t)((var & MASK_MID_BYTE)>>8);
	data_on_spi[0]= (uint8_t)((var & MASK_HIGH_BYTE) >> 16) | DIN_REG;
	spi_write_read( &hspi3, data_on_spi, dummy_read, SPI3_Chip_Select_DAC_GPIO_Port, SPI3_Chip_Select_DAC_Pin, 0, 3, 1 );
}

void state_failure(void){
	//indicate a fault condition
}

void set_operating_state(uint8_t new_state){
//	if(current_state == 0x02){
//        spi_write_read(&hspi1, stop_command, cmd_from_esp, SPI1_Chip_Select_ESP_GPIO_Port, SPI1_Chip_Select_ESP_Pin, 50, FRAME_SIZE, 1000);
//    }
	current_state = new_state;
	//change the status of any state variable here if needed.
}

uint8_t get_operating_state(){
	return (current_state);
}

void default_output_state(void){
	static float var = 0;
    var = 1.25;
    set_output(var);
	enquire_esp = true;
	set_operating_state(STANDBY);
}

float get_ocp_value(uint8_t ocp_needed, float ocp_value){
	return ((ocp_needed == 1)? (ocp_value/2): 0);
}

void get_ocp_offset(){
	write_mux_gains(MEASURE_OFFSET);
	measure_ocp_offset = true;
	//ocp_offset = get_ocp(IMMEDIATE);
}

uint32_t get_exp_duration(float sweep_V2, float sweep_V1, float sweep_rate, float vsocp_sweep1, float vsocp_sweep2, float ocp_value, float stepTime, float pulseHeight){
	float sweep2_final;
	float sweep1_final;
	uint32_t ret_val = 0;
	sweep2_final = (vsocp_sweep2 == 1)? (sweep_V2 + ocp_value): sweep_V2;
	sweep1_final = (vsocp_sweep1 == 1)? (sweep_V1 + ocp_value): sweep_V1; 
	ret_val = (uint32_t)absVal( ( (sweep2_final - sweep1_final + pulseHeight) / sweep_rate) * stepTime * SEC_TO_MILLIS);
	return ret_val;
}
