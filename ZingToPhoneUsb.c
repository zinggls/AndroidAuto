#include "ZingToPhoneUsb.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3error.h"
#include "dma.h"
#include "Zing.h"
#include "phonedrv.h"
#include "cyu3usbhost.h"
#include "uhbuf.h"

extern CyU3PDmaChannel glChHandlePhoneDataOut;

CyU3PReturnStatus_t
CreateZingToPhoneUsbThread(
		void)
{
	CyU3PReturnStatus_t Status;

	CyU3PMemFree(zingToPhoneUsb.StackPtr_);
	zingToPhoneUsb.StackPtr_ = CyU3PMemAlloc(ZINGTOPHONEUSB_THREAD_STACK);

	CyU3PDmaBufferFree(zingToPhoneUsb.pf_);
	if((zingToPhoneUsb.pf_=(PacketFormat*)CyU3PDmaBufferAlloc(520))==0) return CY_U3P_ERROR_MEMORY_ERROR;

	Status = CyU3PThreadCreate(&zingToPhoneUsb.Handle_,		// Handle to my Application Thread
				"202:ZingToPhoneUsb",						// Thread ID and name
				ZingToPhoneUsbThread,						// Thread entry function
				0,											// Parameter passed to Thread
				zingToPhoneUsb.StackPtr_,					// Pointer to the allocated thread stack
				ZINGTOPHONEUSB_THREAD_STACK,				// Allocated thread stack size
				ZINGTOPHONEUSB_THREAD_PRIORITY,				// Thread priority
				ZINGTOPHONEUSB_THREAD_PRIORITY,				// = Thread priority so no preemption
				CYU3P_NO_TIME_SLICE,						// Time slice no supported
				CYU3P_AUTO_START							// Start the thread immediately
	);
	return Status;
}

CyBool_t
ReceiveFromGpifDataIn(
		CyU3PDmaChannel *dmaCh,
		PacketFormat *pf,
		uint32_t *length)
{
	CyU3PReturnStatus_t Status;

	if((Status=Zing_Transfer_Recv(dmaCh,(uint8_t*)pf,length,CYU3P_WAIT_FOREVER))==CY_U3P_SUCCESS) {
		zingToPhoneUsb.Count_.receiveOk++;
        if(zingToPhoneUsb.pf_->size==0) {
            CyU3PDebugPrint(4,"[Z-P] Data size(%d) received from GpifDataIn is zero, Skip further processing\r\n",zingToPhoneUsb.pf_->size);
        }else if(zingToPhoneUsb.pf_->size>512){
            CyU3PDebugPrint(4,"[Z-P] Data size(%d) received from GpifDataIn is greater than 512\r\n",zingToPhoneUsb.pf_->size);
        }
#ifdef DEBUG_THREAD_LOOP
		CyU3PDebugPrint(4,"[Z-P] %d->%d bytes received from GpifDataIn\r\n",*length,zingToPhoneUsb.pf_->size);
#endif

	}else{
		zingToPhoneUsb.Count_.receiveErr++;
		CyU3PDebugPrint (4, "[Z-P] Zing_Transfer_Recv error(0x%x)\n",Status);
		return CyFalse;
	}
	return CyTrue;
}

CyBool_t
SendToPhoneDataOut(
		uint8_t ep,
		CyU3PDmaChannel *dmaCh,
		uint8_t *buffer,
		uint16_t count)
{
	CyU3PReturnStatus_t Status;

    if ((Status=CyFxSendBuffer (ep,dmaCh,buffer,count)) != CY_U3P_SUCCESS) {
    	zingToPhoneUsb.Count_.sendErr++;
		CyU3PDebugPrint(4,"[Z-P] sending %d bytes to PhoneDataOut failed error(0x%x),EP=0x%x\r\n",count,ep);
	    return CyFalse;
    }else{
    	zingToPhoneUsb.Count_.sendOk++;
#ifdef DEBUG_THREAD_LOOP
    	CyU3PDebugPrint(4,"[Z-P] %d bytes sent to PhoneDataOut,EP=0x%x\r\n",count,ep);
#endif
    }
    return CyTrue;
}

void
ZingToPhoneUsbThread(
		uint32_t Value)
{
	uint32_t rt_len;

	CyU3PDebugPrint(4,"[Z-P] Zing to Phone USB thread starts\n");
	CyU3PDebugPrint(4,"[Z-P] GpifDataIn.size=%d\n",Dma.DataIn_.Channel_.size);
	memset(&zingToPhoneUsb.Count_,0,sizeof(zingToPhoneUsb.Count_));
	zingToPhoneUsbTerminate = CyFalse;
	while(1){
		if(CyFalse==ReceiveFromGpifDataIn(&Dma.DataIn_.Channel_,zingToPhoneUsb.pf_,&rt_len)) {
			if(zingToPhoneUsbTerminate) break;
			continue;
		}
		if(zingToPhoneUsb.pf_->size==0) continue;

		SendToPhoneDataOut(Phone.outEp,&glChHandlePhoneDataOut,zingToPhoneUsb.pf_->data,zingToPhoneUsb.pf_->size);
	}
	CyU3PDebugPrint (4, "[Z-P] ZingToPhoneUsbThread ends\n");
}
