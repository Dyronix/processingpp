#pragma once

#include "render/render_instance.h"
#include "render/render_types.h"

#include "util/types.h"

#include <string>
#include <functional>

namespace ppp
{
    namespace render
    {
        class instance_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

            instance_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag, bool enable_texture_support = false);

            virtual ~instance_renderer();

            void begin();
            void render(const glm::mat4& vp);
            void end();
            void terminate();

            void append_instance(topology_type topology, const irender_item* instance);
            void append_drawing_data(u64 geometry_id, const glm::vec4& color, const glm::mat4& world);

            void enable_solid_rendering(bool enable);
            void enable_wireframe_rendering(bool enable);

            void buffer_policy(render_buffer_policy policy);
            void draw_policy(render_draw_policy policy);
            void user_shader_program(const std::string& tag);
            void reset_user_shader_program();

            bool solid_rendering_supported() const;
            bool wireframe_rendering_supported() const;
            
            bool has_instance_data() const;
            bool has_drawing_data() const;

            u32 shader_program() const;

            render_buffer_policy buffer_policy() const;
            render_draw_policy draw_policy() const;

        protected:
            virtual void on_render(instance_drawing_data& drawing_data) = 0;

        private:
            void solid_render(instance_drawing_data& drawing_data);
            void wireframe_render(instance_drawing_data& drawing_data);

        private:
            using instance_data_map = std::unordered_map<u64, instance_drawing_data>;

            std::string m_user_shader_tag;
            std::string m_shader_tag;
            instance_data_map m_instance_data_map;
            s32 m_rasterization_mode;
            vertex_attribute_layout* m_layouts;
            u64 m_layout_count;
            bool m_texture_support;
            render_buffer_policy m_buffer_policy;
            render_draw_policy m_render_policy;
            std::vector<std::function<void(instance_drawing_data&)>> m_render_fns;
        };

        class primitive_instance_renderer : public instance_renderer
        {
        public:
            primitive_instance_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag);
            ~primitive_instance_renderer() override;

            void on_render(instance_drawing_data& drawing_data) override;
        };

        class texture_instance_renderer : public instance_renderer
        {
        public:
            texture_instance_renderer(vertex_attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag);
            ~texture_instance_renderer() override;

            void on_render(instance_drawing_data& drawing_data) override;
        };
    }
}