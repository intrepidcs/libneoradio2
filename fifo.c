/**
** Embedded FIFO implementation
**/

#include "fifo.h"

void FIFO_Init(fifo_t* f, void* buffer, const unsigned int sz)
{
	f->ptr = (uint8_t*)buffer;
	f->maxSz = sz;
	FIFO_Clear(f);
}

void FIFO_Clear(fifo_t* f)
{
	f->in = f->out = 0;
	f->numItems = 0;
}

#ifndef INLINE_FIFO_PUSH_POP
void FIFO_Push(fifo_t* f, const uint8_t* bytes, unsigned int numBytes)
{
	unsigned int copySz = numBytes;
	if (copySz > (f->maxSz - f->in))
		copySz = (f->maxSz - f->in);
	(void)memcpy_fast(IN_PTR(f), bytes, copySz);
	FIFO_IncrementInPtr(f, copySz);
	if (numBytes > copySz)
	{
		(void)memcpy_fast(IN_PTR(f), &bytes[copySz], (numBytes - copySz));
		FIFO_IncrementInPtr(f, (numBytes - copySz));
	}
}

void FIFO_Pop(fifo_t* f, uint8_t* bytes, unsigned int numBytes)
{
	unsigned int copySz = numBytes;
	if (copySz > (f->maxSz - f->out))
		copySz = (f->maxSz - f->out);
	(void)memcpy_fast(bytes, OUT_PTR(f), copySz);
	FIFO_IncrementOutPtr(f, copySz);
	if (numBytes > copySz)
	{
		(void)memcpy_fast(&bytes[copySz], OUT_PTR(f), (numBytes - copySz));
		FIFO_IncrementOutPtr(f, (numBytes - copySz));
	}
}
#endif
unsigned int FIFO_Copy(fifo_t* dst, fifo_t* src, unsigned int numBytes)
{
	if ((FIFO_GetFreeSpace(dst) < numBytes) || (FIFO_GetCount(src) < numBytes))
	{
		TRAP();
		return 0;
	}
	unsigned int sz = FIFO_GetOneShotReadSize(src);
	if (sz >= numBytes)
	{
		(void)FIFO_Push(dst, FIFO_GetReadPtr(src), numBytes);
		FIFO_IncrementOutPtr(src, numBytes);
	}
	else
	{
		(void)FIFO_Push(dst, FIFO_GetReadPtr(src), sz);
		FIFO_IncrementOutPtr(src, sz);
		(void)FIFO_Push(dst, FIFO_GetReadPtr(src), numBytes - sz);
		FIFO_IncrementOutPtr(src, numBytes - sz);
	}
	return 1;
}
