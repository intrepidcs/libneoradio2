#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <thread>
#include <deque>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <nan.h>

using namespace std;

template<typename Data>
class dataQueue
{
    public:
        void write(Data data)
        {
            std::unique_lock <std::mutex> locker(mu);
            buffer_.push_back(data);
            locker.unlock();
            cond.notify_all();
        }

        Data read()
        {
            std::unique_lock <std::mutex> locker(mu);
            cond.wait(locker, [this]() { return buffer_.size() > 0; });
            Data back = buffer_.front();
            buffer_.pop_front();
            locker.unlock();
            cond.notify_all();
            return back;
        }

        //function to check if data exists before doing a read to prevent thread being locked.
        bool haveDataFromNode()
        {
            std::unique_lock <std::mutex> locker(mu);
            bool haveData = false;
            if(buffer_.size() > 0)
            {
                haveData = true;
            }
            locker.unlock();
            cond.notify_all();
            return haveData;
        }

        void readAll(std::deque <Data> &target)
        {
            std::unique_lock <std::mutex> locker(mu);
            std::copy(buffer_.begin(), buffer_.end(), std::back_inserter(target));
            buffer_.clear();
            locker.unlock();
        }

        dataQueue(){}
        ~dataQueue(){}

    private:
        std::mutex mu;
        std::condition_variable cond;
        std::deque <Data> buffer_;
};

class CustomMessage
{
    public:
        string name;
        string data;

        CustomMessage(string name, string data) : name(name), data(data) {}
        ~CustomMessage(){}
};

class DataWorker : public Nan::AsyncProgressWorker
{
    public:
        DataWorker(Nan::Callback *progress, Nan::Callback *callback, Nan::Callback *error_callback) : Nan::AsyncProgressWorker(callback), progress(progress), error_callback(error_callback)
        {
            input_closed = false;
        }

        ~DataWorker()
        {
            delete progress;
            delete error_callback;
        }

        void HandleErrorCallback()
        {
            Nan::HandleScope scope;
            v8::Local <v8::Value> argv[] = { v8::Exception::Error(Nan::New<v8::String>(ErrorMessage()).ToLocalChecked()) };
            error_callback->Call(1, argv);
        }

        void HandleOKCallback()
        {
            drainQueue();
//            callback->Call(0, NULL);
        }

        void HandleProgressCallback(const char *data, size_t size)
        {
            drainQueue();
        }

        void close()
        {
            input_closed = true;
        }

        dataQueue<CustomMessage> fromNode;

    protected:
        void writeToNode(const Nan::AsyncProgressWorker::ExecutionProgress &progress, CustomMessage &msg)
        {
            toNode.write(msg);
            progress.Send(reinterpret_cast<const char *>(&toNode), sizeof(toNode));
        }

        bool closed()
        {
            return input_closed;
        }

        Nan::Callback *progress;
        Nan::Callback *error_callback;
        dataQueue<CustomMessage> toNode;
        bool input_closed;

    private:
        void drainQueue()
        {
            Nan::HandleScope scope;

            std::deque <CustomMessage> contents;
            toNode.readAll(contents);

            for (CustomMessage &msg : contents)
            {
                v8::Local <v8::Value> argv[] = { Nan::New<v8::String>(msg.name.c_str()).ToLocalChecked(), Nan::New<v8::String>(msg.data.c_str()).ToLocalChecked() };
                progress->Call(2, argv);
            }
        }
};

DataWorker *create_worker(Nan::Callback *, Nan::Callback *, Nan::Callback *);