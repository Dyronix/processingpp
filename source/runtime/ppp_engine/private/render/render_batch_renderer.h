#pragma once

#include "render/render_batch.h"

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        class vertex_attribute_layout;
        class render_item;
        
        enum class batch_primitive_type
        {
            POINTS,
            LINES,
            TRIANGLES
        };

        class batch_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

            batch_renderer(batch_primitive_type primitive_type, s32 max_vertices, s32 max_indices, vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag);
            batch_renderer(batch_primitive_type primitive_type, s32 max_vertices, s32 max_indices, s32 max_textures, vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag);

            virtual ~batch_renderer();

            void begin();
            void render(const glm::mat4& vp);
            void end();
            void terminate();

            void append_drawing_data(const render_item& item, const glm::vec4& color, const glm::mat4& world);

            void enable_solid_rendering(bool enable);
            void enable_wireframe_rendering(bool enable);

            bool solid_rendering_supported() const;
            bool wireframe_rendering_supported() const;

            u32 shader_program() const;
            u32 primitive_type() const;

        protected:
            virtual void on_render(batch_drawing_data& drawing_data) = 0;

        private:
            static const s32 s_wireframe = 1 << 0;
            static const s32 s_solid = 1 << 1;

            static f32 s_wireframe_linewidth;
            static s32 s_wireframe_linecolor;

            std::string_view m_shader_tag;
            s32 m_rasterization_mode;
            batch_drawing_data m_drawing_data;
            u32 m_primitive_type;
        };

        class primitive_batch_renderer : public batch_renderer
        {
        public:
            primitive_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag, s32 max_vertices, s32 max_indices, batch_primitive_type primitive_type = batch_primitive_type::TRIANGLES);
            ~primitive_batch_renderer() override;

            void on_render(batch_drawing_data& drawing_data) override;
        };

        class texture_batch_renderer : public batch_renderer
        {
        public:
            texture_batch_renderer(vertex_attribute_layout* layouts, u64 layout_cout, std::string_view shader_tag, s32 max_vertices, s32 max_indices, s32 max_textures, batch_primitive_type primitive_type = batch_primitive_type::TRIANGLES);
            ~texture_batch_renderer() override;

            void on_render(batch_drawing_data& drawing_data) override;
        };
    }
}