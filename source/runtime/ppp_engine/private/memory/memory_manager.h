#pragma once

#include "memory/heaps/heap.h"
#include "memory/heaps/linear_heap.h"
#include "memory/heaps/circular_heap.h"
#include "memory/heaps/tagged_heap.h"

#include "memory/memory_tags.h"

#include <string>

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        struct memory_requirements
        {
            memory_size frame_heap_size         = 1_mb;

            memory_size tagged_heap_block_size  = 500_kb;
            s32         tagged_heap_block_count = 10;

            memory_size scratch_heap_size       = 1_mb;

            //-------------------------------------------------------------------------
            memory_size total_memory() const
            {
                return frame_heap_size + (tagged_heap_block_size * tagged_heap_block_count) + scratch_heap_size;
            }
        };

        class memory_region
        {
        public:
            memory_region(const memory_requirements& requirements);

        public:
            heap*            get_heap();
                             
            linear_heap*     get_frame_heap();
            tagged_heap*     get_tagged_heap();
            circular_heap*   get_scratch_heap();
                             
            void             free();

        private:
            // The "base" heap that owns the entire block of memory
            heap             m_heap;
            // Sub-allocators
            linear_heap      m_frame_heap;
            tagged_heap      m_tagged_heap;
            circular_heap    m_scratch_heap;
        };

        //-------------------------------------------------------------------------
        class imemory_manager
        {
        public:
            virtual ~imemory_manager() = default;

        public:
            virtual memory_region&  get_persistent_region() = 0;
            virtual memory_region&  get_staging_region() = 0;
            virtual memory_region&  get_debug_region() = 0;

            virtual void            free_persistent() = 0;
            virtual void            free_staging() = 0;
            virtual void            free_debug() = 0;
        };

        //-------------------------------------------------------------------------
        class development_memory_manager : public imemory_manager
        {
        public:
            static imemory_manager& instance();

        public:
            memory_region&          get_persistent_region() override { return m_persistant_region; }
            memory_region&          get_staging_region()    override { return m_staging_region; }
            memory_region&          get_debug_region()      override { return m_debug_region; }

            void                    free_persistent()       override { m_persistant_region.free(); }
            void                    free_staging()          override { m_staging_region.free(); }
            void                    free_debug()            override { m_debug_region.free(); }

        private:
            development_memory_manager(const memory_requirements& persistant, const memory_requirements& staging, const memory_requirements& debug);
            ~development_memory_manager() override = default;

            // Non-copyable
            development_memory_manager(const development_memory_manager&) = delete;
            development_memory_manager& operator=(const development_memory_manager&) = delete;

        private:
            memory_region m_persistant_region;
            memory_region m_staging_region;
            memory_region m_debug_region;
        };

        //-------------------------------------------------------------------------
        class shipping_memory_manager : public imemory_manager
        {
        public:
            static imemory_manager& instance();

        public:
            memory_region&          get_persistent_region() override { return m_persistant_region; }
            memory_region&          get_staging_region()    override { return m_staging_region; }
            memory_region&          get_debug_region()      override { return m_debug_region; }

            void                    free_persistent()       override { m_persistant_region.free(); }
            void                    free_staging()          override { m_staging_region.free(); }
            void                    free_debug()            override { m_debug_region.free(); }

        private:
            shipping_memory_manager(const memory_requirements& persistant, const memory_requirements& staging, const memory_requirements& debug);
            ~shipping_memory_manager() override = default;

            // Non-copyable
            shipping_memory_manager(const shipping_memory_manager&) = delete;
            shipping_memory_manager& operator=(const shipping_memory_manager&) = delete;

        private:
            memory_region m_persistant_region;
            memory_region m_staging_region;
            memory_region m_debug_region; // will be empty in a shipping build
        };

        //-------------------------------------------------------------------------
        imemory_manager& get_memory_manager();

        //-------------------------------------------------------------------------
        void print_memory_region(memory_region& region, std::string_view region_name);
        void print_memory_manager(imemory_manager& manager);

        //-------------------------------------------------------------------------
        template <typename TaggedHeapContainer, 
            typename TMemoryPolicy = TaggedHeapContainer::allocator_type::memory_policy,
            typename u32 tag = TaggedHeapContainer::allocator_type::allocator_tag, typename... Args>
        TaggedHeapContainer* create_tagged_new(Args&&... args)
        {
            auto heap = TMemoryPolicy::get_heap();

            void* p = heap->allocate(tag, sizeof(TaggedHeapContainer));

            return new (p) TaggedHeapContainer(std::forward<Args>(args)...);
        }

        //-------------------------------------------------------------------------
        template <typename GenericHeapContainer, 
            typename TMemoryPolicy = GenericHeapContainer::allocator_type::memory_policy,
            typename... Args>
        GenericHeapContainer* create_new(Args&&... args)
        {
            auto heap = TMemoryPolicy::get_heap();

            void* p = heap->allocate(sizeof(GenericHeapContainer));

            return new (p) GenericHeapContainer(std::forward<Args>(args)...);
        }
    }
}