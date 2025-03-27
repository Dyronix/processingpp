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

            static f32 wireframe_linewidth();
            static s32 wireframe_linecolor();

        public:
            batch_renderer(string::string_id shader_tag);
            ~batch_renderer() override;

        public:
            void begin() override;
            void render() override;
            void end() override;
            void terminate() override;

        public:
            void append_drawing_data(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            bool has_drawing_data() const;

        protected:
            virtual void on_render(topology_type type, batch_drawing_data& drawing_data) = 0;

        private:
            using drawing_data_map = std::unordered_map<topology_type, batch_drawing_data>;

            drawing_data_map m_drawing_data_map;
        };

        class primitive_batch_renderer : public batch_renderer
        {
        public:
            primitive_batch_renderer(string::string_id shader_tag);
            ~primitive_batch_renderer() override;

            void on_render(topology_type topology, batch_drawing_data& drawing_data) override;

            bool has_texture_support() const override { return false; }
        };

        class texture_batch_renderer : public batch_renderer
        {
        public:
            texture_batch_renderer(string::string_id shader_tag);
            ~texture_batch_renderer() override;

            void on_render(topology_type topology, batch_drawing_data& drawing_data) override;

            bool has_texture_support() const override { return true; }
        };
    }
}