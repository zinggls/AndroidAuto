#include "ControlCh.h"
#include "Zing.h"
#include "ZingHw.h"
#include "dma.h"
#include "Zing.h"
#include "cyu3error.h"
#include "cyu3system.h"
#include "macro.h"

CyU3PThread ControlChThreadHandle;

void ControlChThread(uint32_t Value)
{
	REG_Resp_t *resp_pt;
	uint32_t rt_len,recv,intEvt,regRead,manFrame;
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (512);
	CyU3PReturnStatus_t Status;

	recv = intEvt = regRead = manFrame = 0;
	while(1) {
		if(Dma.Mode_ == DMA_SYNC) {
			if((Status=Zing_Transfer_Recv(&Dma.ControlIn_.Channel_,buf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
				recv++;
				resp_pt = (REG_Resp_t*)buf;
				if(resp_pt->hdr.dir == 1 && resp_pt->hdr.interrupt == 1) { //Zing Interrupt Event
					// no act
					intEvt++;
#ifdef DEBUG
					CyU3PDebugPrint (4, "[ZCH] discarded interrupt event pk\r\n");
					CyU3PDebugPrint (4, "[ZCH] pk header :(MSB-LSB) 0x%x\r\n",*((uint64_t*)resp_pt));
#endif
				}
				else if(resp_pt->hdr.target == 1) { //Reg
					regRead++;
					memcpy(CcCtx.Data_,buf,rt_len);
					CcCtx.Data_idx_ = rt_len;
					CyU3PEventSet (&CcCtx.Event_, EVT_CTLCH0, CYU3P_EVENT_OR);
				}
				else if(resp_pt->hdr.dir == 1 && resp_pt->hdr.fr_type == 1) { //Management Frame
					manFrame++;
#ifdef DEBUG
					CyU3PDebugPrint (4, "[ZCH] rx management frame\r\n");
					CyU3PDebugPrint (4, "[ZCH] frame header :(MSB-LSB) 0x%x\r\n",*((uint64_t*)resp_pt));
					CyU3PDebugPrint (4, "[ZCH] frame data : ");
					for(uint32_t i=0;i<rt_len-ZING_HDR_SIZE;i++) CyU3PDebugPrint (4, "0x%X, ",buf[i+ZING_HDR_SIZE]);
					CyU3PDebugPrint (4, "\r\n");
#endif

					if(rt_len-ZING_HDR_SIZE == 4) { //EP0 : ZING MNGT_TX4B 12345678 --> ZING MNGT_RX4B
						CcCtx.MngtData_ = *(uint32_t*)(buf+ZING_HDR_SIZE);
					}
				}
#ifdef DEBUG
				CyU3PDebugPrint(4,"[ZCH] Recv:%d Interrupt:%d RegRead:%d ManFrame:%d\n",recv,intEvt,regRead,manFrame);
#endif
			}else{
				CyU3PDebugPrint (4, "[ZCH] Zing_Transfer_Recv error(0x%x)\n",Status);
			}
		}
		else {
			CyU3PThreadSleep(10);
		}
	}
	CyU3PDmaBufferFree(buf);
}

CyU3PReturnStatus_t ControlChThread_Create(void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	CHECK(CyU3PEventCreate(&CcCtx.Event_));

	StackPtr = CyU3PMemAlloc(CONTROLCH_THREAD_STACK);
	Status = CyU3PThreadCreate(&ControlChThreadHandle,	// Handle to my Application Thread
				"22:tmp2",								// Thread ID and name
				ControlChThread,						// Thread entry function
				0,										// Parameter passed to Thread
				StackPtr,								// Pointer to the allocated thread stack
				CONTROLCH_THREAD_STACK,					// Allocated thread stack size
				CONTROLCH_THREAD_PRIORITY,				// Thread priority
				CONTROLCH_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,					// Time slice no supported
				CYU3P_AUTO_START						// Start the thread immediately
	);
	return Status;
}
