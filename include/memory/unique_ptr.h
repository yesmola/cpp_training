#include <iostream>

namespace cc
{

    template <typename T>
    class UniquePtr
    {
    public:
        UniquePtr(T *data = nullptr) : data_(data) {}

        UniquePtr(UniquePtr &&rhs)
        {
            data_ = rhs.data_;
            rhs.data_ = nullptr;
        }

        UniquePtr &operator=(UniquePtr &&rhs)
        {
            if (data_ != nullptr)
            {
                delete data_;
                data_ = nullptr;
            }
            data_ = rhs.data_;
        }

        T *operator->()
        {
            return data_;
        }

        T &operator*()
        {
            return *data_;
        }

    private:
        UniquePtr(const UniquePtr &rhs) = delete;
        UniquePtr &operator=(const UniquePtr &rhs) = delete;

        T *data_;
    };

} // namespace cc