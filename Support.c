#include "Support.h"
#include "cyu3gpio.h"

// 0~1000
uint32_t RandomGen_GetNumber(void)
{
	uint32_t i, seed, rand_val;
	uint32_t rand_arr[10]={0,};

	CyU3PGpioComplexConfig_t gpioConf = {
			CyFalse,CyFalse,CyFalse,CyFalse,
			CY_U3P_GPIO_MODE_STATIC,
			CY_U3P_GPIO_NO_INTR,
			CY_U3P_GPIO_TIMER_HIGH_FREQ,
			0,100000000,0
	};
	CyU3PGpioSetComplexConfig (0, &gpioConf);

	//CyU3PDebugPrint (4, "Random = ");
	for(i=0;i<10;i++) {
		//srand((uint32_t)time(NULL));
		//srand(i);
		CyU3PGpioComplexSampleNow(0,&seed);
		srand(seed);
		rand_val = rand()%1000;
		rand_arr[i] = rand_val;
		//CyU3PDebugPrint (4, "%d, ", rand_val);
	}
	//CyU3PDebugPrint (4, "\r\n");

	//CyU3PDebugPrint (4, "RandNum = %d\r\n",rand_arr[2]);
	return rand_arr[3];
}
