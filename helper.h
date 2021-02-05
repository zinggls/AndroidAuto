#ifndef __HELPER_H__
#define __HELPER_H__

#include <cyu3types.h>

void
SendMessage (
	const char *msg);

void
SendBuffer (
		const char *buffer,
		uint32_t size);

#endif
