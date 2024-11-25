#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) : stop(false), activeTasks(0)
{
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty())
                        return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                    ++this->activeTasks;
                }

                task();

                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    --this->activeTasks;
                    if (this->tasks.empty() && this->activeTasks == 0) {
                        this->completionCondition.notify_all();
                    }
                }
            }
            });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
        worker.join();
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
}