#pragma once

#include "render/render_base_renderer.h"
#include "render/render_instance.h"



namespace ppp
{
    namespace render
    {
        class instance_renderer : public base_renderer
        {
        public:
            static void set_wireframe_linewidth(f32 width);
            static void set_wireframe_linecolor(s32 color);

        public:
            instance_renderer(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag);
            ~instance_renderer() override;

        public:
            void begin() override;
            void render() override;
            void end() override;
            void terminate() override;

        public:
            void append_drawing_data(topology_type topology, const irender_item* item, const glm::vec4& color, const glm::mat4& world);

            bool has_drawing_data() const;

        protected:
            virtual void on_render(topology_type topology, instance_drawing_data& drawing_data) = 0;

        private:          
            void solid_render(topology_type topology, instance_drawing_data& drawing_data);
            void wireframe_render(topology_type topology, instance_drawing_data& drawing_data);

        private:
            using instance_data_map = std::unordered_map<topology_type, instance_drawing_data>;

            instance_data_map m_instance_data_map;

            const attribute_layout* m_instance_layouts;
            u64 m_instance_layout_count;

            std::vector<std::function<void(topology_type, instance_drawing_data&)>> m_render_fns;
        };

        class primitive_instance_renderer : public instance_renderer
        {
        public:
            primitive_instance_renderer(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag);
            ~primitive_instance_renderer() override;

            void on_render(topology_type topology, instance_drawing_data& drawing_data) override;

            bool has_texture_support() const { return false; }
        };

        class texture_instance_renderer : public instance_renderer
        {
        public:
            texture_instance_renderer(const attribute_layout* instance_layouts, u64 instance_layout_count, string::string_id shader_tag);
            ~texture_instance_renderer() override;

            void on_render(topology_type topology, instance_drawing_data& drawing_data) override;

            bool has_texture_support() const { return true; }
        };
    }
}