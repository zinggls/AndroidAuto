#include "i2c.h"
#include "cyu3error.h"
#include "cyu3i2c.h"
#include "macro.h"

CyU3PReturnStatus_t I2C_Init(void)
{
	CyU3PI2cConfig_t i2cConfig;

	CHECK(CyU3PI2cInit());	// Start the I2C driver

	i2cConfig.bitRate    = CY_FX_USBI2C_I2C_BITRATE;
	i2cConfig.busTimeout = 0xFFFFFFFF;
	i2cConfig.dmaTimeout = 0xFFFF;
	i2cConfig.isDma      = CyFalse;
	return CyU3PI2cSetConfig(&i2cConfig, NULL);
}

CyU3PReturnStatus_t I2C_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data_pt, uint32_t data_len)
{
	CyU3PI2cPreamble_t preamble;
	preamble.length    = 2;
	preamble.buffer[0] = dev_addr<<1;
	preamble.buffer[1] = reg_addr;
	preamble.ctrlMask  = 0x0000;

	return CyU3PI2cTransmitBytes(&preamble, data_pt, data_len, 0);
}

CyU3PReturnStatus_t I2C_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data_pt, uint32_t data_len)
{
	CyU3PI2cPreamble_t preamble;
	preamble.length    = 3;
	preamble.buffer[0] = dev_addr<<1;
	preamble.buffer[1] = reg_addr;
	preamble.buffer[2] = ((dev_addr<<1) | 0x01);
	preamble.ctrlMask  = 0x0002;	//start after 2nd byte (bit position)

	return CyU3PI2cReceiveBytes(&preamble, data_pt, data_len, 0);
}
