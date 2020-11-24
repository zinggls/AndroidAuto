#include "ZingToAutoUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"

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
	uint8_t *buf = (uint8_t *)CyU3PDmaBufferAlloc (Dma.DataIn_.Channel_.size);
	CyU3PReturnStatus_t Status;

	CyU3PDebugPrint(4,"[Z-A] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	while(1){
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,buf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
	    	if (buf[0]==0x50 && buf[1]==0x49 && buf[2]==0x4E && buf[3]==0x47 && buf[4]==0x20 && buf[5]==0x4F && buf[6]==0x4E )
	    	{
	    		/* PING ON received*/
	    		CyU3PDebugPrint(4,"[Z-A] PING ON received");
	    	}else{
				if((Status=Zing_Transfer_Send(&glChHandleAutoDataOut,buf,rt_len))==CY_U3P_SUCCESS) {
					CyU3PDebugPrint(4,"Z");
				}else{
					CyU3PDebugPrint (4, "[Z-A] Zing_DataWrite error(0x%x)\n",Status);
				}
	    	}
		}else{
			CyU3PDebugPrint (4, "[Z-A] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
