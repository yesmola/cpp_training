#include <cassert>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <future>
#include <vector>
#include <type_traits>
#include <functional>
#include <condition_variable>

namespace cc
{

    class ThreadPool
    {
    public:
        explicit ThreadPool(const size_t num_threads) : thread_count_(num_threads)
        {
            assert(num_threads > 0);
            running_ = true;

            for (size_t i = 0; i < thread_count_; ++i)
            {
                threads_.emplace_back(std::thread(&ThreadPool::worker, this));
            }
        }

        ~ThreadPool()
        {
            {
                std::unique_lock lk(tasks_mutex_);
                running_ = false;
            }
            task_cv_.notify_all();
            for (std::thread &worker : threads_)
            {
                worker.join();
            }
        }

        template <class F, class... Args>
        auto submit(F &&f, Args &&...args) -> std::future<std::invoke_result_t<F, Args...>>
        {
            using return_type = std::invoke_result_t<F, Args...>;

            auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(tasks_mutex_);

                // don't allow enqueueing after stopping the pool
                if (!running_)
                    throw std::runtime_error("enqueue on stopped ThreadPool");

                tasks_.emplace([task]()
                               { (*task)(); });
            }
            task_cv_.notify_one();
            return res;
        }

    private:
        void worker()
        {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock lk(tasks_mutex_);
                    while (!(!running_ || !tasks_.empty()))
                    {
                        task_cv_.wait(lk);
                    }
                    if (!running_ && tasks_.empty())
                    {
                        return;
                    }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        }
        std::vector<std::thread> threads_;

        const size_t thread_count_;
        bool running_ = false;

        mutable std::mutex tasks_mutex_ = {};
        mutable std::condition_variable task_cv_;
        std::queue<std::function<void()>> tasks_;
    };

} // namespace cc