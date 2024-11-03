#pragma once

#include "util/types.h"
#include "util/log.h"

#include <glm/glm.hpp>

#include <array>
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

        enum class vertex_attribute_type
        {
            POSITION,
            NORMAL,
            TEXCOORD,
            COLOR,
            DIFFUSE_TEXTURE_INDEX
        };

        enum class vertex_attribute_data_type
        {
            FLOAT,
            UNSIGNED_INT
        };

        s32 size_in_bytes_for_data_type(vertex_attribute_data_type type);
        s32 component_count_for_vertex_attribute(vertex_attribute_type type);

        struct vertex_attribute_layout
        {
            vertex_attribute_type type;             // type of the attribute, these are similar to the semantics in shaders (POSITION, NORMAL, TEXCOORD0)
            vertex_attribute_data_type data_type;   // data type of the attribute (FLOAT, UNSIGNED INT)

            s32 idx;        // attribute index 
            s32 count;      // attribute component count, for instance glm::vec3 component count == 3
            s32 normalized; // attribute values are normalized
            u64 stride;     // size the full vertex type ( so stride of a vertex with position (vec3) and color (vec4) would be (3 * 4) + (4 * 4) 
            u64 offset;     // offset in the vertex type ( so offset of color within a vertex with position (vec3) and color (vec4) would be 12

            u64 element_size_in_bytes() const { return size_in_bytes_for_data_type(data_type); }
            u64 total_size_in_bytes() const { return count * element_size_in_bytes(); }
        };

        using vertex_attribute_blob = std::vector<u8>;

        struct vertex_attribute
        {
            vertex_attribute_layout layout;
            vertex_attribute_blob blob;
        };

        using vertex_attribute_map = std::unordered_map<vertex_attribute_type, vertex_attribute>;

        class vertex_component : public render_item_component_impl<vertex_component>
        {
        public:
            vertex_component(u64 vertex_count);

            void add_attribute(vertex_attribute_type type, glm::vec2* data);
            void add_attribute(vertex_attribute_type type, glm::vec3* data);
            void add_attribute(vertex_attribute_type type, f32* data);
            void add_attribute(vertex_attribute_type type, glm::uvec2* data);
            void add_attribute(vertex_attribute_type type, glm::uvec3* data);
            void add_attribute(vertex_attribute_type type, u32* data);

            void set_attribute(vertex_attribute_type type, glm::vec2* data);
            void set_attribute(vertex_attribute_type type, glm::vec3* data);
            void set_attribute(vertex_attribute_type type, f32* data);
            void set_attribute(vertex_attribute_type type, glm::uvec2* data);
            void set_attribute(vertex_attribute_type type, glm::uvec3* data);
            void set_attribute(vertex_attribute_type type, u32* data);

            template<typename T>
            const T* get_attribute_data(vertex_attribute_type type) const;
            const vertex_attribute_map& get_attribute_data() const;

            u64 vertex_count() const;

        private:
            void add_attribute(vertex_attribute_type type, vertex_attribute_data_type data_type, void* data);
            void set_attribute(vertex_attribute_type type, vertex_attribute_data_type data_type, void* data);

            void evaluate_attributes();

            vertex_attribute_map m_attributes;
            u64 m_vertex_count;
        };

        //-------------------------------------------------------------------------
        template<typename T>
        const T* vertex_component::get_attribute_data(vertex_attribute_type type) const
        {
            auto it = m_attributes.find(type);
            if(it == std::cend(m_attributes))
            {
                return nullptr;
            }

            return reinterpret_cast<const T*>(it->second.blob.data());
        }

        class index_component : public render_item_component_impl<index_component>
        {
        public:
            index_component(u32* idxs, u64 count);

            void set_indices(u32* m_indices, u64 m_index_count);

            const u32* indices() const;

            u64 index_count() const;

        private:
            u32* m_indices;
            u64 m_index_count;
        };

        class texture_component : public render_item_component_impl<texture_component> 
        {
        public:
            texture_component(u32 id);

            u32 texture_id() const;

        private:
            u32 m_texture_id;
        };

        std::unique_ptr<vertex_component> make_vertex_component(u64 count);
        std::unique_ptr<index_component> make_index_component(u32* idxs, u64 count);
        std::unique_ptr<texture_component> make_texture_component(u32 id);

        class render_item
        {
        public:
            template <typename T>
            void add_component(std::unique_ptr<T>&& component) 
            {
                m_components[type_tag<T>::value()] = std::move(component);
            }

            template <typename T>
            T* get_component()
            {
                auto it = m_components.find(type_tag<T>::value());
                if (it != m_components.end())
                {
                    return static_cast<T*>(it->second.get());
                }
                return nullptr;
            }

            template <typename T>
            const T* get_component() const 
            {
                auto it = m_components.find(type_tag<T>::value());
                if (it != m_components.end()) 
                {
                    return static_cast<const T*>(it->second.get());
                }
                return nullptr;
            }

        private:
            std::unordered_map<object_type_tag, std::unique_ptr<render_item_component>> m_components;
        };

        class irender_item
        {
        public:
            virtual u64 vertex_count() const = 0;
            virtual u64 index_count() const = 0;

            virtual const std::vector<glm::vec3>& vertex_positions() const = 0;
            virtual const std::vector<glm::vec3>& vertex_normals() const = 0;
            virtual const std::vector<glm::vec2>& vertex_uvs() const = 0;
            virtual const std::vector<face>& faces() const = 0;
        };
    }
}