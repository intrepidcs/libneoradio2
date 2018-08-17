#ifndef __NEORADIO2_FRAME_HANDLER_H__
#define __NEORADIO2_FRAME_HANDLER_H__

#include <stdint.h>

template <typename T>
class neoRADIO2FrameHandler
{
public:
	neoRADIO2FrameHandler()
	{
		reset();
	}

	neoRADIO2FrameHandler(uint8_t* buffer, size_t buffer_size)
	{
		reset();
		deserialize(buffer, buffer_size);
	}

	neoRADIO2FrameHandler(T t)
	{
		reset();
		memcpy(&this->t, &t, sizeof(t));
	}

	void reset()
	{
		mIsValid = false;
		memset(&t, 0, sizeof(t));
	}

	bool pushByte(uint8_t& byte)
	{
		static uint8_t* ptr = (uint8_t*)&t;

		// we are already at the limit
		if ((ptr - (uint8_t*)&t) > (uint8_t)size())
			return false;
		*ptr++ = byte;
		if (ptr - (uint8_t*)&t == size())
			mIsValid = true;
		else
			mIsValid = false;
		return true;
	}

	T* frame() const
	{
		return (T*)&t;
	}

	size_t size() const
	{
		return sizeof(t);
	}
	bool isValid() const { return mIsValid; }
	bool setValid(bool valid=true) { mIsValid = valid; }

protected:
	T t;

private:
	bool mIsValid;
};

#endif // __NEORADIO2_FRAME_HANDLER_H__
