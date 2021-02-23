#ifndef __ZINGTOAUTOUSB_H__
#define __ZINGTOAUTOUSB_H__

#include "cyu3types.h"
#include "thread.h"

#define ZINGTOAUTOUSB_THREAD_STACK		(0x800)
#define ZINGTOAUTOUSB_THREAD_PRIORITY	(8)

CyU3PReturnStatus_t
CreateZingToAutoUsbThread(
		void);

typedef struct CyU3PDmaChannel CyU3PDmaChannel;

void
SendToAutoDataOut(
		CyU3PDmaChannel *dmaCh,
		uint8_t *data,
		uint32_t length);

void
ZingToAutoUsbThread(
		uint32_t Value);

Thread_t zingToAutoUsb;

#endif
