#include "gpio.h"
#include "cyu3gpio.h"
#include "cyu3error.h"
#include "macro.h"

CyU3PReturnStatus_t SetupGPIO(void)
{
	CyU3PGpioClock_t GpioClock;
	CyU3PGpioSimpleConfig_t GpioConfig;

	// Startup the GPIO module
	GpioClock.fastClkDiv = 2;
	GpioClock.slowClkDiv = 0;
	GpioClock.simpleDiv = CY_U3P_GPIO_SIMPLE_DIV_BY_2;
	GpioClock.clkSrc = CY_U3P_SYS_CLK;
	GpioClock.halfDiv = 0;
	CHECK(CyU3PGpioInit(&GpioClock, 0));

	// GPIO57 - TP2
	CHECK(CyU3PDeviceGpioOverride(GPIO57, CyTrue));

	CyU3PMemSet((uint8_t *)&GpioConfig, 0, sizeof(GpioConfig));
	GpioConfig.outValue = CyFalse;
	GpioConfig.driveLowEn = CyTrue;
	GpioConfig.driveHighEn = CyTrue;
	CHECK(CyU3PGpioSetSimpleConfig(GPIO57, &GpioConfig));

	// GPIF Bus Width
	CHECK(CyU3PDeviceGpioOverride(GPIF_BUSWIDTH_CTL0, CyTrue));

	CyU3PMemSet((uint8_t *)&GpioConfig, 0, sizeof(GpioConfig));
	GpioConfig.outValue = CyFalse;
	GpioConfig.driveLowEn = CyTrue;
	GpioConfig.driveHighEn = CyTrue;
	CHECK(CyU3PGpioSetSimpleConfig(GPIF_BUSWIDTH_CTL0, &GpioConfig));

	CHECK(CyU3PDeviceGpioOverride(GPIF_BUSWIDTH_CTL1, CyTrue));

	CyU3PMemSet((uint8_t *)&GpioConfig, 0, sizeof(GpioConfig));
	GpioConfig.outValue = CyFalse;
	GpioConfig.driveLowEn = CyTrue;
	GpioConfig.driveHighEn = CyTrue;
	return CyU3PGpioSetSimpleConfig(GPIF_BUSWIDTH_CTL1, &GpioConfig);
}
