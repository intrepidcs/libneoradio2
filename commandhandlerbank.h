#ifndef __COMMAND_HANDLER_BANK_H__
#define __COMMAND_HANDLER_BANK_H__

#include "command_handler.h"
#include "radio2_frame.h"

#include <unordered_map>

template <typename T, typename S>
class CommandHandlerBanks
{
	std::unordered_map<int, CommandHandler<T, S>* > mBanks;
	const int MaxBankSize = 8;

	std::unordered_map<T, T> mCmdOffset;

public:
	CommandHandlerBanks()
	{
		for (int i=0; i < MaxBankSize; ++i)
			mBanks[i] = new CommandHandler<T, S>();
	}

	virtual ~CommandHandlerBanks()
	{
		for (auto cmd_handler : mBanks)
		{
			if (cmd_handler.second)
			{
				delete cmd_handler.second;
				cmd_handler.second = nullptr;
			}
		}
		mBanks.clear();
	}

	bool areBankMasksValid(int banks)
	{
		if (banks > 0xFF)
			return false;
		for (int i=0; i < MaxBankSize; ++i)
		{
			if (!((banks >> i) & 0x1))
				continue; // Bank isn't enabled
			if (mBanks.find(i) == mBanks.end())
				return false;
		}
		return true;
	}

	void addCmdOffset(int start_of_frame, int offset)
	{
		mCmdOffset[start_of_frame] = offset;
	}

	int getCmdOffset(int start_of_frame, T cmd)
	{
		if (mCmdOffset.find(start_of_frame) == mCmdOffset.end())
			return 0;
		return mCmdOffset[start_of_frame] + (int)cmd;
	}

	int getCmdOffset(neoRADIO2frame* frame)
	{
		if (!frame)
			return 0;
		return getCmdOffset(frame->header.start_of_frame, frame->header.command_status);
	}

	CommandHandler<T, S>* getBank(int bank_index)
	{
		if (mBanks.find(bank_index) == mBanks.end())
			return nullptr;
		return mBanks[bank_index];
	}

	CommandHandler<T, S>* operator[](int bank_index)
	{
		if (mBanks.find(bank_index) == mBanks.end())
			return nullptr;
		return mBanks[bank_index];
	}

	bool updateBankCmd(neoRADIO2frame* frame, S state)
	{
		if (!frame)
			return false;
		for (unsigned int i=0; i < mBanks.size(); ++i)
		{
			if (frame->header.bank & (1 << i))
			{
				// This bank is enabled so lets update it
#ifdef DEBUG_ANNOYING
				DEBUG_PRINT("Updating Bank %d command %s with state %d", i, mBanks[i]->name(getCmdOffset(frame)).c_str(), state);
#endif // DEBUG_ANNOYING
				mBanks[i]->setState(getCmdOffset(frame), state);
			}
		}
		return true;
	}

	bool updateBankData(neoRADIO2frame* frame)
	{
		if (!frame)
			return false;
		int cmd = getCmdOffset(frame); //+frame->header.command_status;
									   // The bank returned is an index if response message
		for (unsigned int i=0; i < mBanks.size(); ++i)
		{
			if (frame->header.bank & (1 << i))
			{	// This bank is enabled so lets update it
				mBanks[i]->clearData(cmd);
				mBanks[i]->addData(cmd, frame->data, frame->header.len);
			}
		}
		return true;
	}

	bool addCommand(T cmd, std::string name)
	{
		int success_count = 0;
		for (unsigned int i=0; i < mBanks.size(); ++i)
		{
			if (mBanks[i]->add(cmd, name))
				++success_count;
		}
		return success_count == mBanks.size();
	}

	bool removeCommand(T cmd)
	{
		int success_count = 0;
		for (auto cmd_handler : mBanks)
		{
			if (cmd_handler.second)
			{
				if (cmd_handler.second->remove(cmd))
					++success_count;
			}
		}
		return success_count == mBanks.size();
	}

	bool setCommandState(T cmd, S state)
	{
		int success_count = 0;
		for (auto cmd_handler : mBanks)
		{
			if (cmd_handler.second)
			{
				if (cmd_handler.second->setState(cmd, state))
					++success_count;
			}
		}
		return success_count == mBanks.size();
	}

	bool setCommandStateForAll(S state, bool clear_data)
	{
		int success_count = 0;
		for (auto cmd_handler : mBanks)
		{
			if (cmd_handler.second)
			{
				if (cmd_handler.second->setAllState(state))
					++success_count;
				if (clear_data)
					cmd_handler.second->clearData(cmd_handler.first);
			}
		}
		return success_count == mBanks.size();
	}

	template <typename P>
	bool isStateSet(T cmd, S state, P timeout)
	{
		int success_count = 0;
		for (auto cmd_handler : mBanks)
		{
			if (cmd_handler.second)
			{
				if (cmd_handler.second->isStateSet(cmd, state, timeout))
					++success_count;
				//else
				//	DEBUG_PRINT("NOTICE: Command: %s is not set to state (%d) in bank %d", cmd_handler.second->name(cmd).c_str(), state, cmd_handler.first);
			}
		}
		return success_count == mBanks.size();
	}
};


#endif // __COMMAND_HANDLER_BANK_H__
