#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "flash.h"
#include "state_machine.h"

float step;

__IO uint32_t uwPrescalerValue = 0;
static uint32_t millis = 0;
static uint32_t micros = 0;
int flag_esp_write = 0;

uint8_t completed_page;

TIM_HandleTypeDef    TimHandle;
GPIO_InitTypeDef GPIO_InitStruct = {0};

void SystemClock_Config (void);
void HAL_TIM_Base_MspInit (TIM_HandleTypeDef *);
void scheduler (void);
void timer_config(void);
void dma_nvic_config(void);
void config_system(void);
extern uint8_t Data_for_try[3],ret_for_try[3];
extern uint8_t cmd_from_esp[64];
static uint8_t page_index = 0;
static uint8_t page_written = 0;
static bool sampling_instant = 0;
 void MX_DMA_Init(void);
float *page = NULL;
static uint32_t adc_Timer = 0;
static int16_t switchGain = 0;
int32_t adc_data;
float val;
double scaler_adc = 0.00001907348;       //2.5 / 131072.0;
double conversion_factor = 1.0;         //default to 1


int main(void){
    config_system();
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET);

    while (1) {
        scheduler();
    }
}

void config_system(void){
	HAL_Init();
    SystemClock_Config();
	MX_GPIO_Init();
	MX_SPI3_Init();
	MX_SPI2_Init();
	MX_SPI1_Init();
	MX_DMA_Init();
	dma_nvic_config();
	timer_config();
	W25qxx_Init();
	W25qxx_EraseChip();
}

void dma_nvic_config(){
    
	__HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}
void SystemClock_Config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        while(1);
    }
  
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;  
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        while(1);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    TIMx_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIMx_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(TIMx_IRQn);
}

