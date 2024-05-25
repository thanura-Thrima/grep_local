#pragma once
#include <atomic>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <functional>
#include <condition_variable>

namespace Utility
{

    class ThreadPool
    {
    public:
        ThreadPool() = delete;
        ThreadPool(uint32_t threadCount);
        ~ThreadPool();
        void start();
        void addAJob(const std::function<void()> &job);
        void stop();
        void join();
        bool isRunning();

    private:
        void loop();

    private:
        uint32_t m_AvailableThreads{1};
        std::atomic<bool> m_running{true};
        std::vector<std::thread> m_threads;
        std::mutex m_mutex;
        std::condition_variable m_condition;
        std::queue<std::function<void()>> m_jobQueue;
    };

} // namespace Utility