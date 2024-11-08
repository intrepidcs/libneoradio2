#ifndef _FIFO_H_
#define _FIFO_H_

#ifdef __cplusplus
extern "C" {
#endif 
	//#include "icsglb.h"
#include <string.h>
#include <inttypes.h>

#define INLINE_FIFO_PUSH_POP
//#define OPTIMIZE_POINTERS           // Uncomment to reset indices when the FIFO is empty
//#define OPTIMIZE_SHORT_TRANSFERS    // Experimental. Uncomment to unroll transfers of less than 4 bytes
//#define FIFO_INSPECT_PTRS             // make sure every push/pop is valid
#ifdef ICS_DEBUGGING
#define TRACK_USAGE// Uncomment during development to track maximum fifo usage - useful to dial in sizes
#endif

#ifndef ics_inline
//#error ics_inline undefined, falling back on static. Functions may not be inlined.
#define ics_inline static
#endif

#ifndef __CROSSWORKS_ARM
#define memcpy_fast memcpy
#define TRAP()
#endif

	typedef struct _fifo_t
	{
		uint8_t* ptr;
		unsigned int maxSz;
		volatile unsigned int in;
		volatile unsigned int out;
		volatile unsigned int numItems;
#ifdef TRACK_USAGE
		volatile unsigned int numItems_max;
#endif
	} fifo_t;

#define IN_PTR(f) (&(f->ptr[f->in]))
#define OUT_PTR(f) (&(f->ptr[f->out]))

#ifdef FIFO_INSPECT_PTRS
	static void FIFO_InspectPtrs(const fifo_t* f)
	{
		unsigned int expectedCount;
		if (f->in >= f->out)
		{
			if (f->numItems == f->maxSz)
				expectedCount = f->maxSz;
			else
				expectedCount = f->in - f->out;
		}
		else
			expectedCount = (f->maxSz - f->out) + f->in;
		if (expectedCount != f->numItems)
			TRAP();
	}
#endif

	ics_inline void FIFO_IncrementInPtr(fifo_t* f, unsigned int c)
	{
		f->in += c;
		if (f->in >= f->maxSz)
			f->in -= f->maxSz;
		f->numItems += c;
#ifdef TRACK_USAGE
		if (f->numItems > f->numItems_max)
			f->numItems_max = f->numItems;
#endif
#ifdef FIFO_INSPECT_PTRS
		FIFO_InspectPtrs(f);
#endif
	}

	ics_inline void FIFO_IncrementOutPtr(fifo_t* f, unsigned int c)
	{
		f->out += c;
		if (f->out >= f->maxSz)
			f->out -= f->maxSz;
		f->numItems -= c;
#ifdef FIFO_INSPECT_PTRS
		FIFO_InspectPtrs(f);
#endif
	}

	ics_inline unsigned int FIFO_GetMaxSize(const fifo_t* f)
	{
		return f->maxSz;
	}

	ics_inline unsigned int FIFO_GetFreeSpace(const fifo_t* f)
	{
		return (f->maxSz - f->numItems);
	}

	ics_inline unsigned int FIFO_GetCount(const fifo_t* f)
	{
		return f->numItems;
	}

	ics_inline unsigned int FIFO_GetOneShotWriteSize(const fifo_t* f)
	{
		return (f->maxSz - f->in) > FIFO_GetFreeSpace(f) ? FIFO_GetFreeSpace(f) : (f->maxSz - f->in);
	}

	ics_inline unsigned int FIFO_GetOneShotReadSize(const fifo_t* f)
	{
		return (f->maxSz - f->out) > FIFO_GetCount(f) ? FIFO_GetCount(f) : (f->maxSz - f->out);
	}

	ics_inline uint8_t* FIFO_GetWritePtr(const fifo_t* f)
	{
		return IN_PTR(f);
	}

	ics_inline uint8_t* FIFO_GetReadPtr(const fifo_t* f)
	{
		return OUT_PTR(f);
	}

	ics_inline uint8_t* FIFO_Iter(const fifo_t* f, uint8_t* p)
	{
		if (p == &f->ptr[f->maxSz - 1])
			return f->ptr;
		return p + 1;
	}

	ics_inline uint8_t FIFO_PopOne(fifo_t* f)
	{
		if (!FIFO_GetCount(f))
			return 0;
		uint8_t ret = *FIFO_GetReadPtr(f);
		FIFO_IncrementOutPtr(f, 1);
		return ret;
	}

	ics_inline void FIFO_PushOne(fifo_t* f, uint8_t b)
	{
		if (!FIFO_GetFreeSpace(f))
			return;
		uint8_t* p = FIFO_GetWritePtr(f);
		*p = b;
		FIFO_IncrementInPtr(f, 1);
	}

#if defined(INLINE_FIFO_PUSH_POP)
	ics_inline void FIFO_Push(fifo_t* f, const uint8_t* bytes, unsigned int numBytes)
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

	ics_inline void FIFO_Pop(fifo_t* f, uint8_t* bytes, unsigned int numBytes)
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
#else
	void FIFO_Push(fifo_t* f, const uint8_t* bytes, unsigned int numBytes);
	void FIFO_Pop(fifo_t* f, uint8_t* bytes, unsigned int numBytes);
#endif

	void FIFO_Init(fifo_t* f, void* buffer, const unsigned int sz);
	void FIFO_Clear(fifo_t* f);
	unsigned int FIFO_Copy(fifo_t* dst, fifo_t* src, unsigned int numBytes);
#endif

#ifdef __cplusplus
}
#endif 