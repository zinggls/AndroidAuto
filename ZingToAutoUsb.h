#ifndef __ZINGTOAUTOUSB_H__
#define __ZINGTOAUTOUSB_H__

#include "cyu3types.h"
#include "Counter.h"

#define ZINGTOAUTOUSB_THREAD_STACK		(0x1000)
#define ZINGTOAUTOUSB_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateZingToAutoUsbThread(
		void);

void
ZingToAutoUsbThread(
		uint32_t Value);

Counter zingToAutoUsbCnt;

#endif
