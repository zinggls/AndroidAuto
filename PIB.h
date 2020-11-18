#ifndef __PIB_H__
#define __PIB_H__

#include "cyu3types.h"

#define ZING_GPIF_BUSWIDTH	(32)	// 8 / 16 / 32
#define ZING_BUG_WM3		(1)		// 0 : zlp, 1 : no zlp

CyU3PReturnStatus_t PIB_Init(void);

#endif
