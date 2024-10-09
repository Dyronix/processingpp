#pragma once

#include "util/types.h"
#include "util/log.h"

#include <vector>
#include <unordered_map>
#include <memory>

namespace ppp
{
    namespace render
    {
        class render_item_component 
        {
        public:
            virtual ~render_item_component() = default;
            virtual object_type_tag get_type_tag() const = 0;
        };

        template <typename T>
        class render_item_component_impl : public render_item_component 
        {
        public:
            object_type_tag get_type_tag() const override 
            {
                return type_tag<T>::value();
            }
        };

        template<typename T>
        class vertex_component : public render_item_component_impl<vertex_component<T>> 
        {
        public:
            vertex_component(T* verts, u64 count)
                : m_vertices(verts)
                , m_vertex_count(count) 
            {}

            const T* vertices() const { return m_vertices; }
            u64 vertex_count() const { return m_vertex_count; }

        private:
            T* m_vertices;
            u64 m_vertex_count;
        };

        class index_component : public render_item_component_impl<index_component>
        {
        public:
            index_component(u32* idxs, u64 count)
                : m_indices(idxs)
                , m_index_count(count)
            {}

            const u32* indices() const { return m_indices; }
            u64 index_count() const { return m_index_count; }

        private:
            u32* m_indices;
            u64 m_index_count;
        };

        class texture_component : public render_item_component_impl<texture_component> {
        public:
            texture_component(u32 id) 
                : m_texture_id(id)
            {}

            u32 texture_id() const { return m_texture_id; }

        private:
            u32 m_texture_id;
        };

        template<typename T>
        inline std::unique_ptr<vertex_component<T>> make_vertex_component(T* verts, u64 count)
        {
            return std::make_unique<vertex_component<T>>(verts, count);
        }

        inline std::unique_ptr<index_component> make_index_component(u32* idxs, u64 count)
        {
            return std::make_unique<index_component>(idxs, count);
        }

        inline std::unique_ptr<texture_component> make_texture_component(u32 id)
        {
            return std::make_unique<texture_component>(id);
        }

        class render_item
        {
        public:
            render_item()
            {

            }

            template <typename T>
            void add_component(std::unique_ptr<T>&& component) 
            {
                components[type_tag<T>::value()] = std::move(component);
            }

            template <typename T>
            const T* get_component() const 
            {
                auto it = components.find(type_tag<T>::value());
                if (it != components.end()) 
                {
                    return static_cast<const T*>(it->second.get());
                }
                return nullptr;
            }

        private:
            std::unordered_map<object_type_tag, std::unique_ptr<render_item_component>> components;
        };
    }
}