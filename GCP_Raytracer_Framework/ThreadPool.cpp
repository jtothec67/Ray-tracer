#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0)
{
    InitialiseThreads(numThreads);
}

void ThreadPool::InitialiseThreads(size_t numThreads)
{
    for (size_t i = 0; i < numThreads; ++i)
    {
        workers.emplace_back([this]
        {
                while (true)
                {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty())
                            return;
                        task = std::move(tasks.front());
                        tasks.pop();
                        ++activeTasks;
                    }

                    task();

                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        --activeTasks;
                        if (tasks.empty() && activeTasks == 0)
                        {
                            completionCondition.notify_all();
                        }
                    }
                }
        });
    }
}

ThreadPool::~ThreadPool()
{
    Shutdown();
}

void ThreadPool::EnqueueTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::move(task));
    }
    condition.notify_one();
}

void ThreadPool::WaitForCompletion()
{
    std::unique_lock<std::mutex> lock(queueMutex);
    completionCondition.wait(lock, [this] { return tasks.empty() && activeTasks == 0; });
}

void ThreadPool::Shutdown()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
    workers.clear();
    stop = false;
}