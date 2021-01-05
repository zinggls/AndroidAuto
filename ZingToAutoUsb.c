#include "ZingToAutoUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "setup.h"
#include "PacketFormat.h"

extern CyU3PDmaChannel glChHandleAutoDataOut;
extern CyBool_t glIsApplnActive;

CyU3PReturnStatus_t
CreateZingToAutoUsbThread(
		void)
{
	CyU3PReturnStatus_t Status;

	zingToAutoUsb.StackPtr_ = CyU3PMemAlloc(ZINGTOAUTOUSB_THREAD_STACK);
	Status = CyU3PThreadCreate(&zingToAutoUsb.Handle_,		// Handle to my Application Thread
				"102:ZingToAutoUsb",						// Thread ID and name
				ZingToAutoUsbThread,						// Thread entry function
				0,											// Parameter passed to Thread
				zingToAutoUsb.StackPtr_,					// Pointer to the allocated thread stack
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
	memset(&zingToAutoUsb.Count_,0,sizeof(zingToAutoUsb.Count_));
	while(1){
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,(uint8_t*)pf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			zingToAutoUsb.Count_.receiveOk++;
            if(pf->size==0) {
                CyU3PDebugPrint(4,"[Z-A] Data size(%d) received from GpifDataIn is zero, Skip further processing\r\n",pf->size);
                continue;
            }else if(pf->size>512){
                CyU3PDebugPrint(4,"[Z-A] Data size(%d) received from GpifDataIn is greater than 512\r\n",pf->size);
            }
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
	    		CyU3PDeviceReset(CyFalse);
	    		continue;
	    	}

	    	/* If USB is connected and Application started properly, glIsApplnActive should become True.
	    	 * Otherwise, Zing_Transfer_Send should be delayed until it becomes True.
	    	 * */
	    	if(!glIsApplnActive) continue;
#endif
			if((Status=Zing_Transfer_Send(&glChHandleAutoDataOut,pf->data,pf->size))==CY_U3P_SUCCESS) {
				zingToAutoUsb.Count_.sendOk++;
#ifdef DEBUG_THREAD_LOOP
				CyU3PDebugPrint(4,"[A-Z] %d bytes sent to AutoDataOut\r\n",pf->size);
#endif
			}else{
				zingToAutoUsb.Count_.sendErr++;
				CyU3PDebugPrint (4, "[Z-A] Zing_DataWrite(%d) error(0x%x)\n",pf->size,Status);
			}
		}else{
			zingToAutoUsb.Count_.receiveErr++;
			CyU3PDebugPrint (4, "[Z-A] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
