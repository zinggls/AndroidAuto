#ifndef __GPIO_H__
#define __GPIO_H__

#include "cyu3types.h"

#define GPIO57						(57)
#define GPIF_BUSWIDTH_CTL0			(50)    // 50, 38
#define GPIF_BUSWIDTH_CTL1			(51)    // 51, 37

CyU3PReturnStatus_t SetupGPIO(void);

#endif
