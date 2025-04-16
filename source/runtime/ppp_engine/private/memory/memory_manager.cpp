#include "memory/memory_manager.h"
#include "memory/memory_tracker.h"
#include "memory/memory_config.h"

#include "util/log.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        memory_region::memory_region(const memory_requirements& requirements)
            : m_heap(requirements.total_memory())
            , m_frame_heap(&m_heap, requirements.frame_heap_size)
            , m_tagged_heap(&m_heap, requirements.tagged_heap_block_size, requirements.tagged_heap_block_count)
            , m_scratch_heap(&m_heap, requirements.scratch_heap_size)
        {}

        //-------------------------------------------------------------------------
        heap* memory_region::get_heap()
        {
            return &m_heap;
        }

        //-------------------------------------------------------------------------
        linear_heap* memory_region::get_frame_heap()
        {
            return &m_frame_heap;
        }

        //-------------------------------------------------------------------------
        tagged_heap* memory_region::get_tagged_heap()
        {
            return &m_tagged_heap;
        }

        //-------------------------------------------------------------------------
        circular_heap* memory_region::get_scratch_heap()
        {
            return &m_scratch_heap;
        }

        //-------------------------------------------------------------------------
        void memory_region::free()
        {
            m_frame_heap.free();
            m_tagged_heap.free();
            m_scratch_heap.free();

            m_heap.free();
        }

        //-------------------------------------------------------------------------
        // DEVELOPMENT MEMORY MANAGER
        // Static function that returns the singleton instance
        imemory_manager& development_memory_manager::instance()
        {
            bool was_tracking = disable_tracking();

            static memory_requirements persistant;
            persistant.frame_heap_size = development_persistant_frame_heap_size();
            persistant.tagged_heap_block_size = development_persistant_tagged_heap_block_size();
            persistant.tagged_heap_block_count = development_persistant_tagged_heap_block_count();
            persistant.scratch_heap_size = development_persistant_scratch_heap_size();
            
            static memory_requirements staging;
            staging.frame_heap_size = development_staging_frame_heap_size();
            staging.tagged_heap_block_size = development_staging_tagged_heap_block_size();
            staging.tagged_heap_block_count = development_staging_tagged_heap_block_count();
            staging.scratch_heap_size = development_staging_scratch_heap_size();

            static memory_requirements debug;
            debug.frame_heap_size = development_debug_frame_heap_size();
            debug.tagged_heap_block_size = development_debug_tagged_heap_block_size();
            debug.tagged_heap_block_count = development_debug_tagged_heap_block_count();
            debug.scratch_heap_size = development_debug_scratch_heap_size();

            static development_memory_manager s_instance(persistant, staging, debug);

            if (was_tracking)
            {
                enable_tracking();
            }

            return s_instance;
        }

        //-------------------------------------------------------------------------
        development_memory_manager::development_memory_manager(const memory_requirements& persistant, const memory_requirements& staging, const memory_requirements& debug)
            : m_persistant_region(persistant)
            , m_transient_region(staging)
            , m_debug_region(debug)
        {}

        //-------------------------------------------------------------------------
        // SHIPPING MEMORY MANAGER
        // Static function that returns the singleton instance
        imemory_manager& shipping_memory_manager::instance()
        {
            bool was_tracking = disable_tracking();

            static memory_requirements persistant;
            persistant.frame_heap_size = shipping_persistant_frame_heap_size();
            persistant.tagged_heap_block_size = shipping_persistant_tagged_heap_block_size();
            persistant.tagged_heap_block_count = shipping_persistant_tagged_heap_block_count();
            persistant.scratch_heap_size = shipping_persistant_scratch_heap_size();

            static memory_requirements staging;
            staging.frame_heap_size = shipping_staging_frame_heap_size();
            staging.tagged_heap_block_size = shipping_staging_tagged_heap_block_size();
            staging.tagged_heap_block_count = shipping_staging_tagged_heap_block_count();
            staging.scratch_heap_size = shipping_staging_scratch_heap_size();

            static memory_requirements debug;
            debug.frame_heap_size = shipping_debug_frame_heap_size();
            debug.tagged_heap_block_size = shipping_debug_tagged_heap_block_size();
            debug.tagged_heap_block_count = shipping_debug_tagged_heap_block_count();
            debug.scratch_heap_size = shipping_debug_scratch_heap_size();

            static shipping_memory_manager s_instance(persistant, staging, debug);

            if (was_tracking)
            {
                enable_tracking();
            }

            return s_instance;
        }

        //-------------------------------------------------------------------------
        shipping_memory_manager::shipping_memory_manager(const memory_requirements& persistant, const memory_requirements& staging, const memory_requirements& debug)
            : m_persistant_region(persistant)
            , m_transient_region(staging)
            , m_debug_region(debug)
        {}

        //-------------------------------------------------------------------------
        // GLOBAL ACCESSORS
        imemory_manager& get_memory_manager()
        {
            #ifdef _DEBUG
            return development_memory_manager::instance();
            #else
            return shipping_memory_manager::instance();
            #endif
        }

        //-------------------------------------------------------------------------
        void print_memory_region(memory_region& region, std::string_view region_name)
        {
            log::info("=== region | {} ===", region_name);

            // Base heap information.
            auto base_heap = region.get_heap();
            log::info("Base Heap: {} / {} bytes", base_heap->current_memory().size_in_bytes(), base_heap->total_memory().size_in_bytes());

            // Frame (linear) heap.
            auto frame_heap = region.get_frame_heap();
            log::info("Frame Heap: {} / {} bytes", frame_heap->current_memory().size_in_bytes(), frame_heap->total_memory().size_in_bytes());

            // Tagged heap: iterate over each block.
            auto tagged_heap = region.get_tagged_heap();
            log::info("Tagged Heap ({} blocks):", tagged_heap->block_count());
            for (int i = 0; i < tagged_heap->block_count(); ++i)
            {
                log::info("  Block {} (Tag: {}): {} / {} bytes",
                    i,
                    tagged_heap->block_tag(i),
                    tagged_heap->block_current_size(i).size_in_bytes(),
                    tagged_heap->block_total_size().size_in_bytes());
            }

            // Scratch (circular) heap.
            auto scratch_heap = region.get_scratch_heap();
            log::info("Scratch Heap: {} / {} bytes", scratch_heap->current_memory().size_in_bytes(), scratch_heap->total_memory().size_in_bytes());

            log::info("==========================");

        }
        //-------------------------------------------------------------------------
        void print_memory_manager(imemory_manager& manager)
        {
            print_memory_region(manager.get_persistent_region(), "persistant");
            print_memory_region(manager.get_transient_region(), "staging");
            print_memory_region(manager.get_debug_region(), "debug");
        }
    }
}