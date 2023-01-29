#include "stm32l4xx.h"

#define MASK_LOW_BYTE 		0x0000FF
#define MASK_MID_BYTE 		0x00FF00
#define MASK_HIGH_BYTE 		0xFF0000
#define	LSB_UNUSED				2
#define DIN_REG						0x10
#define OFFSET_REG				0x20
#define GAIN_REG					0x30

void Max5318_Spi(uint8_t* Data);
void Max5318_Offset_Register(uint32_t c);
void Max5318_Gain_Write_Register(uint32_t c);
void Max5318_init(void);
