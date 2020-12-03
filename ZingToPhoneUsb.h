#ifndef __ZINGTOPHONEUSB_H__
#define __ZINGTOPHONEUSB_H__

#include "cyu3types.h"
#include "Counter.h"

#define ZINGTOPHONEUSB_THREAD_STACK		(0x1000)
#define ZINGTOPHONEUSB_THREAD_PRIORITY	(8)
#define PHONE_DATAOUT_WAIT_TIMEOUT		(5000)          /* Timeout for transfers. */

CyU3PReturnStatus_t
CreateZingToPhoneUsbThread(
		void);

void
ZingToPhoneUsbThread(
		uint32_t Value);

Counter zingToPhoneUsbCnt;

#endif
