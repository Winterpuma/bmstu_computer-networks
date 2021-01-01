#include "ThreadPool.hpp"

#include <iterator>
#include <algorithm>


ThreadPool::ThreadPool(std::size_t threadCount): terminate(false)
{
    if (threadCount==0)
        threadCount = std::thread::hardware_concurrency();
    // prevent potential reallocation, thereby screwing up all our hopes and dreams
    threads.reserve(threadCount);
    std::generate_n(std::back_inserter(threads), threadCount, [this]() { return std::thread{ threadTask, this }; });
}

ThreadPool::~ThreadPool()
{
    clear();

    // tell threads to stop when they can
    terminate = true;
    jobsAvailable.notify_all();

    // wait for all threads to finish
    for (auto& t : threads)
    {
        if (t.joinable())
            t.join();
    }
}


void ThreadPool::clear()
{
    std::lock_guard<std::mutex> lock{ jobsMutex };

    while (!jobs.empty())
        jobs.pop();
}

void ThreadPool::threadTask(ThreadPool* pool)
{
    while (!pool->terminate)
    {
        std::unique_lock<std::mutex> jobsLock{ pool->jobsMutex };

        if (pool->jobs.empty())
            pool->jobsAvailable.wait(jobsLock, [&]() { return pool->terminate || !(pool->jobs.empty()); });

        // check once more before grabbing a job, since we want to stop ASAP
        if (!pool->terminate)
        {

            // take next job
            auto job = std::move(pool->jobs.front());
            pool->jobs.pop();

            jobsLock.unlock();

            job();
        }
    }
}
