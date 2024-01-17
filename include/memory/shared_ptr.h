#include <iostream>
#include <cassert>
#include <vector>

namespace cc
{
    template <typename T>
    class SharedPtr
    {
    private:
        class ControlBlock
        {
        public:
            explicit ControlBlock(T *data) : data_(data)
            {
                counter_ = data_ == nullptr ? 0 : 1;
            }

            void decrease_counter()
            {
                counter_--;
            }

            void increase_counter()
            {
                counter_++;
            }

            int get_counter()
            {
                return counter_;
            }

            void clean_up()
            {
                counter_--;
                if (counter_ == 0)
                {
                    delete data_;
                }
            }

            T *data_;
            int counter_;
        };

    public:
        SharedPtr(T *data = nullptr) : cb_(new ControlBlock(data)) {}
        ~SharedPtr()
        {
            if (cb_)
            {
                cb_->clean_up();
                if (cb_->get_counter() == 0)
                {
                    delete cb_;
                }
            }
        }

        SharedPtr(const SharedPtr &rhs)
        {
            cb_ = rhs.cb_;
            if (cb_->data_ != nullptr)
            {
                cb_->increase_counter();
            }
        }

        SharedPtr(SharedPtr &&rhs)
        {
            cb_ = rhs.cb_;
            rhs.cb_ = nullptr;
        }

        SharedPtr &operator=(const SharedPtr &rhs)
        {
            cb_->clean_up();
            cb_ = rhs.cb_;
            if (cb_->data_ != nullptr)
            {
                cb_->increase_counter();
            }
            return *this;
        }

        SharedPtr &operator=(SharedPtr &&rhs)
        {
            cb_->clean_up();
            cb_ = rhs.cb_;
            rhs.cb_ = nullptr;
            return *this;
        }

        T *operator->()
        {
            return cb_->data_;
        }

        T &operator*()
        {
            return *(cb_->data_);
        }

        int get_counter()
        {
            return cb_->get_counter();
        }

    private:
        ControlBlock *cb_;
    };
} // namespace cc

namespace cctest
{
    using cc::SharedPtr;

    namespace inner
    {
        static void TEST_SharedPtr_copy()
        {
            SharedPtr<int> a(new int(3));
            assert(*a == 3);
            assert(a.get_counter() == 1);
            std::cout << "====== SharedPtr copy Success ======" << std::endl;

            {
                std::vector<SharedPtr<int>> vec(10);
                for (int i = 0; i < 10; i++)
                {
                    vec[i] = a;
                    assert(*(vec[i]) == 3);
                    assert(vec[i].get_counter() == i + 2);
                }
            }
            assert(a.get_counter() == 1);
        }

        static void TEST_SharedPtr_move()
        {
            SharedPtr<int> a(new int(3));
            SharedPtr<int> b = std::move(a);
            assert(*b == 3);
            assert(b.get_counter() == 1);
        }

    } // namespace cc inner

    static void TEST_SharedPtr()
    {
        inner::TEST_SharedPtr_copy();
        inner::TEST_SharedPtr_move();
    }
} // namespace cctesta