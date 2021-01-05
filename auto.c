/*
 ## Cypress USB 3.0 Platform source file (cyfxbulklpmanual.c)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2018,
 ##  All Rights Reserved
 ##  UNPUBLISHED, LICENSED SOFTWARE.
 ##
 ##  CONFIDENTIAL AND PROPRIETARY INFORMATION
 ##  WHICH IS THE PROPERTY OF CYPRESS.
 ##
 ##  Use of this file is governed
 ##  by the license agreement included in the file
 ##
 ##     <install>/license/license.txt
 ##
 ##  where <install> is the Cypress software
 ##  installation root directory path.
 ##
 ## ===========================
*/

/* This file illustrates the bulkloop application example using the DMA MANUAL mode. */

/*
   This example illustrates a loopback mechanism between two USB bulk endpoints. The example comprises of
   vendor class USB enumeration descriptors with two bulk endpoints.

   A bulk OUT endpoint acts as the producer of data from the host. A bulk IN endpoint acts as the consumer
   of data to the host. The loopback is achieved with the help of a DMA MANUAL channel. DMA MANUAL Channel
   is created between the producer USB bulk endpoint and the consumer USB bulk endpoint.

   Upon every reception of data in the DMA buffer from the host, the CPU is signalled using DMA callbacks.
   The CPU then commits the DMA buffer received so that the data is transferred to the consumer endpoint
   and thereby to the host.

   The DMA buffer size is defined based on the USB speed. 64 for full speed, 512 for high speed and 1024
   for super speed. CY_FX_BULKLP_DMA_BUF_COUNT in the header file defines the number of DMA buffers.
 */

#include "cyu3system.h"
#include "cyu3os.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "auto.h"
#include "cyu3usb.h"
#include "cyu3uart.h"
#include "cyu3utils.h"
#include "gitcommit.h"
#include "setup.h"
#include "util.h"
#include "gpio.h"
#include "AutoUsbToZing.h"
#include "ZingToAutoUsb.h"
#include "ControlCh.h"

CyU3PThread     AutoAppThread;	         /* Auto application thread structure */
CyU3PDmaChannel glChHandleAutoDataIn;    /* DMA Channel handle */
CyU3PDmaChannel glChHandleAutoDataOut;   /* DMA Channel handle */

CyBool_t glIsApplnActive = CyFalse;      /* Whether the loopback application is active or not. */

/* This function initializes the debug module. The debug prints
 * are routed to the UART and can be seen using a UART console
 * running at 115200 baud rate. */
void
CyFxAutoApplnDebugInit (void)
{
    CyU3PUartConfig_t uartConfig;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    /* Initialize the UART for printing debug messages */
    apiRetStatus = CyU3PUartInit();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set UART configuration */
    CyU3PMemSet ((uint8_t *)&uartConfig, 0, sizeof (uartConfig));
    uartConfig.baudRate = CY_U3P_UART_BAUDRATE_115200;
    uartConfig.stopBit = CY_U3P_UART_ONE_STOP_BIT;
    uartConfig.parity = CY_U3P_UART_NO_PARITY;
    uartConfig.txEnable = CyTrue;
    uartConfig.rxEnable = CyFalse;
    uartConfig.flowCtrl = CyFalse;
    uartConfig.isDma = CyTrue;

    apiRetStatus = CyU3PUartSetConfig (&uartConfig, NULL);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set the UART transfer to a really large value. */
    apiRetStatus = CyU3PUartTxSetBlockXfer (0xFFFFFFFF);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Initialize the debug module. */
    apiRetStatus = CyU3PDebugInit (CY_U3P_LPP_SOCKET_UART_CONS, 8);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyFxAppErrorHandler(apiRetStatus);
    }

    CyU3PDebugPreamble (CyFalse);
}

/* This function starts the Auto application. This is called
 * when a SET_CONF event is received from the USB host. The endpoints
 * are configured and the DMA pipe is setup in this function. */
