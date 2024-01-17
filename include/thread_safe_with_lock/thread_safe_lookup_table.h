#include <functional>
#include <shared_mutex>
#include <list>
#include <cassert>

namespace cc
{

    template <typename K, typename V, typename Hash = std::hash<K>>
    class ThreadSafeLookUpTable
    {
    private:
        class bucket_type
        {
        private:
            typedef std::pair<K, V> bucket_value;
            typedef std::list<bucket_value> bucket_data;
            typedef typename bucket_data::iterator bucket_iterator;

            bucket_iterator find_entry_for(const K &key)
            {
                return std::find_if(data_.begin(), data_.end(), [&](bucket_value &item)
                                    { return item.first == key; });
            }

            bucket_data data_;
            mutable std::shared_mutex mu_;

        public:
            V value_for(const K &key, const V &default_value)
            {
                std::shared_lock<std::shared_mutex> lock(mu_);
                const bucket_iterator found_entry = find_entry_for(key);
                return (found_entry == data_.end() ? default_value : found_entry->second);
            }

            void add_or_update_mappping(const K &key, const V &value)
            {
                std::unique_lock<std::shared_mutex> lock(mu_);
                const bucket_iterator found_entry = find_entry_for(key);
                if (found_entry == data_.end())
                {
                    data_.push_back(bucket_value(key, value));
                }
                else
                {
                    found_entry->second = value;
                }
            }

            void remove_mapping(const K &key)
            {
                std::shared_lock<std::shared_mutex> lock(mu_);
                const bucket_iterator found_entry = find_entry_for(key);
                if (found_entry != data_.end())
                {
                    data_.erase(found_entry);
                }
            }
        };

    public:
        typedef K key_type;
        typedef V mapped_type;
        typedef Hash hash_type;

        ThreadSafeLookUpTable(size_t bucket_num = 19, Hash const &hasher = Hash()) : buckets_(bucket_num), hasher_(hasher)
        {
            for (size_t i = 0; i < bucket_num; i++)
            {
                buckets_[i].reset(new bucket_type());
            }
        }

        ThreadSafeLookUpTable(const ThreadSafeLookUpTable &rhs) = delete;
        ThreadSafeLookUpTable &operator=(const ThreadSafeLookUpTable &rhs) = delete;

        V value_for(const K &key, const V &default_value = V())
        {
            return get_bucket(key).value_for(key, default_value);
        }

        void add_or_update_mapping(const K &key, const V &value)
        {
            get_bucket(key).add_or_update_mappping(key, value);
        }

        void remove_mapping(const K &key)
        {
            get_bucket(key).remove_mapping(key);
        }

    private:
        bucket_type &get_bucket(const K &key) const
        {
            std::size_t const index = hasher_(key) % buckets_.size();
            return *buckets_[index];
        }
        std::vector<std::unique_ptr<bucket_type>> buckets_;
        Hash hasher_;
    };
}; // namespace cc

namespace cctest
{
    using cc::ThreadSafeLookUpTable;
    static void TEST_thread_safe_lookup_table()
    {
        ThreadSafeLookUpTable<int, int> m;
        for (int i = 0; i < 1000; i++)
        {
            m.add_or_update_mapping(i, i);
            assert(m.value_for(i) == i);
            m.add_or_update_mapping(i, i + 1);
            assert(m.value_for(i) == i + 1);
        }

        for (int i = 0; i < 1000; i++)
        {
            m.remove_mapping(i);
            assert(m.value_for(i) == 0);
        }
        return;
    }
} // namespace cctest