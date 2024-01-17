#include <stack>
#include <memory>

namespace cc
{

    template <typename T>
    class ThreadSafeStatck
    {
    public:
        ThreadSafeStatck() {}
        ~ThreadSafeStatck()
        {
            data_.clear();
        }

        ThreadSafeStatck(const ThreadSafeStatck &rhs)
        {
            std::lock_guard lock(rhs.mu_);
            data_ = rhs.st_;
        }
        ThreadSafeStatck &operator=(const ThreadSafeStatck &rhs) = delete;

        void push(T value)
        {
            std::lock_guard lock(mu_);
            data_.push(std::move(value));
        }

        std::shared_ptr<T> pop()
        {
            std::lock_guard lock(mu_);
            if (data_.empty())
                throw "empty_stack";
            auto res = std::make_shared<T>(std::move(data_.top()));
            data_.pop();
            return res;
        }

        void pop(T &value)
        {
            std::lock_guard lock(mu_);
            if (data_.empty())
                throw "empty_stack";
            value = std::move(data_.top());
            data_.pop();
            return;
        }

        bool empty()
        {
            std::lock_guard lock(mu_);
            return data_.empty();
        }

    private:
        std::stack<T> data_;
        std::mutex mu_;
    };

} // namespace cc