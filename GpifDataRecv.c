#include "GpifDataRecv.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "PacketFormat.h"

CyU3PReturnStatus_t
CreateGpifDataRecvThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(GPIFDATARECV_THREAD_STACK);
	Status = CyU3PThreadCreate(&GpifDataRecvThreadHandle,	// Handle to my Application Thread
				"202:GpifDataRecv",							// Thread ID and name
				GpifDataRecvThread,							// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				GPIFDATARECV_THREAD_STACK,					// Allocated thread stack size
				GPIFDATARECV_THREAD_PRIORITY,				// Thread priority
				GPIFDATARECV_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
GpifDataRecvThread(
		uint32_t Value)
{
	CyU3PReturnStatus_t Status;
	uint32_t rt_len;

	PacketFormat *pf;
	if((pf=(PacketFormat*)CyU3PDmaBufferAlloc(512*2))==0){
		CyU3PDebugPrint(4,"[GpifDataRecv] PacketFormat CyU3PDmaBufferAlloc error\r\n");
		return;
	}

	CyU3PDebugPrint(4,"[GpifDataRecv] Zing to Phone USB thread starts\n");
	CyU3PDebugPrint(4,"[GpifDataRecv] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	memset(&gpifRecvCounter,0,sizeof(gpifRecvCounter));
	while(1){
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,(uint8_t*)pf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			gpifRecvCounter.Ok++;

			/* TO DO
			 * enqueue payload of reveiced PacketFormat */

#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint(4,"[GpifDataRecv] %d->%d bytes received from GpifDataIn\r\n",rt_len,pf->size);
#endif

		}else{
			gpifRecvCounter.Err++;
			CyU3PDebugPrint (4, "[GpifDataRecv] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
