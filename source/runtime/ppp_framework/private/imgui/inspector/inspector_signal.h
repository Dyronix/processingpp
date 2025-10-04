#pragma once

#include <mutex>
#include <vector>
#include <functional>

namespace ppp
{
    namespace imgui
    {
        namespace inspector
        {
            // Lightweight signal/slot with integer connections. Thread-safe subscribe/emit.
            // - Subscribe returns a connection id. Keep it if you want to unsubscribe later.
            // - You can also clear() to remove all listeners at once.
            // - This is header-only except for the bus singleton in the .cpp.

            using connection_id = std::uint64_t;

            namespace detail 
            {
                inline connection_id next_connection_id() 
                {
                    static std::atomic_uint64_t s{ 1 };
                    return s.fetch_add(1, std::memory_order_relaxed);
                }
            }

            template <typename... Args>
            class inspector_signal 
            {
            public:
                using listener = std::function<void(Args...)>;

                connection_id subscribe(listener cb) 
                {
                    const connection_id id = detail::next_connection_id();
                    std::scoped_lock lock(m_mutex);
                    m_listeners.emplace(id, std::move(cb));
                    return id;
                }

                bool unsubscribe(connection_id id) 
                {
                    std::scoped_lock lock(m_mutex);
                    return m_listeners.erase(id) > 0;
                }

                void clear() 
                {
                    std::scoped_lock lock(m_mutex);
                    m_listeners.clear();
                }

                // Emit to a snapshot to avoid iterator invalidation if handlers unsubscribe during emit
                void emit(Args... args) const 
                {
                    std::vector<listener> snapshot;
                    {
                        std::scoped_lock lock(m_mutex);
                        snapshot.reserve(m_listeners.size());
                        for (auto& kv : m_listeners) snapshot.push_back(kv.second);
                    }

                    for (auto& fn : snapshot) 
                    {
                        fn(args...); 
                    }
                }

                bool empty() const 
                {
                    std::scoped_lock lock(m_mutex);
                    return m_listeners.empty();
                }

            private:
                mutable std::mutex m_mutex;
                std::unordered_map<connection_id, listener> m_listeners;
            };
        }
    }
}