void
CyFxAutoApplnStart (
        void)
{
    uint16_t size = 0;
    CyU3PEpConfig_t epCfg;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
    CyU3PUSBSpeed_t usbSpeed = CyU3PUsbGetSpeed();

    /* First identify the usb speed. Once that is identified,
     * create a DMA channel and start the transfer on this. */

    /* Based on the Bus Speed configure the endpoint packet size */
    CyU3PDebugPrint (1, "CyFxAutoApplnStart start\r\n",size);
    switch (usbSpeed)
    {
        case CY_U3P_FULL_SPEED:
            size = 64;
            CyU3PDebugPrint (1, "CyFxAutoApplnStart, CY_U3P_FULL_SPEED(%d)\r\n",size);
            break;

        case CY_U3P_HIGH_SPEED:
            size = 512;
            CyU3PDebugPrint (1, "CyFxAutoApplnStart, CY_U3P_HIGH_SPEED(%d)\r\n",size);
            break;

        case  CY_U3P_SUPER_SPEED:
            size = 1024;
            CyU3PDebugPrint (1, "CyFxAutoApplnStart, CY_U3P_SUPER_SPEED(%d)\r\n",size);
            break;

        default:
            CyU3PDebugPrint (4, "Error! Invalid USB speed.\n");
            CyFxAppErrorHandler (CY_U3P_ERROR_FAILURE);
            break;
    }

    CyU3PMemSet ((uint8_t *)&epCfg, 0, sizeof (epCfg));
    epCfg.enable = CyTrue;
    epCfg.epType = CY_U3P_USB_EP_BULK;
    epCfg.burstLen = 1;
    epCfg.streams = 0;
    epCfg.pcktSize = size;

    /* Producer endpoint configuration */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_PRODUCER, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PSetEpConfig failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Consumer endpoint configuration */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_CONSUMER, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PSetEpConfig failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Create a DMA MANUAL channel between two sockets of the U port.
     * DMA size is set based on the USB speed. */
	CyFxCreateChannel(size*8,
                      CY_FX_AUTO_DMA_BUF_COUNT,
                      CY_FX_EP_PRODUCER_SOCKET,
                      CY_U3P_CPU_SOCKET_CONS,
                      CY_U3P_DMA_CB_PROD_EVENT,
                      0,
                      &glChHandleAutoDataIn,
                      CY_U3P_DMA_TYPE_MANUAL_IN);
	CyU3PDebugPrint(4,"[Auto] USB Prod - CPU Consumer DMA channel created\n");

	CyFxCreateChannel(size*8,
                      CY_FX_AUTO_DMA_BUF_COUNT,
                      CY_U3P_CPU_SOCKET_PROD,
                      CY_FX_EP_CONSUMER_SOCKET,
                      CY_U3P_DMA_CB_PROD_EVENT,
                      0,
                      &glChHandleAutoDataOut,
                      CY_U3P_DMA_TYPE_MANUAL_OUT);
	CyU3PDebugPrint(4,"[Auto] CPU Producer - USB Consumer DMA channel created\n");

    /* Flush the Endpoint memory */
    CyU3PUsbFlushEp(CY_FX_EP_PRODUCER);
    CyU3PUsbFlushEp(CY_FX_EP_CONSUMER);

    /* Update the status flag. */
    glIsApplnActive = CyTrue;
    CyU3PDebugPrint (1, "CyFxAutoApplnStart end\r\n",size);
}

/* This function stops the Auto application. This shall be called whenever
 * a RESET or DISCONNECT event is received from the USB host. The endpoints are
 * disabled and the DMA pipe is destroyed by this function. */
void
CyFxAutoApplnStop (
        void)
{
    CyU3PEpConfig_t epCfg;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    CyU3PDebugPrint (1, "CyFxAutoApplnStop start\r\n");
    /* Update the flag. */
    glIsApplnActive = CyFalse;

    /* Unregister the EP event callback. */
    CyU3PUsbRegisterEpEvtCallback (0, 0, 0, 0);

    /* Flush the endpoint memory */
    CyU3PUsbFlushEp(CY_FX_EP_PRODUCER);
    CyU3PUsbFlushEp(CY_FX_EP_CONSUMER);

    /* Destroy the channel */
    CyU3PDmaChannelDestroy (&glChHandleAutoDataIn);
    CyU3PDmaChannelDestroy (&glChHandleAutoDataOut);

    /* Disable endpoints. */
    CyU3PMemSet ((uint8_t *)&epCfg, 0, sizeof (epCfg));
    epCfg.enable = CyFalse;

    /* Producer endpoint configuration. */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_PRODUCER, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PSetEpConfig failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler (apiRetStatus);
    }

    /* Consumer endpoint configuration. */
    apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_CONSUMER, &epCfg);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PSetEpConfig failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler (apiRetStatus);
    }
    CyU3PDebugPrint (1, "CyFxAutoApplnStop end\r\n");
}

