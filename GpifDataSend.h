#ifndef __GPIFDATASEND_H__
#define __GPIFDATASEND_H__

#include "cyu3types.h"
#include "cyu3os.h"
#include "Counter.h"

#define GPIFDATASEND_THREAD_STACK		(0x1000)
#define GPIFDATASEND_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateGpifDataSendThread(
		void);

void
GpifDataSendThread(
		uint32_t Value);

Counter gpifSendCounter;
CyU3PThread GpifDataSendThreadHandle;

#endif
