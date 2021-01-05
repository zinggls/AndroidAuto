#ifndef __CONTROL_CH_H__
#define __CONTROL_CH_H__

#include "cyu3types.h"
#include "thread.h"

#define CONTROLCH_THREAD_STACK		(0x800)
#define CONTROLCH_THREAD_PRIORITY	(8)
#define EVT_CTLCH0					(1 << 1)

typedef struct ConChCtx_t{
	CyU3PEvent Event_;
	uint8_t Data_[512];
	uint32_t Data_idx_;
	uint32_t MngtData_;
}ConChCtx_t;

ConChCtx_t CcCtx;

void ControlChThread(uint32_t Value);
CyU3PReturnStatus_t ControlChThread_Create(void);

Thread_t ControlCh;

#endif