/* Callback to handle the USB setup requests. */
CyBool_t
CyFxAutoApplnUSBSetupCB (
        uint32_t setupdat0, /* SETUP Data 0 */
        uint32_t setupdat1  /* SETUP Data 1 */
    )
{
    /* Fast enumeration is used. Only requests addressed to the interface, class,
     * vendor and unknown control requests are received by this function.
     * This application does not support any class or vendor requests. */

    uint8_t  bRequest, bReqType;
    uint8_t  bType, bTarget;
    uint16_t wValue, wIndex;
    CyBool_t isHandled = CyFalse;

    /* Decode the fields from the setup request. */
    bReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
    bType    = (bReqType & CY_U3P_USB_TYPE_MASK);
    bTarget  = (bReqType & CY_U3P_USB_TARGET_MASK);
    bRequest = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
    wValue   = ((setupdat0 & CY_U3P_USB_VALUE_MASK)   >> CY_U3P_USB_VALUE_POS);
    wIndex   = ((setupdat1 & CY_U3P_USB_INDEX_MASK)   >> CY_U3P_USB_INDEX_POS);

    if (bType == CY_U3P_USB_STANDARD_RQT)
    {
        /* Handle SET_FEATURE(FUNCTION_SUSPEND) and CLEAR_FEATURE(FUNCTION_SUSPEND)
         * requests here. It should be allowed to pass if the device is in configured
         * state and failed otherwise. */
        if ((bTarget == CY_U3P_USB_TARGET_INTF) && ((bRequest == CY_U3P_USB_SC_SET_FEATURE)
                    || (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)) && (wValue == 0))
        {
            if (glIsApplnActive)
                CyU3PUsbAckSetup ();
            else
                CyU3PUsbStall (0, CyTrue, CyFalse);

            isHandled = CyTrue;
        }

        /* CLEAR_FEATURE request for endpoint is always passed to the setup callback
         * regardless of the enumeration model used. When a clear feature is received,
         * the previous transfer has to be flushed and cleaned up. This is done at the
         * protocol level. Since this is just a loopback operation, there is no higher
         * level protocol. So flush the EP memory and reset the DMA channel associated
         * with it. If there are more than one EP associated with the channel reset both
         * the EPs. The endpoint stall and toggle / sequence number is also expected to be
         * reset. Return CyFalse to make the library clear the stall and reset the endpoint
         * toggle. Or invoke the CyU3PUsbStall (ep, CyFalse, CyTrue) and return CyTrue.
         * Here we are clearing the stall. */
        if ((bTarget == CY_U3P_USB_TARGET_ENDPT) && (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)
                && (wValue == CY_U3P_USBX_FS_EP_HALT))
        {
            if ((wIndex == CY_FX_EP_PRODUCER) || (wIndex == CY_FX_EP_CONSUMER))
            {
                if (glIsApplnActive)
                {
                    CyU3PUsbSetEpNak (CY_FX_EP_PRODUCER, CyTrue);
                    CyU3PUsbSetEpNak (CY_FX_EP_CONSUMER, CyTrue);
                    CyU3PBusyWait (125);

                    CyU3PDmaChannelReset (&glChHandleAutoDataIn);
                    CyU3PDmaChannelReset (&glChHandleAutoDataOut);
                    CyU3PUsbFlushEp (CY_FX_EP_PRODUCER);
                    CyU3PUsbFlushEp (CY_FX_EP_CONSUMER);
                    CyU3PUsbResetEp (CY_FX_EP_PRODUCER);
                    CyU3PUsbResetEp (CY_FX_EP_CONSUMER);
                    CyU3PDmaChannelSetXfer (&glChHandleAutoDataIn, CY_FX_AUTO_DMA_TX_SIZE);
                    CyU3PDmaChannelSetXfer (&glChHandleAutoDataOut, CY_FX_AUTO_DMA_TX_SIZE);
                    CyU3PUsbStall (wIndex, CyFalse, CyTrue);

                    CyU3PUsbSetEpNak (CY_FX_EP_PRODUCER, CyFalse);
                    CyU3PUsbSetEpNak (CY_FX_EP_CONSUMER, CyFalse);

                    CyU3PUsbAckSetup ();
                    isHandled = CyTrue;
                }
            }
        }
    }

    return isHandled;
}

/* This is the callback function to handle the USB events. */
void
CyFxAutoApplnUSBEventCB (
    CyU3PUsbEventType_t evtype, /* Event type */
    uint16_t            evdata  /* Event data */
    )
{
    switch (evtype)
    {
        case CY_U3P_USB_EVENT_SETCONF:
        	CyU3PDebugPrint (4, "CY_U3P_USB_EVENT_SETCONF\n");
            /* Disable the low power entry to optimize USB throughput */
            CyU3PUsbLPMDisable();		
            /* Stop the application before re-starting. */
            if (glIsApplnActive)
            {
                CyFxAutoApplnStop ();
            }
            /* Start the loop back function. */
            CyFxAutoApplnStart ();
            break;

        case CY_U3P_USB_EVENT_RESET:
        	CyU3PDebugPrint (4, "CY_U3P_USB_EVENT_RESET\n");
        case CY_U3P_USB_EVENT_DISCONNECT:
        	CyU3PDebugPrint (4, "CY_U3P_USB_EVENT_DISCONNECT\n");
            /* Stop the loop back function. */
            if (glIsApplnActive)
            {
                CyFxAutoApplnStop ();
            }
            break;

        default:
        	CyU3PDebugPrint (4, "event not handled(%d)\n",evtype);
            break;
    }
}

