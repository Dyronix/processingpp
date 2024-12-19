#pragma once

#include "render/render_item_components.h"
#include "render/render_vertex_buffer.h"
#include "render/render_index_buffer.h"
#include "render/render_types.h"

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <unordered_map>

namespace ppp
{
    namespace render
    {
        class instance
        {
        public:
            instance(const irender_item* instance, const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count, s32 size_textures = -1);
            ~instance();

            instance(const instance& other) = delete;             // unique ptr, so we can delete
            instance(instance&& other) noexcept;                           // has to be defined, due to auto compiler generated function and forward delclaration

            instance& operator=(const instance& other) = delete;  // unique ptr, so we can delete
            instance& operator=(instance&& other) noexcept;                // has to be defined, due to auto compiler generated function and forward delclaration

        public:
            void bind() const;
            void unbind() const;
            void submit() const;
            void draw(topology_type topology, u32 shader_program) const;

        public:
            void increment_instance_count();

            void append(const std::vector<texture_id>& texture_ids, const glm::vec4& color, const glm::mat4& world);
            void reset();
            void release();

            bool has_data() const;
            bool has_reserved_texture_space() const;

        public:
            u64 instance_id() const;

            const void* vertices() const;
            const void* indices() const;
            const s32* samplers() const;
            const u32* textures() const;

            u32 active_vertex_count() const;
            u32 active_index_count() const;
            u32 active_sampler_count() const;
            u32 active_texture_count() const;

            u64 vertex_buffer_byte_size() const;
            u64 index_buffer_byte_size() const;

        private:
            class impl;
            std::unique_ptr<impl> m_pimpl;
        };

        class instance_drawing_data
        {
        public:
            instance_drawing_data(const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count, render_buffer_policy render_buffer_policy);
            instance_drawing_data(s32 max_textures, const attribute_layout* layouts, u64 layout_count, const attribute_layout* instance_layouts, u64 instance_layout_count, render_buffer_policy render_buffer_policy);
            ~instance_drawing_data();

            instance_drawing_data(const instance_drawing_data& other) = delete;             // unique ptr, so we can delete
            instance_drawing_data(instance_drawing_data&& other) noexcept;                           // has to be defined, due to auto compiler generated function and forward delclaration

            instance_drawing_data& operator=(const instance_drawing_data& other) = delete;  // unique ptr, so we can delete
            instance_drawing_data& operator=(instance_drawing_data&& other) noexcept;                // has to be defined, due to auto compiler generated function and forward delclaration

        public:
            void append(const irender_item* item, const glm::vec4& color, const glm::mat4& world);
            void reset();
            void release();

            const instance* first_instance();
            const instance* next_instance();

            bool has_drawing_data() const;

        private:
            using instance_map = std::vector<instance>;

        private:
            struct impl;
            std::unique_ptr<impl> m_pimpl;
        };
    }
}