void timer_config(){
	uwPrescalerValue = (uint32_t)(SystemCoreClock / 1000000) - 1;//1us
    TimHandle.Instance = TIMx;
    TimHandle.Init.Period            = 40 - 1;//40us
    TimHandle.Init.Prescaler         = uwPrescalerValue;
    TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;//0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;

    if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK) {
        Error_Handler();
    }
	HAL_TIM_Base_MspInit(&TimHandle);
    if (HAL_TIM_Base_Start_IT(&TimHandle) != HAL_OK) {
        Error_Handler();
    }
}
#define ADC_MID_VALUE   0x20000
#define ADC_RANGE       0x3FFFF
#define GAIN_3M         0.00033		//1000/3000000
#define GAIN_300K       0.0033		//1000/300000
#define GAIN_3K         0.33		//1000/3000
#define GAIN_30         8.33		//1000/30

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *TimHandle) {
	volatile register int16_t dummy_cnt = 0;
    static bool check_gains = 0;
    static float scaledAdcVal = 0.;
	static float temp_scaledAdcVal = 0.;
	micros += 40;
	if (micros % 1000 == 0){
		millis += 1;
		if (micros >= 0xFFFFFED8)
            micros = 0;
	}
    if (measActive && (absVal(micros - adc_Timer) >= (uint32_t)(adc_sampling_time * 1000))){ //adc_sampling_time - 10
//		GPIOB->BRR = (uint32_t)ADC_CONVST_Pin;	//Reset ADC SOC Pin
		sampling_instant = 1;
		adc_Timer = micros;
	}
	if(sampling_instant){
		sampling_instant = 0;
		adc_data = 0;
		if (!(page_written % 2) ){
			page = page1.float_page;
			completed_page = 1;
		}
		else{
			page = page2.float_page;
			completed_page = 0;
		}
		
		GPIOB->BSRR = (uint32_t)ADC_CONVST_Pin;
        for (register int i = 0; i < 9; i++){
			dummy_cnt++;//added to get 1300us sampling time for ADC
		}
        GPIOB->BRR = (uint32_t)ADC_CONVST_Pin;
        for (register int i = 0; i < 18; i++){
            GPIOC->BSRR = (uint32_t)ADC_CLK_Pin;
			adc_data += GPIOC->IDR & ADC_DATA_Pin?1:0;
            GPIOC->BRR = (uint32_t)ADC_CLK_Pin;
            adc_data <<= 1;
			dummy_cnt++;//added to get 280ns off time for clock
        }

		adc_data >>= 1;//compensate for an extra left shift in the above for loop
		GPIOB->BRR = (uint32_t)ADC_CONVST_Pin;	//Reset ADC SOC Pin
		
		if(adc_data < ADC_MID_VALUE){ //mid adc value.. above this is negative and below this is positive
            if(measure_ocp_offset){
                measure_ocp_offset = false;
                ocp_offset = 0;//adc_data;
            }            
            scaledAdcVal = (float)((adc_data - ocp_offset) * scaler_adc);
		}
        else {
            if(measure_ocp_offset){
                measure_ocp_offset = false;
                ocp_offset = 0;//adc_data;
            }
            scaledAdcVal = -(float)((ADC_RANGE - (adc_data - ocp_offset)) * scaler_adc);
		}
//		if ((parameter_table[EXP_NUM_ROW]._char[EXP_NUM_COL] != CHRONOPOTENTIOMETRY) || (parameter_table[EXP_NUM_ROW]._char[EXP_NUM_COL] != CHRONO_OCP))
//		scaledAdcVal = (scaledAdcVal + 0.0039)/0.03;
//		scaledAdcVal *= conversion_factor;
//		if(switchGain != 3)
		*(page + page_index) = (float)((scaledAdcVal ) * conversion_factor);
//		else
//        *(page + page_index) = (float)((scaledAdcVal + 0.0039)/0.03);
		if(flag_ocp_acq_inProgress){
			put_ocp(scaledAdcVal);
		}
		page_index++;
		if(page_index > 63){
			page_index = 0;
			page_written++;
			flag_flash_write = SET;
		}
		check_gains = 1;
	}
	else if (check_gains){
		
		if(switchGain == 0 && (float)absVal(scaledAdcVal) >= 2.4)//2.49998283
		{
			switchGain++;
		}
		else if ((switchGain != 0) && (float)absVal(scaledAdcVal) >= 2.4)
		{
			switchGain++;
			if (switchGain >= 3)
				switchGain = 3;
		}
		
		else if((switchGain == 3) && (float)absVal(scaledAdcVal) < 0.0002)
		{
			switchGain--;
		}
		else if ((switchGain != 3) && (float)absVal(scaledAdcVal) < 0.02)
		{
			switchGain--;
			if (switchGain <= 0)
				switchGain = 0;
		}
		
//		if(switchGain == 0 && (float)absVal(scaledAdcVal) >= 2.499)//2.49998283
//		{
//			switchGain++;
//		}
//		else if ((switchGain != 0) && (float)absVal(scaledAdcVal) >= 2.49)
//		{
//			switchGain++;
//			if (switchGain >= 3)
//				switchGain = 3;
//		}
//		
//		if((switchGain == 3) && (float)absVal(scaledAdcVal) < 0.002)
//		{
//			switchGain--;
//		}
//		else if ((switchGain != 3) && (float)absVal(scaledAdcVal) < 0.02)
//		{
//			switchGain--;
//			if (switchGain <= 0)
//				switchGain = 0;
//		}


//		if ((float)absVal(scaledAdcVal) > 2.3){
//			if(switchGain < 3){
//				switchGain ++;
//			}
//		}
//		else if ((float)absVal(scaledAdcVal) < 0.3){
//			if(switchGain > 0 ){
//				switchGain --;
//			}
//		}
		switchGain = 1;
        switch(switchGain){
		default:
		case 0:		//3M
			set_adc_meas_gains(0x00, 0x02, 0x08);
            conversion_factor = GAIN_3M;
			break;
		case 1:		//300k
			set_adc_meas_gains(0x00, 0x01, 0x04);
            conversion_factor = GAIN_300K;
			break;		
		case 2:		//3k
			set_adc_meas_gains(0x00, 0x00, 0x82);
            conversion_factor = GAIN_3K;
			break;	
		case 3:		//30
			set_adc_meas_gains(0x00, 0x00, 0x041);
            conversion_factor = GAIN_30;
			break;
		}
        if ((parameter_table[EXP_NUM_ROW]._char[EXP_NUM_COL] == CHRONOPOTENTIOMETRY) || (parameter_table[EXP_NUM_ROW]._char[EXP_NUM_COL] == CHRONO_OCP)){
            conversion_factor = 1.0;
        }
	check_gains = 0;
	}	
}

double absVal(double x){
	*(((int *) &x) + 1) &= 0x7fffffff;
	return x;
}

uint32_t get_millis() {
	return millis;
}

void scheduler() {
	while (1){
        state_machine();
        //sample_channel();
		//GPIOB->BRR = GPIO_PIN_2;
		comms();
		//GPIOB->BSRR = GPIO_PIN_2;
  }
	
}

void Error_Handler(void) {
	//indicate a fault condition
}


//void sample_channel(){
//	if (measActive && (get_millis() - adc_Timer >= (adc_sampling_time))){ //adc_sampling_time - 10
//		GPIOB->BRR = (uint32_t)ADC_CONVST_Pin;	//Reset ADC SOC Pin
//		sampling_instant = 1;
//		adc_Timer = get_millis();
//	}
//	
//}

uint16_t spi_write_read( SPI_HandleTypeDef *spi, uint8_t *tx_buf, uint8_t *rx_buf, GPIO_TypeDef *port, uint16_t pin, 
    uint8_t delay, uint16_t len, uint16_t timeout ){
	
    HAL_Delay(delay);
    HAL_GPIO_WritePin( port, pin, GPIO_PIN_RESET );
	HAL_Delay(delay);
	if( HAL_SPI_TransmitReceive( spi, (uint8_t*) tx_buf, (uint8_t*) rx_buf, len, timeout ) != HAL_OK ) {
		HAL_Delay(delay);
        Error_Handler();
        HAL_GPIO_WritePin( port, pin, GPIO_PIN_SET );
        return 0;
	}
    HAL_Delay(delay);
    HAL_GPIO_WritePin( port, pin, GPIO_PIN_SET );
    return 1;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}


#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
