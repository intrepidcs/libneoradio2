#ifndef __COMMAND_HANDLER_H__
#define __COMMAND_HANDLER_H__

#include <mutex>
#include <unordered_map>
#include <iterator>
#include <vector>

template <typename T, typename S>
class CommandHandler
{
	typedef std::vector<uint8_t> Data;

	typedef struct _StatusEvent
	{
		S state;
		Data data;
		std::mutex lock;
		std::string name;
	} StatusEvent;

public:

	virtual ~CommandHandler()
	{
		for (auto& cmd : mSentCmds)
		{
			if (cmd.second)
				delete cmd.second;
			cmd.second = nullptr;
		}
		mSentCmds.clear();
	}

	#define cmd_handler_add_param(cmd) cmd, #cmd
	#define cmd_handler_add_param_offset(cmd, offset) cmd+offset, #cmd
	bool add(T cmd, std::string cmd_name)
	{
		if (find(cmd))
			return false; // we already exist
		auto* status = new StatusEvent();
		status->state = (S)0;
		status->name = cmd_name;
		mSentCmds[cmd] = status;
		return true;
	}

	bool addData(T cmd, Data data)
	{
		auto status = find(cmd);
		if (!status)
			return false;
		std::lock_guard<std::mutex> lock(status->lock);
		std::copy(data.begin(), data.end(), std::back_inserter(status->data));
		return true;
	}

	bool addData(T cmd, uint8_t data)
	{
		Data temp;
		temp.push_back(data);
		return addData(cmd, temp);
	}

	bool addData(T cmd, uint8_t* data, size_t data_size)
	{
		if (!data)
			return false;
		Data temp;
		for (size_t i=0; i < data_size; ++i)
			temp.push_back(data[i]);
		return addData(cmd, temp);
	}
	bool addData(int cmd, uint8_t* data, size_t data_size)
	{
		return addData((T)cmd, data, data_size);
	}

	void clearData(T cmd)
	{
		auto status = find(cmd);
		if (status)
		{
			std::lock_guard<std::mutex> lock(status->lock);
			status->data.clear();
		}
	}

	void clearData(int cmd)
	{
		auto status = find((T)cmd);
		if (status)
		{
			std::lock_guard<std::mutex> lock(status->lock);
			status->data.clear();
		}
	}

	Data getData(T cmd)
	{
		auto status = find(cmd);
		if (!status)
			return Data();
		std::lock_guard<std::mutex> lock(status->lock);
		return status->data;
	}

	void remove(T cmd)
	{
		auto status = find(cmd);
		if (!status)
			return;
		// TODO: Can you delete a locked mutex?
		// std::lock_guard<std::mutex> lock(status->lock);
		delete status;
		status = nullptr;
		mCmd.erase(cmd);
	}

	StatusEvent* find(T cmd)
	{
		auto status = mSentCmds.find(cmd);
		if (status == mSentCmds.end())
			return nullptr;
		return status->second;
	}

	bool setState(T cmd, S state)
	{
		auto status = find(cmd);
		if (!status)
			return false;
		std::lock_guard<std::mutex> lock(status->lock);
		status->state = state;
		return true;
	}

	bool setState(int cmd, S state)
	{
		return setState((T)cmd, state);
	}

	bool setAllState(S state)
	{
		
		for (auto& cmd : mSentCmds)
		{
			std::lock_guard<std::mutex> lock(cmd.second->lock);
			cmd.second->state = state;
		}
		return true;
	}

	S getState(T cmd)
	{
		auto status = find(cmd);
		if (!status)
			return (S)NULL;
		std::lock_guard<std::mutex> lock(status->lock);
		return status->state;
	}

	S getState(int cmd)
	{
		return getState((T)cmd);
	}

	std::string name(T cmd)
	{
		auto status = find(cmd);
		if (!status)
			return std::string();
		std::lock_guard<std::mutex> lock(status->lock);
		return status->name;
	}

	std::string name(int cmd)
	{
		return name((T)cmd);
	}

	bool setName(T cmd, std::string name)
	{
		auto status = find(cmd);
		if (!status)
			return false;
		std::lock_guard<std::mutex> lock(status->lock);
		status->name = name;
		return true;
	}

	// Checks if command is set, returns false if timeout is hit
	// Example: 
	//	using namespace std::chrono;
	//	isCmdSet<std::chrono::milliseconds>(CMD, 100ms);
	template <typename P>
	bool isStateSet(T cmd, S state, P timeout)
	{
		using namespace std::chrono;
		using namespace std::this_thread;

		auto status = find(cmd);
		if (!status)
			return false;
		
		auto start_time = high_resolution_clock::now();
		//auto elapsed = duration_cast<P>();
		do
		{
			sleep_for(1ms);
			std::lock_guard<std::mutex> lock(status->lock);
			if (status->state == state)
				return true;
		} while ((high_resolution_clock::now() - start_time) <= timeout);
		return false;
	}

protected:
	std::unordered_map<T, StatusEvent*> mSentCmds;


private:


};
#endif // __COMMAND_HANDLER_H__
