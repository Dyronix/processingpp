#pragma once

#include "memory/allocators/tagged_heap_allocator.h"

namespace ppp
{
    //--------------------------------------------------------------
    // Custom smart pointer that uses free_list_allocator for deallocation
    template <typename T, typename Allocator = memory::persistent_global_tagged_allocator<T>>
    class unique_ptr 
    {
    public:
        // Make other instantiations of unique_ptr friends so we can access their private members.
        template <typename U, typename OtherAllocator>
        friend class unique_ptr;

    public:
        // Constructor: takes ownership of the pointer.
        explicit unique_ptr(T* p = nullptr) noexcept 
            : ptr(p) 
        {}

        // Overloaded constructor for std::nullptr_t (explicitly handles nullptr).
        unique_ptr(std::nullptr_t) noexcept 
            : ptr(nullptr) 
        {}

        // Destructor: explicitly calls the destructor of T, then deallocates memory.
        ~unique_ptr() 
        {
            if (ptr) 
            {
                // Call the destructor explicitly.
                ptr->~T();

                // Use a temporary allocator instance to deallocate memory.
                Allocator alloc;
                alloc.deallocate(ptr, 1);
            }
        }

        // Delete copy semantics.
        unique_ptr(const unique_ptr&) = delete;
        unique_ptr& operator=(const unique_ptr&) = delete;

        // Move constructor.
        unique_ptr(unique_ptr&& other) noexcept 
            : ptr(other.ptr) 
        {
            other.ptr = nullptr;
        }

        // Move assignment operator.
        unique_ptr& operator=(unique_ptr&& other) noexcept 
        {
            if (this != &other) 
            {
                reset(other.ptr);
                other.ptr = nullptr;
            }

            return *this;
        }

        // --- Polymorphic support ---
        // Conversion move constructor: allows moving from a unique_ptr<U, OtherAllocator>
        // to a unique_ptr<T, Allocator> if U* is convertible to T*.
        template <typename U, typename OtherAllocator, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
            unique_ptr(unique_ptr<U, OtherAllocator>&& other) noexcept
            : ptr(other.ptr) 
        {
            other.ptr = nullptr;
        }

        // Conversion move assignment operator.
        template <typename U, typename OtherAllocator, typename = std::enable_if_t<std::is_convertible<U*, T*>::value>>
            unique_ptr & operator=(unique_ptr<U, OtherAllocator>&& other) noexcept
        {
            // Avoid self-assignment (even across different instantiations, comparing the stored pointers suffices).
            if (ptr != other.ptr) 
            {
                reset();

                ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }
        // --- End Polymorphic support ---

        // Overloaded assignment operator for std::nullptr_t.
        unique_ptr& operator=(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        // Access the underlying pointer.
        T* get() const noexcept { return ptr; }

        // Overload dereference operators.
        T& operator*() const noexcept { return *ptr; }
        T* operator->() const noexcept { return ptr; }

        // Reset the pointer, deallocating the old memory.
        void reset(T* p = nullptr) 
        {
            if (ptr) 
            {
                ptr->~T();

                Allocator alloc;
                alloc.deallocate(ptr, 1);
            }

            ptr = p;
        }

    private:
        T* ptr;
    };
    //--------------------------------------------------------------


    //--------------------------------------------------------------
    // Factory function to create a unique_ptr.
    // It allocates memory using the free_list_allocator and constructs the object in-place.
    template <typename T, typename Allocator = memory::persistent_global_tagged_allocator<T>, typename... Args>
    unique_ptr<T, Allocator> make_unique(Args&&... args) 
    {
        Allocator alloc;
        T* raw_ptr = alloc.allocate(1);

        // Use placement new to construct T in the allocated memory.
        new (raw_ptr) T(std::forward<Args>(args)...);

        return unique_ptr<T, Allocator>(raw_ptr);
    }
    //--------------------------------------------------------------
}