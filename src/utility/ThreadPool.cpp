#include "ThreadPool.h"

#include <iostream>

namespace Utility
{

    ThreadPool::ThreadPool(uint32_t threadCount)
    {
        m_AvailableThreads = threadCount > 0 ? threadCount : 1;
    }

    ThreadPool::~ThreadPool()
    {
        stop();
    }

    void ThreadPool::start()
    {
        for (uint32_t i = 0; i < m_AvailableThreads; i++)
        {
            m_threads.emplace_back(std::thread(&ThreadPool::loop, this));
        }
    }

    void ThreadPool::addAJob(const std::function<void()> &job)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_jobQueue.push(job);
        }
        m_condition.notify_one();
    }

    void ThreadPool::stop()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_running.store(false);
        }
        m_condition.notify_all();

        for (auto &t : m_threads)
        {
            t.join();
        }

        m_threads.clear();
    }

    void ThreadPool::loop()
    {
        while (true)
        {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_condition.wait(lock, [this]
                                 { return !m_jobQueue.empty() || !m_running; });
                if (!m_running)
                {
                    return;
                }
                job = m_jobQueue.front();
                m_jobQueue.pop();
            }
            job();
        }
    }

    void ThreadPool::join()
    {
        for (auto &t : m_threads)
        {
            if (t.joinable())
            {
                t.join();
            }
        }
    }

    bool ThreadPool::isRunning()
    {
        return !m_jobQueue.empty() || !m_running;
    }

}