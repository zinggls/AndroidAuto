#ifndef __I2C_H__
#define __I2C_H__

#include "cyu3types.h"

#define CY_FX_USBI2C_I2C_BITRATE	(100000)

CyU3PReturnStatus_t I2C_Init(void);
void WriteI2C_test(void);
void ReadI2C_test(void);
CyU3PReturnStatus_t I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data_pt, uint32_t data_len);
CyU3PReturnStatus_t I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data_pt, uint32_t data_len);

#endif
