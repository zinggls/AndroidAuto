#include "uhbuf.h"
#include "cyu3error.h"
#include "cyu3usbhost.h"
#include "cyu3utils.h"
#include "host.h"

CyU3PReturnStatus_t
CyFxChanRecovery (
		uint8_t Ep,
		CyU3PDmaChannel Ch)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Abort and reset the endpoint. */
    if (Ep != 0)
    {
        CyU3PDmaChannelReset (&Ch);
        CyU3PUsbHostEpAbort (Ep);

        status = CyFxSendSetupRqt (0x02, CY_U3P_USB_SC_CLEAR_FEATURE,
                0, Ep, 0, glEp0Buffer);
        if (status == CY_U3P_SUCCESS)
        {
            status = CyU3PUsbHostEpReset (Ep);
        }
    }
    return status;
}

CyU3PReturnStatus_t
CyFxSendBuffer (
		uint8_t outEp,
		CyU3PDmaChannel outCh,
        uint8_t *buffer,
        uint16_t count)
{
    CyU3PDmaBuffer_t buf_p;
    CyU3PUsbHostEpStatus_t epStatus;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Setup the DMA for transfer. */
    buf_p.buffer = buffer;
    buf_p.count  = count;
    buf_p.size   = ((count + 0x0F) & ~0x0F);
    buf_p.status = 0;
    status = CyU3PDmaChannelSetupSendBuffer (&outCh, &buf_p);
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PUsbHostEpSetXfer (outEp,
                CY_U3P_USB_HOST_EPXFER_NORMAL, count);
    }
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PUsbHostEpWaitForCompletion (outEp, &epStatus,
        		CY_FX_WAIT_TIMEOUT);
    }
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PDmaChannelWaitForCompletion (&outCh, CYU3P_NO_WAIT);
    }

    if (status != CY_U3P_SUCCESS)
    {
    	CyFxChanRecovery (outEp,outCh);
    }

    return status;
}

CyU3PReturnStatus_t
CyFxRecvBuffer (
		uint8_t inpEp,
		CyU3PDmaChannel inpCh,
        uint8_t *buffer,
        uint16_t count)
{
    CyU3PDmaBuffer_t buf_p;
    CyU3PUsbHostEpStatus_t epStatus;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Setup the DMA for transfer. */
    buf_p.buffer = buffer;
    buf_p.count  = 0;
    buf_p.size   = ((count + 0x0F) & ~0x0F);
    buf_p.status = 0;
    status = CyU3PDmaChannelSetupRecvBuffer (&inpCh, &buf_p);
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PUsbHostEpSetXfer (inpEp,
                CY_U3P_USB_HOST_EPXFER_NORMAL, count);
    }
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PUsbHostEpWaitForCompletion (inpEp, &epStatus,
        		CY_FX_WAIT_TIMEOUT);
    }
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PDmaChannelWaitForCompletion (&inpCh, CYU3P_NO_WAIT);
    }

    if (status != CY_U3P_SUCCESS)
    {
    	CyFxChanRecovery (inpEp,inpCh);
    }

    return status;
}
