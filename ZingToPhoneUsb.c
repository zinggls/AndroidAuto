#include "ZingToPhoneUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "uhbuf.h"
#include "PacketFormat.h"

CyU3PThread ZingToPhoneUsbThreadHandle;
extern CyU3PDmaChannel glChHandlePhoneDataOut;

CyU3PReturnStatus_t
CreateZingToPhoneUsbThread(
		void)
{
	void *StackPtr = NULL;
	CyU3PReturnStatus_t Status;

	StackPtr = CyU3PMemAlloc(ZINGTOPHONEUSB_THREAD_STACK);
	Status = CyU3PThreadCreate(&ZingToPhoneUsbThreadHandle,	// Handle to my Application Thread
				"202:ZingToPhoneUsb",						// Thread ID and name
				ZingToPhoneUsbThread,						// Thread entry function
				0,											// Parameter passed to Thread
				StackPtr,									// Pointer to the allocated thread stack
				ZINGTOPHONEUSB_THREAD_STACK,				// Allocated thread stack size
				ZINGTOPHONEUSB_THREAD_PRIORITY,				// Thread priority
				ZINGTOPHONEUSB_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

void
ZingToPhoneUsbThread(
		uint32_t Value)
{
	CyU3PReturnStatus_t Status;
	uint32_t rt_len;

	PacketFormat *pf;
	if((pf=(PacketFormat*)CyU3PDmaBufferAlloc(512*17))==0){
		CyU3PDebugPrint(4,"[Z-P] PacketFormat CyU3PDmaBufferAlloc error\r\n");
		return;
	}

	CyU3PDebugPrint(4,"[Z-P] Zing to Phone USB thread starts\n");
	CyU3PDebugPrint(4,"[Z-P] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	memset(&zingToPhoneUsbCnt,0,sizeof(zingToPhoneUsbCnt));
	while(1){
		if((Status=Zing_Transfer_Recv(&Dma.DataIn_.Channel_,(uint8_t*)pf,&rt_len,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
			zingToPhoneUsbCnt.receiveOk++;
#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint(4,"[Z-P] %d->%d bytes received from GpifDataIn\r\n",rt_len,pf->size);
#endif
		    if ((Status=CyFxSendBuffer (Phone.outEp,&glChHandlePhoneDataOut,pf->data,pf->size)) != CY_U3P_SUCCESS) {
		    	zingToPhoneUsbCnt.sendErr++;
#ifdef DEBUG_THREAD_LOOP
				CyU3PDebugPrint(4,"[Z-P] sending %d bytes to PhoneDataOut failed error(0x%x),EP=0x%x\r\n",pf->size,Status,Phone.outEp);
#endif
		    }else{
		    	zingToPhoneUsbCnt.sendOk++;
#ifdef DEBUG_THREAD_LOOP
		    	CyU3PDebugPrint(4,"[Z-P] %d bytes sent to PhoneDataOut,EP=0x%x\r\n",pf->size,Phone.outEp);
#endif
		    }
		}else{
			zingToPhoneUsbCnt.receiveErr++;
#ifdef DEBUG_THREAD_LOOP
			CyU3PDebugPrint (4, "[Z-P] Zing_Transfer_Recv error(0x%x)\n",Status);
#endif
		}
	}
}
