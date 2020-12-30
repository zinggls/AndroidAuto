#ifndef __GPIFDATARECV_H__
#define __GPIFDATARECV_H__

#include "cyu3types.h"
#include "cyu3os.h"
#include "Counter.h"

#define GPIFDATARECV_THREAD_STACK		(0x800)
#define GPIFDATARECV_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateGpifDataRecvThread(
		void);

void
GpifDataRecvThread(
		uint32_t Value);

Counter gpifRecvCounter;
CyU3PThread GpifDataRecvThreadHandle;

#endif
