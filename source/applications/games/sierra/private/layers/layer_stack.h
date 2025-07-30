#pragma once

#include "string/string_id.h"

#include "util/types.h"

#include <vector>
#include <memory>

namespace ppp
{
    class layer;

    class layer_stack
    {
    public:
        using layer_stack_it = std::vector<std::unique_ptr<layer>>::iterator;
        using const_layer_stack_it = std::vector<std::unique_ptr<layer>>::const_iterator;
        using reverse_layer_stack_it = std::vector<std::unique_ptr<layer>>::reverse_iterator;
        using const_reverse_layer_stack_it = std::vector<std::unique_ptr<layer>>::const_reverse_iterator;

        explicit layer_stack(s32 initial_size = -1);
        ~layer_stack() noexcept;

        //
        // stack iterator
        //
        layer_stack_it begin();
        const_layer_stack_it begin() const;
        const_layer_stack_it cbegin() const;
        layer_stack_it end();
        const_layer_stack_it end() const;
        const_layer_stack_it cend() const;

        reverse_layer_stack_it rbegin();
        const_reverse_layer_stack_it rbegin() const;
        const_reverse_layer_stack_it crbegin() const;
        reverse_layer_stack_it rend();
        const_reverse_layer_stack_it rend() const;
        const_reverse_layer_stack_it crend() const;

        //
        // stack functionality
        //
        void push(std::unique_ptr<layer> layer);

        void remove_layer(const string::string_id& layer_name);
        void remove_layer(layer* layer);

        void clear();

    private:
        std::vector<std::unique_ptr<layer>> m_layers;
    };
}