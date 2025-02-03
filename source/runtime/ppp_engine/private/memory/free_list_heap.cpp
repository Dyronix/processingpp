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
            , m_current_memory_size(0)
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
        void* free_list_heap::allocate(memory_size size)
        {
            // Convert the memory_size to a raw size in bytes.
            // Determine the required alignment (typically the alignment of max_align_t).
            u64 request_size = size.size_in_bytes();
            u64 alignment = alignof(std::max_align_t);

            // Align the requested size upward so that it meets the alignment requirement.
            request_size = align_up(size.size_in_bytes(), alignment);

            // Total size required includes the requested size plus space for the block header.
            u64 total_size = request_size + sizeof(block_header);

            assert(m_current_memory_size.size_in_bytes() + total_size <= m_total_memory_size.size_in_bytes());

            m_current_memory_size += total_size;

            block_header* prev = nullptr;
            block_header* curr = m_free_list;

            // Traverse the free list to find a free block that can accommodate the total size.
            while (curr != nullptr)
            {
                if (curr->size >= total_size)
                {
                    // Suitable block found.
                    // Check if the block is larger than needed so that it can be split.
                    if (curr->size - total_size > sizeof(block_header))
                    {
                        // Split the block:
                        // Create a new block header for the remaining free space.
                        block_header* new_block = reinterpret_cast<block_header*>(reinterpret_cast<u8*>(curr) + total_size);

                        new_block->size = curr->size - total_size;
                        new_block->next = curr->next;

                        // Update the current block to reflect the allocated size.
                        curr->size = total_size;

                        // Remove the current block from the free list and replace it with the new block.
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
                        // If the block cannot be split (i.e., too small to hold another header),
                        //      then remove the entire block from the free list.
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

            assert("No suitable block found, perhaps ran out of available memory");
            return nullptr;
        }

        //-------------------------------------------------------------------------
        void free_list_heap::deallocate(void* ptr)
        {
            // Ignore null pointers.
            if (ptr == nullptr)
            {
                return;
            }

            // Retrieve the block header. The header is located just before the pointer.
            block_header* block = reinterpret_cast<block_header*>(reinterpret_cast<u8*>(ptr) - sizeof(block_header));

            // Insert the freed block at the head of the free list.
            block->next = m_free_list;
            m_free_list = block;
        }
    }
}