/* Callback function to handle LPM requests from the USB 3.0 host. This function is invoked by the API
   whenever a state change from U0 -> U1 or U0 -> U2 happens. If we return CyTrue from this function, the
   FX3 device is retained in the low power state. If we return CyFalse, the FX3 device immediately tries
   to trigger an exit back to U0.

   This application does not have any state in which we should not allow U1/U2 transitions; and therefore
   the function always return CyTrue.
 */
CyBool_t
CyFxAutoApplnLPMRqtCB (
        CyU3PUsbLinkPowerMode link_mode)
{
    return CyTrue;
}

/* This function initializes the USB Module, sets the enumeration descriptors.
 * This function does not start the bulk streaming and this is done only when
 * SET_CONF event is received. */
void
CyFxAutoApplnInit (void)
{
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    CyFxAutoSetupGpio();
    CyU3PDebugPrint(4,"[Auto] Setup GPIO OK\n");
    CyFxAutoI2cInit();
    CyU3PDebugPrint(4,"[Auto] I2C Init OK\n");
    CyFxAutoPibInit();
    CyU3PDebugPrint(4,"[Auto] PIB Init OK\n");

    ControlCh.StackPtr_ = 0;
    ControlCh.pf_ = 0;
    autoUsbToZing.StackPtr_ = 0;
    autoUsbToZing.pf_ = 0;
    zingToAutoUsb.StackPtr_ = 0;
    zingToAutoUsb.pf_ = 0;

    CyFxCreateCpuPibDmaChannels("[Auto]",CY_FX_DATA_BURST_LENGTH);
    CyU3PDebugPrint(4,"[Auto] DMA Channels for CPU-PIB Created\n");
    CyFxCreateControlChannel();
    CyU3PDebugPrint(4,"[Auto] Control Channel Thread Created\n");

    /* Start the USB functionality. */
    apiRetStatus = CyU3PUsbStart();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "CyU3PUsbStart failed to Start, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* The fast enumeration is the easiest way to setup a USB connection,
     * where all enumeration phase is handled by the library. Only the
     * class / vendor requests need to be handled by the application. */
    CyU3PUsbRegisterSetupCallback(CyFxAutoApplnUSBSetupCB, CyTrue);

    /* Register a callback to handle LPM requests from the USB 3.0 host. */
    CyU3PUsbRegisterLPMRequestCallback(CyFxAutoApplnLPMRqtCB);

    /* Setup the callback to handle the USB events. */
    CyU3PUsbRegisterEventCallback(CyFxAutoApplnUSBEventCB);

    /* Set the USB Enumeration descriptors */

    /* Super speed device descriptor. */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB30DeviceDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set device descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* High speed device descriptor. */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB20DeviceDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set device descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* BOS descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)CyFxUSBBOSDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set configuration descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Device qualifier descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFxUSBDeviceQualDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set device qualifier descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Super speed configuration descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBSSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set configuration descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* High speed configuration descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBHSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB Set Other Speed Descriptor failed, Error Code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Full speed configuration descriptor */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBFSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB Set Configuration Descriptor failed, Error Code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* String descriptor 0 */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFxUSBStringLangIDDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set string descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* String descriptor 1 */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFxUSBManufactureDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set string descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* String descriptor 2 */
    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFxUSBProductDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "USB set string descriptor failed, Error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    CyFxZingInit();
    CyU3PDebugPrint(4,"[Auto] ZING Init OK\n");

    CyFxSetHRCP();
    CyU3PDebugPrint(4,"[Auto] Set HRCP done\n");

    CyFxGolay();
    CyU3PDebugPrint(4,"[Auto] Golay code done\n");

#ifdef PERSISTENT_USB
    CyFxCreateAutoUsbToZingThread ();
    CyU3PDebugPrint(4,"[Auto] AutoUsb To Zing Thread Created\n");
#endif
    CyFxCreateZingToAutoUsbThread ();
    CyU3PDebugPrint(4,"[Auto] Zing To AutoUsb Thread Created\n");
}

