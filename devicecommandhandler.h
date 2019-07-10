#ifndef __DEVICE_COMMAND_HANDLER_H__
#define __DEVICE_COMMAND_HANDLER_H__

#ifdef _MSC_VER
#pragma warning(disable : 4503)
#endif

#include <unordered_map>
#include <tuple>
#include <vector>
#include <mutex>
#include <string>
#include <cassert>

template <class T>
class DeviceCommandHandler
{
public:
	typedef std::vector<uint8_t> CommandData;

	bool updateCommand(neoRADIO2frame_header* header, T cmd_state, bool bitfields)
	{
		if (!header)
			return false;
		return updateCommand(header->start_of_frame, header->device, header->bank, header->command_status, cmd_state, bitfields);
	}

	bool updateCommand(int sof, int device, int bank, int cmd, T cmd_state, bool bitfields)
	{
		if (!bitfields)
		{
			assert(bank <= 7);
			assert(device <= 7);
			createCommandInfoIfNeeded(sof, device, bank, cmd);
			std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
			mSof[sof][device][bank][cmd].state = cmd_state;
			DEBUG_PRINT_ANNOYING("updateCommand(): %d -- [0x%x][0x%x][0x%x][0x%x]", cmd_state, sof, device, bank, cmd);
			return true;
		}
		for (auto d = 0; d < 8; ++d)
		{
			if (device != 0xFF && d != device)
				continue;
			for (auto b = 0; b < 8; ++b)
			{
				if (!((1 << b) & bank))
					continue;
				createCommandInfoIfNeeded(sof, d, b, cmd);
				std::lock_guard<std::mutex> lock(mSof[sof][d][b][cmd].lock);
				mSof[sof][d][b][cmd].state = cmd_state;
			}
		}
		DEBUG_PRINT_ANNOYING("updateCommand(): %d -- [0x%x][0x%x][0x%x][0x%x] bitfield", cmd_state, sof, device, bank, cmd);
		return true;
	}

	// Only update the Command If the current_cmd_state is the same as the command's current command state.
	bool updateCommandIf(neoRADIO2frame_header* header, T current_cmd_state, T cmd_state, bool bitfields)
	{
		if (!header)
			return false;
		return updateCommandIf(header->start_of_frame, header->device, header->bank, header->command_status, current_cmd_state, cmd_state, bitfields);
	}

	// Only update the Command If the current_cmd_state is the same as the command's current command state.
	bool updateCommandIf(int sof, int device, int bank, int cmd, T current_cmd_state, T cmd_state, bool bitfields)
	{
		if (!bitfields)
		{
			assert(bank <= 7);
			assert(device <= 7);
			createCommandInfoIfNeeded(sof, device, bank, cmd);
			std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
			if (mSof[sof][device][bank][cmd].state == current_cmd_state)
			{
				mSof[sof][device][bank][cmd].state = cmd_state;
				DEBUG_PRINT_ANNOYING("updateCommand(): %d -- [0x%x][0x%x][0x%x][0x%x]", cmd_state, sof, device, bank, cmd);
			}
			return true;
		}
		for (auto d = 0; d < 8; ++d)
		{
			if (device != 0xFF && d != device)
				continue;
			for (auto b = 0; b < 8; ++b)
			{
				if (!((1 << b) & bank))
					continue;
				createCommandInfoIfNeeded(sof, d, b, cmd);
				std::lock_guard<std::mutex> lock(mSof[sof][d][b][cmd].lock);
				if (mSof[sof][d][b][cmd].state == current_cmd_state)
				{
					mSof[sof][d][b][cmd].state = cmd_state;
				}
			}
		}
		DEBUG_PRINT_ANNOYING("updateCommand(): %d -- [0x%x][0x%x][0x%x][0x%x] bitfield", cmd_state, sof, device, bank, cmd);
		return true;
	}

	bool updateData(neoRADIO2frame_header* header, std::vector<uint8_t>& data, bool bitfields)
	{
		if (!header)
			return false;
		return updateData(header->start_of_frame, header->device, header->bank, header->command_status, data, bitfields);
	}

