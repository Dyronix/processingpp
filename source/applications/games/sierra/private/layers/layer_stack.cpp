#include "layers/layer_stack.h"
#include "layers/layer.h"

#include <algorithm>
#include <cassert>

namespace ppp
{
    constexpr u64 initial_layerstack_capacity = 5;

    class layer_guard 
    {
    public:
        //-------------------------------------------------------------------------
        explicit layer_guard(layer& l) noexcept 
            : m_layer(l)
            , m_attached(true) 
        {
            m_layer.attached();
        }

        //-------------------------------------------------------------------------
        ~layer_guard() noexcept 
        {
            if (m_attached)
            {
                m_layer.detached();
            }
        }

        //-------------------------------------------------------------------------
        void commit() noexcept { m_attached = false; }

    private:
        layer& m_layer;
        bool m_attached;
    };


    //-------------------------------------------------------------------------
    layer_stack::layer_stack(s32 initial_size)
    {
        s32 size = initial_size == -1 ? initial_layerstack_capacity : initial_size;

        if (initial_size > 0)
        {
            m_layers.reserve(size);
        }
    }
    //-------------------------------------------------------------------------
    layer_stack::~layer_stack() noexcept
    {
        clear();
    }

    //-------------------------------------------------------------------------
    layer_stack::layer_stack_it layer_stack::begin()
    {
        return m_layers.begin();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_layer_stack_it layer_stack::begin() const
    {
        return m_layers.begin();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_layer_stack_it layer_stack::cbegin() const
    {
        return begin();
    }
    //-------------------------------------------------------------------------
    layer_stack::layer_stack_it layer_stack::end()
    {
        return m_layers.end();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_layer_stack_it layer_stack::end() const
    {
        return m_layers.end();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_layer_stack_it layer_stack::cend() const
    {
        return end();
    }

    //-------------------------------------------------------------------------
    layer_stack::reverse_layer_stack_it layer_stack::rbegin()
    {
        return m_layers.rbegin();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_reverse_layer_stack_it layer_stack::rbegin() const
    {
        return m_layers.rbegin();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_reverse_layer_stack_it layer_stack::crbegin() const
    {
        return rbegin();
    }
    //-------------------------------------------------------------------------
    layer_stack::reverse_layer_stack_it layer_stack::rend()
    {
        return m_layers.rend();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_reverse_layer_stack_it layer_stack::rend() const
    {
        return crend();
    }
    //-------------------------------------------------------------------------
    layer_stack::const_reverse_layer_stack_it layer_stack::crend() const
    {
        return m_layers.crend();
    }

    //-------------------------------------------------------------------------
    void layer_stack::push(std::unique_ptr<layer> l)
    {
        layer_guard guard(*l);

        m_layers.push_back(std::move(l));

        std::sort(std::begin(m_layers), std::end(m_layers), [](const std::unique_ptr<layer>& layer1, const std::unique_ptr<layer>& layer2)
                  {
                      return layer1->order() < layer2->order();
                  });

        guard.commit();
    }

    //-------------------------------------------------------------------------
    void layer_stack::remove_layer(const string::string_id& layername)
    {
        auto it = std::find_if(std::cbegin(m_layers), std::cend(m_layers), 
            [layername](const std::unique_ptr<layer>& layer)
        {
            return layername == layer->name();
        });

        if (it == std::cend(m_layers))
            return;

        (*it)->detached();
        m_layers.erase(it);
    }
    //-------------------------------------------------------------------------
    void layer_stack::remove_layer(layer* in_layer)
    {       
        auto it = std::find_if(std::cbegin(m_layers), std::cend(m_layers), 
            [in_layer](const std::unique_ptr<layer>& l)
        {
            return in_layer == l.get();
        });

        if (it == std::cend(m_layers))
            return;

        (*it)->detached();
        m_layers.erase(it);
    }

    //-------------------------------------------------------------------------
    void layer_stack::clear()
    {       
        for (auto& l : m_layers)
        {
            l->detached();
        }

        m_layers.clear();
    }
}