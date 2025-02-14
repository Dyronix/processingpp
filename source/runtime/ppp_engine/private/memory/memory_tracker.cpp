#include "memory/memory_tracker.h"

#include "memory/heaps/tagged_heap.h"
#include "memory/memory_tags.h"
#include "memory/memory_manager.h"

#include "util/log.h"

#define PRINT_WARN_WITHIN_SAME_FRAME 0

#define PRINT_INFO_ALLOCATIONS_TOTAL 0
#define PRINT_INFO_ALLOCATION_THIS_FRAME 0
#define PRINT_INFO_ALLOCATION_AND_DEALLOCATIONS_THIS_FRAME 0
#define PRINT_INFO_ALLOCATION_ADDRESS_AND_SIZE 0

namespace ppp
{
    namespace memory
    {
        namespace internal
        {
            bool _constructing_tracker = false;
            bool _tracking_enabled = false;
        }

        class memory_tracker
        {
        public:
            //-------------------------------------------------------------------------
            memory_tracker() = default;
            //-------------------------------------------------------------------------
            ~memory_tracker() = default;

            //-------------------------------------------------------------------------
            static memory_tracker* instance()
            {
                if (s_instance == nullptr)
                {
                    internal::_constructing_tracker = true;

                    s_instance = std::make_unique<memory_tracker>();

                    internal::_constructing_tracker = false;
                }

                return s_instance.get();
            }

            //-------------------------------------------------------------------------
            void start_frame(s32 frame)
            {
                m_active_frame = frame;
            }

            //-------------------------------------------------------------------------
            void end_frame()
            {
                if (m_active_frame == -1)
                {
                    log::warn("Did not start a new frame");
                    return;
                }

                dump_memory_data();

                m_active_frame = -1;

                m_total_alloc = 0;
                m_frame_alloc_and_dealloc = 0;
            }

            //-------------------------------------------------------------------------
            void dump_memory_data()
            {
#if PRINT_INFO_ALLOCATIONS_TOTAL
                log::info("Frame nr: {} | Total amount of allocations: {}", m_active_frame, m_allocations.size());
#endif

#if PRINT_INFO_ALLOCATION_ADDRESS_AND_SIZE
                log::info("Objects that are still allocated at the end of the frame:");

                bool printed = false;
                for (const auto& pair : m_allocations)
                {
                    if (pair.second == m_active_frame)
                    {
                        log::info("Address: {}, Size: {} bytes", pair.first, m_frame_allocations[pair.first]);
                        printed = true;
                    }
                }

                if (!printed)
                {
                    log::info("None");
                }
#endif

#if PRINT_INFO_ALLOCATION_THIS_FRAME
                log::info("Total Bytes allocated this frame: {}", m_total_alloc);
#endif
#if PRINT_INFO_ALLOCATION_AND_DEALLOCATIONS_THIS_FRAME
                log::info("Total Bytes allocated and deallocated this frame: {}", m_frame_alloc_and_dealloc);
#endif
            }

            //-------------------------------------------------------------------------
            void track_allocation(void* p, u64 size)
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                m_allocations[p] = m_active_frame;
                m_frame_allocations[p] = size;

                m_total_alloc += size;
            }

            //-------------------------------------------------------------------------
            void track_deallocation(void* p)
            {
                std::lock_guard<std::mutex> lock(m_mutex);

                auto it = m_allocations.find(p);
                if (it != std::cend(m_allocations))
                {
                    if (it->second == m_active_frame && m_frame_allocations[p] > 0)
                    {
#if PRINT_WARN_WITHIN_SAME_FRAME
                        log::warn("Allocation and deallocation within the same frame");
#endif

                        m_frame_alloc_and_dealloc += m_frame_allocations[p];
                    }

                    m_frame_allocations.erase(p);
                    m_allocations.erase(p);
                }
            }

        private:
            static std::unique_ptr<memory_tracker> s_instance;

            std::unordered_map<void*, u64> m_allocations;
            std::unordered_map<void*, u64> m_frame_allocations;
            
            std::mutex m_mutex;
            
            s32 m_active_frame = -1;

            u64 m_total_alloc = 0;
            u64 m_frame_alloc_and_dealloc = 0;
        };

        //-------------------------------------------------------------------------
        std::unique_ptr<memory_tracker> memory_tracker::s_instance = nullptr;

        //-------------------------------------------------------------------------
        void start_frame(s32 frame_index)
        {
            if (internal::_tracking_enabled)
            {
                memory_tracker::instance()->start_frame(frame_index);
            }
        }

        //-------------------------------------------------------------------------
        void end_frame()
        {
            if (internal::_tracking_enabled)
            {
                memory_tracker::instance()->end_frame();
            }
        }

        //-------------------------------------------------------------------------
        void enable_tracking()
        {
            internal::_tracking_enabled = true;
        }

        //-------------------------------------------------------------------------
        void disable_tracking()
        {
            internal::_tracking_enabled = false;
        }
        
        //-------------------------------------------------------------------------
        bool is_tracking_enabled()
        {
            return internal::_tracking_enabled;
        }

        //-------------------------------------------------------------------------
        bool is_constructing_tracker()
        {
            return internal::_constructing_tracker;
        }

        //-------------------------------------------------------------------------
        void track_allocation(void* p, u64 size)
        {
            memory_tracker::instance()->track_allocation(p, size);
        }

        //-------------------------------------------------------------------------
        void track_deallocation(void* p)
        {
            memory_tracker::instance()->track_deallocation(p);
        }

        //-------------------------------------------------------------------------
        void peek()
        {
            memory_tracker::instance()->dump_memory_data();
        }
    }
}

//-------------------------------------------------------------------------
void* operator new(u64 size)
{
    assert(size != 0);

    void* p = nullptr;

    if (ppp::memory::is_tracking_enabled() && ppp::memory::is_constructing_tracker() == false)
    {
        ppp::memory::disable_tracking();
        ppp::memory::track_allocation(p, size);
        p = ppp::memory::memory_manager::instance().get_scratch_heap()->allocate(size);
        ppp::memory::enable_tracking();
    }
    else
    {
        p = malloc(size);
    }

    return p;
}

//-------------------------------------------------------------------------
void operator delete(void* p) noexcept
{
    if (ppp::memory::is_tracking_enabled() && ppp::memory::is_constructing_tracker() == false)
    {
        ppp::memory::disable_tracking();
        ppp::memory::track_deallocation(p);
        // scratch memory should free their memory all at once
        // ppp::memory::memory_manager::instance().get_scratch_heap()->deallocate(p);
        ppp::memory::enable_tracking();
    }
    else
    {
        free(p);
    }
}