	bool updateData(int sof, int device, int bank, int cmd, std::vector<uint8_t>& data, bool bitfields)
	{
		if (!bitfields)
		{
			assert(bank <= 7);
			assert(device <= 7);
			createCommandInfoIfNeeded(sof, device, bank, cmd);
			std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
			mSof[sof][device][bank][cmd].data = data;
			return true;
		}
		for (auto d = 0; d < 8; ++d)
		{
			if (device != 0xFF && d != device)
				continue;
			for (auto b = 0; b < 8; ++b)
			{
				if (!((1 << b) & bank))
					continue;
				createCommandInfoIfNeeded(sof, d, b, cmd);
				std::lock_guard<std::mutex> lock(mSof[sof][d][b][cmd].lock);
				mSof[sof][d][b][cmd].data = data;
			}
		}
		return true;
	}

	bool updateExtraData(neoRADIO2frame_header* header, std::vector<uint8_t>& data, bool bitfields)
	{
		if (!header)
			return false;
		return updateExtraData(header->start_of_frame, header->device, header->bank, header->command_status, data, bitfields);
	}

	bool updateExtraData(int sof, int device, int bank, int cmd, std::vector<uint8_t>& data, bool bitfields)
	{
		if (!bitfields)
		{
			assert(bank <= 7);
			assert(device <= 7);
			createCommandInfoIfNeeded(sof, device, bank, cmd);
			std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
			mSof[sof][device][bank][cmd].extra_data = data;
			return true;
		}
		for (auto d = 0; d < 8; ++d)
		{
			if (device != 0xFF && d != device)
				continue;
			for (auto b = 0; b < 8; ++b)
			{
				if (!((1 << b) & bank))
					continue;
				createCommandInfoIfNeeded(sof, d, b, cmd);
				std::lock_guard<std::mutex> lock(mSof[sof][d][b][cmd].lock);
				mSof[sof][d][b][cmd].extra_data = data;
			}
		}
		return true;
	}

	bool updateExtra(neoRADIO2frame_header* header, int extra, bool bitfields)
	{
		if (!header)
			return false;
		return updateExtra(header->start_of_frame, header->device, header->bank, header->command_status, extra, bitfields);
	}

	bool updateExtra(int sof, int device, int bank, int cmd, int extra, bool bitfields)
	{
		if (!bitfields)
		{
			assert(bank <= 7);
			assert(device <= 7);
			createCommandInfoIfNeeded(sof, device, bank, cmd);
			std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
			mSof[sof][device][bank][cmd].extra = extra;
			return true;
		}
		for (auto d = 0; d < 8; ++d)
		{
			if (device != 0xFF && d != device)
				continue;
			for (auto b = 0; b < 8; ++b)
			{
				if (!((1 << b) & bank))
					continue;
				createCommandInfoIfNeeded(sof, d, b, cmd);
				std::lock_guard<std::mutex> lock(mSof[sof][d][b][cmd].lock);
				mSof[sof][d][b][cmd].extra = extra;
			}
		}
		return true;
	}

	// This command assumes the cmd exists, no checking is done.
	T getState(int sof, int device, int bank, int cmd)
	{
		std::lock_guard<std::mutex> _lock(mLock);
		std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
		return mSof[sof][device][bank][cmd].state;
	}

	// This command assumes the cmd exists, no checking is done.
	CommandData getData(neoRADIO2frame_header* header)
	{
		return getData(header->start_of_frame, header->device, header->bank, header->command_status);
	}

	// This command assumes the cmd exists, no checking is done.
	CommandData getData(int sof, int device, int bank, int cmd)
	{
		std::lock_guard<std::mutex> _lock(mLock);
		std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
		return mSof[sof][device][bank][cmd].data;
	}

	// This command assumes the cmd exists, no checking is done.
	CommandData getExtraData(neoRADIO2frame_header* header)
	{
		return getExtraData(header->start_of_frame, header->device, header->bank, header->command_status);
	}

	// This command assumes the cmd exists, no checking is done.
	CommandData getExtraData(int sof, int device, int bank, int cmd)
	{
		std::lock_guard<std::mutex> _lock(mLock);
		std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
		return mSof[sof][device][bank][cmd].extra_data;
	}

	int getExtraState(neoRADIO2frame_header* header, int cmd)
	{
		return getExtraState(header->start_of_frame, header->device, header->bank, cmd);
	}

	int getExtraState(int sof, int device, int bank, int cmd)
	{
		std::lock_guard<std::mutex> _lock(mLock);
		std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
		return mSof[sof][device][bank][cmd].extra;
	}

