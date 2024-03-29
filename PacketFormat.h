#ifndef __PACKETFORMAT_H__
#define __PACKETFORMAT_H__

#include "cyu3types.h"

#define USB_EP_MAX_SIZE		(512*8)

typedef struct _PacketFormat{
    uint32_t	size;
    uint8_t		data[512*8];
}PacketFormat;

#endif