/* Entry function for the AutoAppThread. */
void
AutoThread_Entry (
        uint32_t input)
{
    uint32_t iter;

    /* Initialize the debug module */
    CyFxAutoApplnDebugInit();

    CyU3PDebugPrint(4,"[Auto] Git:%s\r\n",GIT_INFO);

    /* Initialize the Auto application */
    CyFxAutoApplnInit();
    CyU3PDebugPrint(4,"[Auto] AutoAppln Init OK\r\n");

#ifdef PERSISTENT_USB
    CyFxUsbConnect();
    CyU3PDebugPrint(4,"[Auto] USB Connected\r\n");
#endif

    iter = 0;
    for (;;)
    {
        CyU3PThreadSleep (5000);
        iter++;
#ifndef DEBUG_THREAD_LOOP
        CyU3PDebugPrint (2, "%d [A->Z] Rcv(o:%d x:%d) Snd(o:%d x:%d) | [Z->A] Rcv(o:%d x:%d) Snd(o:%d x:%d)\r\n",iter,
        		autoUsbToZing.Count_.receiveOk,autoUsbToZing.Count_.receiveErr,autoUsbToZing.Count_.sendOk,autoUsbToZing.Count_.sendErr,
        		zingToAutoUsb.Count_.receiveOk,zingToAutoUsb.Count_.receiveErr,zingToAutoUsb.Count_.sendOk,zingToAutoUsb.Count_.sendErr);
#endif
    }
}

/* Application define function which creates the threads. */
void
CyFxApplicationDefine (
        void)
{
    void *ptr = NULL;
    uint32_t retThrdCreate = CY_U3P_SUCCESS;

    /* Allocate the memory for the threads */
    ptr = CyU3PMemAlloc (CY_FX_AUTO_THREAD_STACK);

    /* Create the thread for the application */
    retThrdCreate = CyU3PThreadCreate (&AutoAppThread,             /* Auto App Thread structure */
                          "1:Auto",                                /* Thread ID and Thread name */
                          AutoThread_Entry,                        /* Auto App Thread Entry function */
                          0,                                       /* No input parameter to thread */
                          ptr,                                     /* Pointer to the allocated thread stack */
                          CY_FX_AUTO_THREAD_STACK,                 /* Autop App Thread stack size */
                          CY_FX_AUTO_THREAD_PRIORITY,              /* Auto App Thread priority */
                          CY_FX_AUTO_THREAD_PRIORITY,              /* Auto App Thread priority */
                          CYU3P_NO_TIME_SLICE,                     /* No time slice for the application thread */
                          CYU3P_AUTO_START                         /* Start the Thread immediately */
                          );

    /* Check the return code */
    if (retThrdCreate != 0)
    {
        /* Thread Creation failed with the error code retThrdCreate */

        /* Add custom recovery or debug actions here */

        /* Application cannot continue */
        /* Loop indefinitely */
        while(1);
    }
}

/*
 * Main function
 */
int
main (void)
{
    CyU3PIoMatrixConfig_t io_cfg;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    CyU3PSysClockConfig_t clkCfg;

	/* setSysClk400 clock configurations */
	clkCfg.setSysClk400 = CyTrue;   /* FX3 device's master clock is set to a frequency > 400 MHz */
	clkCfg.cpuClkDiv = 2;           /* CPU clock divider */
	clkCfg.dmaClkDiv = 2;           /* DMA clock divider */
	clkCfg.mmioClkDiv = 2;          /* MMIO clock divider */
	clkCfg.useStandbyClk = CyFalse; /* device has no 32KHz clock supplied */
	clkCfg.clkSrc = CY_U3P_SYS_CLK; /* Clock source for a peripheral block  */
	status = CyU3PDeviceInit(&clkCfg);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

	status = CyU3PDeviceCacheControl(CyTrue, CyFalse, CyFalse);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

	CyU3PMemSet((uint8_t *)&io_cfg, 0, sizeof(io_cfg));
	io_cfg.isDQ32Bit = CyTrue;
	io_cfg.useUart   = CyTrue;
	io_cfg.useI2C    = CyTrue;
	io_cfg.lppMode   = CY_U3P_IO_MATRIX_LPP_DEFAULT;
	io_cfg.gpioSimpleEn[0]  = 0;
	io_cfg.gpioSimpleEn[1]  = 1<<(GPIO57-32); // TP2 in schematic
	status = CyU3PDeviceConfigureIOMatrix(&io_cfg);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* This is a non returnable call for initializing the RTOS kernel */
    CyU3PKernelEntry ();

    /* Dummy return to make the compiler happy */
    return 0;

handle_fatal_error:

    /* Cannot recover from this error. */
    while (1);
}

/* [ ] */

