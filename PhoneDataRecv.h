#ifndef __PHONEDATARECV_H__
#define __PHONEDATARECV_H__

#include "cyu3types.h"
#include "cyu3os.h"
#include "Counter.h"

#define PHONEDATARECV_THREAD_STACK		(0x1000)
#define PHONEDATARECV_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreatePhoneDataRecvThread(
		void);

void
PhoneDataRecvThread(
		uint32_t Value);

Counter phoneRecvCounter;
CyU3PThread PhoneDataRecvThreadHandle;

#endif
