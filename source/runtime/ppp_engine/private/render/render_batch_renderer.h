#pragma once

#include "render/render_batch.h"
#include "render/render_types.h"

#include "util/types.h"

#include <string>

namespace ppp
{
    namespace render
    {
        class vertex_attribute_layout;
        class render_item;
        
        class batch_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

            batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag, bool enable_texture_support = false);

            virtual ~batch_renderer();

            void begin();
            void render(const glm::mat4& vp);
            void end();
            void terminate();

            void append_drawing_data(topology_type topology, const render_item& item, const glm::vec4& color, const glm::mat4& world);

            void enable_solid_rendering(bool enable);
            void enable_wireframe_rendering(bool enable);

            void buffer_policy(batch_buffer_policy policy);
            void render_policy(batch_render_policy policy);

            bool solid_rendering_supported() const;
            bool wireframe_rendering_supported() const;
            bool has_drawing_data() const;

            u32 shader_program() const;

            batch_buffer_policy buffer_policy() const;
            batch_render_policy render_policy() const;

        protected:
            virtual void on_render(topology_type type, batch_drawing_data& drawing_data) = 0;

        private:
            using drawing_data_map = std::unordered_map<topology_type, batch_drawing_data>;

            std::string m_shader_tag;
            drawing_data_map m_drawing_data_map;
            s32 m_rasterization_mode;
            vertex_attribute_layout* m_layouts;
            u64 m_layout_count;
            bool m_texture_support;
            batch_buffer_policy m_batch_buffer_policy;
            batch_render_policy m_batch_render_policy;
        };

        class primitive_batch_renderer : public batch_renderer
        {
        public:
            primitive_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag);
            ~primitive_batch_renderer() override;

            void on_render(topology_type topology, batch_drawing_data& drawing_data) override;
        };

        class texture_batch_renderer : public batch_renderer
        {
        public:
            texture_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag);
            ~texture_batch_renderer() override;

            void on_render(topology_type topology, batch_drawing_data& drawing_data) override;
        };
    }
}