#include "AutoUsbToZing.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "Zing.h"

extern CyU3PDmaChannel glChHandleAutoDataIn;

CyU3PReturnStatus_t
CreateAutoUsbToZingThread(
		void)
{
	CyU3PReturnStatus_t Status;

	CyU3PMemFree(autoUsbToZing.StackPtr_);
	autoUsbToZing.StackPtr_ = CyU3PMemAlloc(AUTOUSBTOZING_THREAD_STACK);

	CyU3PDmaBufferFree(autoUsbToZing.pf_);
	if((autoUsbToZing.pf_=(PacketFormat*)CyU3PDmaBufferAlloc(520))==0) return CY_U3P_ERROR_MEMORY_ERROR;

	Status = CyU3PThreadCreate(&autoUsbToZing.Handle_,		// Handle to my Application Thread
				"101:AutoUsbToZing",						// Thread ID and name
				AutoUsbToZingThread,						// Thread entry function
				0,											// Parameter passed to Thread
				autoUsbToZing.StackPtr_,					// Pointer to the allocated thread stack
				AUTOUSBTOZING_THREAD_STACK,					// Allocated thread stack size
				AUTOUSBTOZING_THREAD_PRIORITY,				// Thread priority
				AUTOUSBTOZING_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
Receive(
		CyU3PDmaChannel *dmaCh,
		uint8_t *data,
		uint32_t *length)
{
	CyU3PReturnStatus_t Status;

	if((Status=Zing_Transfer_Recv(dmaCh,data,length,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
		autoUsbToZing.Count_.receiveOk++;
        if(*length==0) {
            CyU3PDebugPrint(4,"[A-Z] Data size(%d) received from AutoDataIn is zero, Skip further processing\r\n",*length);
            return;
        }else if(*length>512){
            CyU3PDebugPrint(4,"[A-Z] Data size(%d) received from AutoDataIn is greater than 512\r\n",*length);
        }
#ifdef DEBUG_THREAD_LOOP
		CyU3PDebugPrint(4,"[A-Z] %d bytes received from AutoDataIn\r\n",*length);
#endif
	}else{
		autoUsbToZing.Count_.receiveErr++;
		CyU3PDebugPrint (4, "[A-Z] Zing_Transfer_Recv error(0x%x)\n",Status);
	}
}

void
Send(
		PacketFormat *pf,
		uint32_t pfSize)
{
	CyU3PReturnStatus_t Status;

	autoUsbToZing.pf_->size = pfSize;
	if((Status=Zing_DataWrite((uint8_t*)pf,pfSize+sizeof(uint32_t)))==CY_U3P_SUCCESS) {
		autoUsbToZing.Count_.sendOk++;
#ifdef DEBUG_THREAD_LOOP
		CyU3PDebugPrint(4,"[A-Z] %d bytes sent to GpifDataOut\r\n",pfSize);
#endif
	}else{
		autoUsbToZing.Count_.sendErr++;
		CyU3PDebugPrint (4, "[A-Z] Zing_DataWrite(%d) error(0x%x)\n",pfSize,Status);
	}
}

void
AutoUsbToZingThread(
		uint32_t Value)
{
	uint32_t rt_len;

	if(glChHandleAutoDataIn.size==0) {
		CyU3PDebugPrint(4,"[A-Z] Waiting for AutoDataIn.size=%d to be filled in...\n",glChHandleAutoDataIn.size);
		while(glChHandleAutoDataIn.size==0) CyU3PThreadSleep (10);
	}

	CyU3PDebugPrint(4,"[A-Z] AutoDataIn.size=%d\n",glChHandleAutoDataIn.size);
	memset(&autoUsbToZing.Count_,0,sizeof(autoUsbToZing.Count_));
	while(1){
		Receive(&glChHandleAutoDataIn,(uint8_t*)autoUsbToZing.pf_->data,&rt_len);
        if(rt_len==0) {
        	CyU3PDebugPrint(4,"[A-Z] Data size(%d) received from AutoDataIn is zero, Skip further processing\r\n",rt_len);
        	continue;
        }

		Send(autoUsbToZing.pf_,rt_len);
	}
}
