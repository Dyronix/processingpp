#include "memory/free_list_heap.h"
#include "memory/heap.h"

#include "util/pointer_math.h"

namespace ppp
{
    namespace memory
    {
        //-------------------------------------------------------------------------
        free_list_heap::free_list_heap(heap* heap, memory_size size)
            :m_heap(heap)
            , m_pool_memory(nullptr)
            , m_free_list(nullptr)
            , m_total_memory_size(size)
        {
            // Allocate a large contiguous block of memory from the heap.
            m_pool_memory = static_cast<u8*>(m_heap->allocate(size));

            // Initialize the free list:
            // The entire pool is initially one large free block.
            m_free_list = reinterpret_cast<block_header*>(m_pool_memory);
            m_free_list->size = size.size_in_bytes();
            m_free_list->next = nullptr;
        }

        //-------------------------------------------------------------------------
        free_list_heap::~free_list_heap()
        {
            // No need to free m_pool_memory here because its lifetime is managed by the heap.
        }

        //-------------------------------------------------------------------------
        void* free_list_heap::allocate(memory_size size) noexcept
        {
            // Convert the memory_size to a raw size in bytes.
            // Determine the required alignment (typically the alignment of max_align_t).
            u64 request_size = size.size_in_bytes();
            u64 alignment = alignof(std::max_align_t);

            // Align the requested size upward so that it meets the alignment requirement.
            request_size = align_up(size.size_in_bytes(), alignment);

            // Total size required includes the requested size plus space for the block header.
            u64 total_size = request_size + sizeof(block_header);

            block_header* prev = nullptr;
            block_header* curr = m_free_list;

            // Traverse the free list to find a free block that can accommodate the total size.
            while (curr != nullptr)
            {
                if (curr->size >= total_size)
                {
                    // Check if the block is larger than needed so that it can be split.
                    if (curr->size - total_size > sizeof(block_header))
                    {
                        // Split the block:
                        // Create a new block header for the remaining free space.
                        block_header* new_block = reinterpret_cast<block_header*>(reinterpret_cast<u8*>(curr) + total_size);

                        new_block->size = curr->size - total_size;
                        new_block->next = curr->next;

                        // Update the curr block to reflect the allocated size.
                        curr->size = total_size;

                        // Remove the curr block from the free list and replace it with the new block.
                        if (prev == nullptr)
                        {
                            m_free_list = new_block;
                        }
                        else
                        {
                            prev->next = new_block;
                        }
                    }
                    else
                    {
                        // -----------------------------------------------------------
                        // Block cannot be split (i.e., too small to hold another header).
                        // We'll allocate the entire block.

                        if (prev == nullptr)
                        {
                            m_free_list = curr->next;
                        }
                        else
                        {
                            prev->next = curr->next;
                        }
                    }

                    // Return the pointer to the usable memory (immediately after the header).
                    return reinterpret_cast<void*>(reinterpret_cast<u8*>(curr) + sizeof(block_header));
                }

                // Move to the next free block.
                prev = curr;
                curr = curr->next;
            }

            assert(false && "No suitable block found, perhaps ran out of available memory");
            return nullptr;
        }

        //-------------------------------------------------------------------------
        void free_list_heap::deallocate(void* ptr) noexcept
        {
            // Ignore null pointers.
            if (ptr == nullptr)
            {
                return;
            }

            // Retrieve the block header. The header is located just before the pointer.
            block_header* block = reinterpret_cast<block_header*>(reinterpret_cast<u8*>(ptr) - sizeof(block_header));

            // ========================================================================
            // Insert the block into the free list in sorted order (by memory address)
            // ========================================================================
            block_header** curr = &m_free_list;
            block_header* prev = nullptr;

            // Find the insertion point.
            // (The free list should be maintained in increasing order of addresses.)
            while (*curr && *curr < block)
            {
                prev = *curr;
                curr = &((*curr)->next);
            }

            // Insert the freed block into the list.
            block->next = *curr;
            *curr = block;

            // ========================================================================
            // Coalesce with the previous block (if adjacent)
            // ========================================================================
            if (prev)
            {
                // Compute the end address of the previous block.
                u8* prev_end = reinterpret_cast<u8*>(prev) + prev->size;
                if (prev_end == reinterpret_cast<u8*>(block))
                {
                    // The previous block is immediately before this block,
                    // so merge them by extending the size of the previous block.
                    prev->size += block->size;
                    prev->next = block->next;
                    // For further coalescing, treat the merged block as "block".
                    block = prev;
                }
            }

            // ========================================================================
            // Coalesce with the next block (if adjacent)
            // ========================================================================
            block_header* next = block->next;
            if (next)
            {
                // Compute the end address of the curr (or merged) block.
                u8* block_end = reinterpret_cast<u8*>(block) + block->size;
                if (block_end == reinterpret_cast<u8*>(next))
                {
                    // The curr block is immediately before the next block,
                    // so merge them by extending the size of the curr block.
                    block->size += next->size;
                    block->next = next->next;
                }
            }
        }

        //-------------------------------------------------------------------------
        void free_list_heap::free()
        {
            // Reinitialize the free list to cover the entire memory pool.
            m_free_list = reinterpret_cast<block_header*>(m_pool_memory);
            m_free_list->size = m_total_memory_size.size_in_bytes();
            m_free_list->next = nullptr;
        }

        //-------------------------------------------------------------------------
        bool free_list_heap::can_alloc(memory_size size) const
        {
            // Convert the requested size to bytes.
            u64 request_size = size.size_in_bytes();
            // Determine the required alignment (typically the alignment of std::max_align_t).
            u64 alignment = alignof(std::max_align_t);

            // Align the requested size upward so that it meets the alignment requirement.
            request_size = align_up(request_size, alignment);

            // Total size needed includes the requested size plus the space for the block header.
            u64 total_size = request_size + sizeof(block_header);

            // Traverse the free list to see if any free block is large enough.
            for (block_header* curr = m_free_list; curr != nullptr; curr = curr->next)
            {
                if (curr->size >= total_size)
                {
                    return true;
                }
            }

            return false;
        }

        //-------------------------------------------------------------------------
        memory_size free_list_heap::current_size() const
        {
            u64 free_memory = 0;
            block_header* curr = m_free_list;

            while (curr != nullptr)
            {
                free_memory += curr->size;
                curr = curr->next;
            }

            return m_total_memory_size.size_in_bytes() - free_memory;
        }

        //-------------------------------------------------------------------------
        memory_size free_list_heap::total_size() const
        {
            return m_total_memory_size;
        }
    }
}