	bool isStateSet(neoRADIO2frame_header* header, T cmd_state, bool bitfield)
	{
		if (!header)
			return false;
		return isStateSet(header->start_of_frame, header->device, header->bank, header->command_status, cmd_state, bitfield);
	}

	bool isStateSet(neoRADIO2frame_header* header, T cmd_state, bool bitfield, std::chrono::milliseconds timeout)
	{
		if (!header)
			return false;
		return isStateSet(header->start_of_frame, header->device, header->bank, header->command_status, cmd_state, bitfield, timeout);
	}

	bool isStateSet(int sof, int device, int bank, int cmd, T cmd_state, bool bitfield, std::chrono::milliseconds timeout)
	{
		using namespace std::chrono;
		auto start_time = std::chrono::high_resolution_clock::now();
		do
		{
			if (isStateSet(sof, device, bank, cmd, cmd_state, bitfield))
				return true;
			else
				std::this_thread::sleep_for(1ms);
		} while ((std::chrono::high_resolution_clock::now() - start_time) <= timeout);
		return false;
	}

	bool isStateSet(int sof, int device, int bank, int cmd, T cmd_state, bool bitfield)
	{
		if (!bitfield)
		{
			assert(bank <= 7);
			assert(device <= 7);
			createCommandInfoIfNeeded(sof, device, bank, cmd);
			std::lock_guard<std::mutex> _lock(mLock);
			std::lock_guard<std::mutex> lock(mSof[sof][device][bank][cmd].lock);
			return mSof[sof][device][bank][cmd].state == cmd_state;
		}
		int enable_count = 0;
		int matched_count = 0;
		for (auto d=0; d < 8; ++d)
		{
			if (device != 0xFF && d != device)
				continue;
			for (auto b=0; b < 8; ++b)
			{
				if (!((1 << b) & bank))
					continue;
				++enable_count;
				createCommandInfoIfNeeded(sof, d, b, cmd);
				std::lock_guard<std::mutex> _lock(mLock);
				std::lock_guard<std::mutex> lock(mSof[sof][d][b][cmd].lock);
				if (mSof[sof][d][b][cmd].state == cmd_state)
					++matched_count;
			}
		}
		return (enable_count == matched_count) && (matched_count != 0);
	}
private:

	// <start of frame <cmd, is_bitfield> >
	std::map<int, std::tuple<int, bool> > mBitfieldOverride;

	
	typedef struct _CommandInfo
	{
		T state;
		CommandData data;
		std::mutex lock;

		// Command specific "extra" integer
		// for generic use.
		int extra;
		CommandData extra_data;

	} CommandInfo;

	// mSof[sof][device][bank][cmd] = CommandInfo
	typedef std::unordered_map<int, CommandInfo> CommandInfoMap;
	typedef std::unordered_map<int, CommandInfoMap> BankMap;
	typedef std::unordered_map<int, BankMap> DeviceMap;
	typedef std::unordered_map<int, DeviceMap> StartOfFrameMap;
	StartOfFrameMap mSof;

	void createSofIfNeeded(int sof)
	{
		std::lock_guard<std::mutex> _lock(mLock);
		// create the start of frame key
		if (mSof.find(sof) == mSof.end())
			mSof[sof];
	}

	void createDeviceIfNeeded(int sof, int device)
	{
		createSofIfNeeded(sof);
		std::lock_guard<std::mutex> _lock(mLock);
		if (mSof[sof].find(device) == mSof[sof].end())
			mSof[sof][device];
	}

	void createBankIfNeeded(int sof, int device, int bank)
	{
		createDeviceIfNeeded(sof, device);
		std::lock_guard<std::mutex> _lock(mLock);
		if (mSof[sof][device].find(bank) == mSof[sof][device].end())
		{
			mSof[sof][device];
		}
	}

	void createCommandInfoIfNeeded(int sof, int device, int bank, int cmd)
	{
		createBankIfNeeded(sof, device, bank);
		std::lock_guard<std::mutex> _lock(mLock);
		if (mSof[sof][device][bank].find(cmd) == mSof[sof][device][bank].end())
		{
			DEBUG_PRINT_ANNOYING("Creating CommandInfo on [0x%x][0x%x][0x%x][0x%x]", sof, device, bank, cmd);
			assert(device != 0xFF);
			assert(bank != 0xFF);
			mSof[sof][device][bank][cmd];
		}
	}

private:
	std::mutex mLock;
};

#endif // __DEVICE_COMMAND_HANDLER_H__
