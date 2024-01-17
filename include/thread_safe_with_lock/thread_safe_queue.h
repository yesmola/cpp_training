#include <queue>
#include <memory>
#include <condition_variable>

namespace cc
{
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        ThreadSafeQueue() {}
        ~ThreadSafeQueue()
        {
            data_.clear();
        }

        ThreadSafeQueue(const ThreadSafeQueue &rhs)
        {
            std::lock_guard lock(rhs.mu_);
            data_ = rhs.data_;
        }
        ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

        void push(T value)
        {
            std::lock_guard lock(mu_);
            data_.push(std::move(value));
            cv_.notify_all();
        }

        void wait_and_pop(T &value)
        {
            std::unique_lock lock(mu_);
            while (data_.empty())
            {
                cv_.wait(lock);
            }
            value = std::move(data_.front());
            data_.pop();
            return;
        }

        std::shared_ptr<T> wait_and_pop()
        {
            std::unique_lock lock(mu_);
            while (data_.empty())
            {
                cv_.wait(lock);
            }
            auto res = std::make_shared<T>(std::move(data_.front()));
            data_.pop();
            return res;
        }

        bool try_pop(T &value)
        {
            std::lock_guard lock(mu_);
            if (data_.empty())
            {
                return false;
            }
            value = std::move(data_.front());
            data_.pop();
            return true;
        }

        std::shared_ptr<T> try_pop()
        {
            std::lock_guard lock(mu_);
            if (data_.empty())
            {
                return nullptr;
            }
            auto res = std::make_shared<T>(std::move(data_.front()));
            data_.pop();
            return res;
        }

    private:
        std::queue<T> data_;
        std::mutex mu_;
        std::condition_variable cv_;
    };
} // namespace cc