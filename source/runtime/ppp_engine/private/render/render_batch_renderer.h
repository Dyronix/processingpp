#pragma once

#include "render/render_base_renderer.h"
#include "render/render_batch.h"

namespace ppp
{
    namespace render
    {
        class attribute_layout;
        
        class batch_renderer : public base_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

        public:
            batch_renderer(const attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag, bool enable_texture_support = false);
            ~batch_renderer() override;

        public:
            void begin() override;
            void render(const glm::mat4& vp) override;
            void end() override;
            void terminate() override;

        public:
            void append_drawing_data(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            void buffer_policy(render_buffer_policy policy);
            void draw_policy(render_draw_policy policy);

        public:
            bool has_drawing_data() const;

            render_buffer_policy buffer_policy() const;
            render_draw_policy draw_policy() const;

        protected:
            virtual void on_render(topology_type type, batch_drawing_data& drawing_data) = 0;

        private:
            void solid_render(topology_type topology, batch_drawing_data& drawing_data);
            void wireframe_render(topology_type topology, batch_drawing_data& drawing_data);

        private:
            using drawing_data_map = std::unordered_map<topology_type, batch_drawing_data>;

            drawing_data_map m_drawing_data_map;

            render_buffer_policy m_buffer_policy;
            render_draw_policy m_render_policy;
            
            std::vector<std::function<void(topology_type, batch_drawing_data&)>> m_render_fns;
        };

        class primitive_batch_renderer : public batch_renderer
        {
        public:
            primitive_batch_renderer(attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag);
            ~primitive_batch_renderer() override;

            void on_render(topology_type topology, batch_drawing_data& drawing_data) override;
        };

        class texture_batch_renderer : public batch_renderer
        {
        public:
            texture_batch_renderer(attribute_layout* layouts, u64 layout_cout, const std::string& shader_tag);
            ~texture_batch_renderer() override;

            void on_render(topology_type topology, batch_drawing_data& drawing_data) override;
        };
    }
}