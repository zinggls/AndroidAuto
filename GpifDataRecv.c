#include "GpifDataRecv.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "PacketFormat.h"

extern CyU3PQueue GpifDataQueue;

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

	CyU3PDebugPrint(4,"[GpifDataRecv] Gpif Data Receiving thread starts\n");
	CyU3PDebugPrint(4,"[GpifDataRecv] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	memset(&gpifRecvCounter,0,sizeof(gpifRecvCounter));
	while(1){
		//CyU3PDebugPrint(4,"[GpifDataRecv] Waiting for data...\r\n");
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,(uint8_t*)pf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			//CyU3PDebugPrint(4,"[GpifDataRecv] %d bytes received from Gpif\r\n",rt_len);

			PPacketFormat* packet = (PPacketFormat*)CyU3PMemAlloc(sizeof(PPacketFormat));
			//CyU3PDebugPrint(4,"[GpifDataRecv] CyU3PMemAlloc packet:0x%x, sizeof(PPacketFormat)=%d,sizeof(PPacketFormat*)=%d\r\n",packet,sizeof(PPacketFormat),sizeof(PPacketFormat*));

			packet->data = (uint8_t*)CyU3PMemAlloc(pf->size);
			//CyU3PDebugPrint(4,"[GpifDataRecv] CyU3PMemAlloc(%d) packet->data:0x%x\r\n",pf->size,packet->data);

			CyU3PMemCopy(packet->data,pf->data,pf->size);
			//CyU3PDebugPrint(4,"[GpifDataRecv] CyU3PMemCopy pf->size=%d\r\n",pf->size);
			packet->size = pf->size;

			//CyU3PDebugPrint(4,"[GpifDataRecv] CyU3PQueueSend(0x%x)...\r\n",packet);
			if((Status=CyU3PQueueSend(&GpifDataQueue,&packet,CYU3P_WAIT_FOREVER))!=CY_U3P_SUCCESS) {
				CyU3PDebugPrint(4,"[GpifDataRecv] CyU3PQueueSend failed, error=0x%x\r\n",Status);
			}else{
				//CyU3PDebugPrint(4,"[GpifDataRecv] CyU3PQueueSend %d bytes ok\r\n",packet->size);
			}
			gpifRecvCounter.Ok++;

#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint(4,"[GpifDataRecv] %d->%d bytes received from GpifDataIn\r\n",rt_len,pf->size);
#endif

		}else{
			gpifRecvCounter.Err++;
			CyU3PDebugPrint (4, "[GpifDataRecv] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
