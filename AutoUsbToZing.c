#include "AutoUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"
#include "PacketFormat.h"

CyU3PThread AutoUsbToZingThreadHandle;
extern CyU3PDmaChannel glChHandleAutoDataIn;

CyU3PReturnStatus_t
CreateAutoUsbToZingThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(AUTOUSBTOZING_THREAD_STACK);
	Status = CyU3PThreadCreate(&AutoUsbToZingThreadHandle,	// Handle to my Application Thread
				"101:AutoUsbToZing",						// Thread ID and name
				AutoUsbToZingThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				AUTOUSBTOZING_THREAD_STACK,					// Allocated thread stack size
				AUTOUSBTOZING_THREAD_PRIORITY,				// Thread priority
				AUTOUSBTOZING_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
AutoUsbToZingThread(
		uint32_t Value)
{
	uint32_t rt_len;
	CyU3PReturnStatus_t Status;

	PacketFormat *pf;
	if((pf=(PacketFormat*)CyU3PDmaBufferAlloc(512*17))==0){
		CyU3PDebugPrint(4,"[A-Z] PacketFormat CyU3PDmaBufferAlloc error\r\n");
		return;
	}

	if(glChHandleAutoDataIn.size==0) {
		CyU3PDebugPrint(4,"[A-Z] Waiting for AutoDataIn.size=%d to be filled in...\n",glChHandleAutoDataIn.size);
		while(glChHandleAutoDataIn.size==0) CyU3PThreadSleep (10);
	}

	CyU3PDebugPrint(4,"[A-Z] AutoDataIn.size=%d\n",glChHandleAutoDataIn.size);
	memset(&autoUsbToZingCnt,0,sizeof(autoUsbToZingCnt));
	while(1){
		if((Status=Zing_Transfer_Recv(&glChHandleAutoDataIn,(uint8_t*)pf->data,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			autoUsbToZingCnt.receiveOk++;
            if(rt_len==0) {
                CyU3PDebugPrint(4,"[A-Z] Data size(%D) received from AutoDataIn is zero, Skip further processing\r\n",rt_len);
                continue;
            }else if(rt_len>512){
                CyU3PDebugPrint(4,"[A-Z] Data size(%d) received from AutoDataIn is greater than 512\r\n",rt_len);
            }
#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint(4,"[A-Z] %d bytes received from AutoDataIn\r\n",rt_len);
#endif
			pf->size = rt_len;
			if((Status=Zing_DataWrite((uint8_t*)pf,pf->size+sizeof(uint32_t)))==CY_U3P_SUCCESS) {
				autoUsbToZingCnt.sendOk++;
#ifdef DEBUG_THREAD_LOOP
				CyU3PDebugPrint(4,"[A-Z] %d bytes sent to GpifDataOut\r\n",rt_len);
#endif
			}else{
				autoUsbToZingCnt.sendErr++;
				CyU3PDebugPrint (4, "[A-Z] Zing_DataWrite error(0x%x)\n",Status);
			}
		}else{
			autoUsbToZingCnt.receiveErr++;
			CyU3PDebugPrint (4, "[A-Z] Zing_Transfer_Recv error(0x%x)\n",Status);
		}
	}
}
