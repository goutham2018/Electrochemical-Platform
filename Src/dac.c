#include "dac.h"
#include "gpio.h"
#include "spi.h"
#include "main.h"

void Max5318_init(void){
	Max5318_Offset_Register(0);
	Max5318_Gain_Write_Register(1);
}

void Max5318_Offset_Register(uint32_t c)
{
	uint8_t data_on_spi[3];
	uint32_t var = c << LSB_UNUSED;

	data_on_spi[2]=(uint8_t)(var & MASK_LOW_BYTE);
	data_on_spi[1]=(uint8_t)((var & MASK_MID_BYTE) >> 8);
	data_on_spi[0]=(uint8_t)((var & MASK_HIGH_BYTE) >> 16) | OFFSET_REG;
	Max5318_Spi(data_on_spi);
}

void Max5318_Gain_Write_Register(uint32_t c)
{
	uint8_t data_on_spi[3];
	uint32_t var = c << LSB_UNUSED;

	data_on_spi[2]=(uint8_t)(var & MASK_LOW_BYTE);
	data_on_spi[1]=(uint8_t)((var & MASK_MID_BYTE)>>8);
	data_on_spi[0]=(uint8_t)((var & MASK_HIGH_BYTE) >> 16) | GAIN_REG;
	Max5318_Spi(data_on_spi);
}

void Max5318_Spi(uint8_t* Data)
{
	SPI_HandleTypeDef hspi3;
	HAL_GPIO_WritePin(SPI3_Chip_Select_DAC_GPIO_Port,SPI3_Chip_Select_DAC_Pin,GPIO_PIN_RESET);
	if(HAL_SPI_Transmit(&hspi3, (uint8_t*)Data, 3,1)!= HAL_OK)
	{
		Error_Handler();
	}
	HAL_GPIO_WritePin(SPI3_Chip_Select_DAC_GPIO_Port,SPI3_Chip_Select_DAC_Pin,GPIO_PIN_SET);
}
