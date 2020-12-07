#include "ZingToAutoUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "setup.h"
#include "PacketFormat.h"

CyU3PThread ZingToAutoUsbThreadHandle;
extern CyU3PDmaChannel glChHandleAutoDataOut;

CyU3PReturnStatus_t
CreateZingToAutoUsbThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(ZINGTOAUTOUSB_THREAD_STACK);
	Status = CyU3PThreadCreate(&ZingToAutoUsbThreadHandle,	// Handle to my Application Thread
				"102:ZingToAutoUsb",						// Thread ID and name
				ZingToAutoUsbThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				ZINGTOAUTOUSB_THREAD_STACK,					// Allocated thread stack size
				ZINGTOAUTOUSB_THREAD_PRIORITY,				// Thread priority
				ZINGTOAUTOUSB_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
ZingToAutoUsbThread(
		uint32_t Value)
{
	uint32_t rt_len;
	CyU3PReturnStatus_t Status;

	PacketFormat *pf;
	if((pf=(PacketFormat*)CyU3PDmaBufferAlloc(512*17))==0){
		CyU3PDebugPrint(4,"[Z-A] PacketFormat CyU3PDmaBufferAlloc error\r\n");
		return;
	}

	CyU3PDebugPrint(4,"[Z-A] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	memset(&zingToAutoUsbCnt,0,sizeof(zingToAutoUsbCnt));
	while(1){
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,(uint8_t*)pf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			zingToAutoUsbCnt.receiveOk++;
#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint(4,"[Z-A] %d->%d bytes received from GpifDataIn\r\n",rt_len,pf->size);
#endif
#ifndef PERSISTENT_USB
			uint8_t *buf = pf->data;
	    	if (buf[0]==0x50 && buf[1]==0x49 && buf[2]==0x4E && buf[3]==0x47 && buf[4]==0x20 && buf[5]==0x4F && buf[6]==0x4E )
	    	{
	    		CyU3PDebugPrint(4,"PING ON received. Connecting USB...\r\n");
	    	    CyFxUsbConnect();
	    	    CyU3PDebugPrint(4,"USB Connected\r\n");

	    	    CyFxCreateAutoUsbToZingThread ();
	    	    CyU3PDebugPrint(4,"[Auto] AutoUsb To Zing Thread Created\n");
	    	    continue;
	    	}

	    	if (buf[0]==0x50 && buf[1]==0x49 && buf[2]==0x4E && buf[3]==0x47 && buf[4]==0x20 && buf[5]==0x4F && buf[6]==0x46 && buf[7]==0x46)
	    	{
	    		CyU3PDebugPrint(4,"PING OFF received. Disconnecting USB...\r\n");
	    		CyFxUsbDisconnect();
	    		CyU3PDebugPrint(4,"USB Disconnected\r\n");
	    		continue;
	    	}
#endif
			if((Status=Zing_Transfer_Send(&glChHandleAutoDataOut,pf->data,pf->size))==CY_U3P_SUCCESS) {
				zingToAutoUsbCnt.sendOk++;
#ifdef DEBUG_THREAD_LOOP
				CyU3PDebugPrint(4,"[A-Z] %d bytes sent to AutoDataOut\r\n",pf->size);
#endif
			}else{
				zingToAutoUsbCnt.sendErr++;
				CyU3PDebugPrint (4, "[Z-A] Zing_DataWrite error(0x%x)\n",Status);
			}
		}else{
			zingToAutoUsbCnt.receiveErr++;
			CyU3PDebugPrint (4, "[Z-A] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
