#include "render/render.h"
#include "render/render_transform.h"
#include "render/render_batch.h"
#include "render/render_instance.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"
#include "render/render_brush.h"

#include "render/opengl/render_gl_error.h"

#include "resources/shader_pool.h"

#include "util/log.h"
#include "util/color_ops.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <vector>
#include <array>
#include <functional>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            //-------------------------------------------------------------------------
            render_draw_mode _draw_mode = render_draw_mode::BATCHED;

            //-------------------------------------------------------------------------
            std::vector<std::function<void()>> _draw_begin_subs;
            std::vector<std::function<void()>> _draw_end_subs;

            //-------------------------------------------------------------------------
            struct camera
            {
                glm::vec3 eye;
                glm::vec3 center;
                glm::vec3 up;

                glm::mat4 proj;
            };

            camera _active_camera = {};
            camera _image_camera = {};
            camera _font_camera = {};

            //-------------------------------------------------------------------------
            s32 _scissor_x = -1;
            s32 _scissor_y = -1;
            s32 _scissor_width = -1;
            s32 _scissor_height = -1;
            bool _scissor_enable = false;

            //-------------------------------------------------------------------------
            struct pos_col_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };

            //-------------------------------------------------------------------------
            struct pos_tex_col_format
            {
                glm::vec3 position;
                glm::vec2 texcoord;
                glm::vec4 color;
                f32       texture_idx;
            };

            //-------------------------------------------------------------------------
            struct pos_tex_col_norm_format
            {
                glm::vec3 position;
                glm::vec2 texcoord;
                glm::vec3 normal;
                glm::vec4 color;
                f32       texture_idx;
            };

            //-------------------------------------------------------------------------
            std::array<vertex_attribute_layout, 2> _pos_col_layout =
            {
                vertex_attribute_layout{
                    vertex_attribute_type::POSITION,
                    vertex_attribute_data_type::FLOAT,

                    0,
                    3,
                    false,
                    sizeof(pos_col_format),
                    0
                },
                vertex_attribute_layout{
                    vertex_attribute_type::COLOR,
                    vertex_attribute_data_type::FLOAT,

                    1,
                    4,
                    false,
                    sizeof(pos_col_format),
                    3 * sizeof(float)
                }
            };
            std::array<vertex_attribute_layout, 4> _pos_tex_col_layout
            {
                vertex_attribute_layout{
                    vertex_attribute_type::POSITION,
                    vertex_attribute_data_type::FLOAT,

                    0,
                    3,
                    false,
                    sizeof(pos_tex_col_format),
                    0
                },
                vertex_attribute_layout{
                    vertex_attribute_type::TEXCOORD,
                    vertex_attribute_data_type::FLOAT,

                    1,
                    2,
                    false,
                    sizeof(pos_tex_col_format),
                    3 * sizeof(float)
                },
                vertex_attribute_layout{
                    vertex_attribute_type::COLOR,
                    vertex_attribute_data_type::FLOAT,

                    2,
                    4,
                    false,
                    sizeof(pos_tex_col_format),
                    3 * sizeof(float) + 2 * sizeof(float)
                },
                vertex_attribute_layout{
                    vertex_attribute_type::DIFFUSE_TEXTURE_INDEX,
                    vertex_attribute_data_type::FLOAT,

                    3,
                    1,
                    false,
                    sizeof(pos_tex_col_format),
                    3 * sizeof(float) + 2 * sizeof(float) + 4 * sizeof(float)
                }
            };
            std::array<vertex_attribute_layout, 5> _pos_tex_col_norm_layout
            {
                vertex_attribute_layout{
                    vertex_attribute_type::POSITION,
                    vertex_attribute_data_type::FLOAT,

                    0,
                    3,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    0
                },
                vertex_attribute_layout{
                    vertex_attribute_type::TEXCOORD,
                    vertex_attribute_data_type::FLOAT,

                    1,
                    2,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float)
                },
                vertex_attribute_layout{
                    vertex_attribute_type::NORMAL,
                    vertex_attribute_data_type::FLOAT,

                    2,
                    3,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float) + 2 * sizeof(float)
                },
                vertex_attribute_layout{
                    vertex_attribute_type::COLOR,
                    vertex_attribute_data_type::FLOAT,

                    3,
                    4,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float)
                },
                vertex_attribute_layout{
                    vertex_attribute_type::DIFFUSE_TEXTURE_INDEX,
                    vertex_attribute_data_type::FLOAT,

                    4,
                    1,
                    false,
                    sizeof(pos_tex_col_norm_format),
                    3 * sizeof(float) + 3 * sizeof(float) + 2 * sizeof(float) + 4 * sizeof(float)
                }
            };

            //-------------------------------------------------------------------------
            constexpr s32 _min_frame_buffer_width = 32;
            constexpr s32 _min_frame_buffer_height = 32;

            //-------------------------------------------------------------------------
            s32 _frame_buffer_width = -1;
            s32 _frame_buffer_height = -1;

            //-------------------------------------------------------------------------
            u32 _render_fbo;
            u32 _render_depth_rbo;
            u32 _render_texture;

            //-------------------------------------------------------------------------
            std::string _fill_user_shader = {};
            std::string _stroke_user_shader = {};

            vertex_type _fill_user_vertex_type = vertex_type::POSITION_TEXCOORD_NORMAL_COLOR;

            vertex_attribute_layout* fill_user_layout(vertex_type type)
            {
                switch (type)
                {
                case vertex_type::POSITION_COLOR: return _pos_col_layout.data();
                case vertex_type::POSITION_TEXCOORD_COLOR: return _pos_tex_col_layout.data();
                case vertex_type::POSITION_TEXCOORD_NORMAL_COLOR: return _pos_tex_col_norm_layout.data();
                }

                log::error("Unsupported vertex type");
                return nullptr;
            }

            u64 fill_user_layout_count(vertex_type type)
            {
                switch (type)
                {
                case vertex_type::POSITION_COLOR: return _pos_col_layout.size();
                case vertex_type::POSITION_TEXCOORD_COLOR: return _pos_tex_col_layout.size();
                case vertex_type::POSITION_TEXCOORD_NORMAL_COLOR: return _pos_tex_col_norm_layout.size();
                }

                log::error("Unsupported vertex type");
                return 0;
            }

            //-------------------------------------------------------------------------
            void create_frame_buffer()
            {
                GL_CALL(glGenFramebuffers(1, &_render_fbo));
                GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _render_fbo));

                GL_CALL(glGenTextures(1, &_render_texture));
                GL_CALL(glBindTexture(GL_TEXTURE_2D, _render_texture));

                GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _frame_buffer_width, _frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
                GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
                GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
                GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture, 0));

                GL_CALL(glGenRenderbuffers(1, &_render_depth_rbo));
                GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, _render_depth_rbo));
                GL_CALL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _frame_buffer_width, _frame_buffer_height));
                GL_CALL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _render_depth_rbo));

                u32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
                GL_CALL(glDrawBuffers(1, attachments));

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    log::error("FramebufferStatus != FRAMEBUFFER_COMPLETE");
                    return;
                }

                GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
            }

            //-------------------------------------------------------------------------
            std::unique_ptr<instance_renderer> _primitive_instance_renderer;

            //-------------------------------------------------------------------------
            std::unique_ptr<batch_renderer> _primitive_batch_renderer;
            std::unique_ptr<batch_renderer> _primitive_stroke_batch_renderer;
            std::unique_ptr<batch_renderer> _image_batch_renderer;
            std::unique_ptr<batch_renderer> _image_stroke_batch_renderer;
            std::unique_ptr<batch_renderer> _font_batch_renderer;

            std::unordered_map<std::string, std::unique_ptr<batch_renderer>> _custom_geometry_batch_renderers;

            //-------------------------------------------------------------------------
            class geometry_builder
            {
            public:
                void activate(const std::string& tag)
                {
                    m_is_active = true;
                    m_shader_tag = tag;
                }

                void deactivate()
                {
                    m_is_active = false;
                    m_shader_tag.clear();
                }

                bool is_active() const
                {
                    return m_is_active;
                }

                const std::string& shader_tag() const
                {
                    return m_shader_tag;
                }

            private:
                bool m_is_active = false;
                std::string m_shader_tag;
            };

            geometry_builder _geometry_builder;

            //-------------------------------------------------------------------------
            void submit_custom_render_item(topology_type topology, const render_item& item)
            {
                if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false && brush::fill_enabled() == false)
                {
                    // When there is no "stroke" and there is no "fill" the object would be invisible.
                    // So we don't add anything to the drawing list.
                    return;
                }

                const std::string& shader_tag = _fill_user_shader.empty() ? _geometry_builder.shader_tag() : _fill_user_shader;

                if (internal::_custom_geometry_batch_renderers.find(shader_tag) == std::cend(internal::_custom_geometry_batch_renderers))
                {
                    auto renderer = std::make_unique<primitive_batch_renderer>(
                        _fill_user_shader.empty() ? internal::_pos_col_layout.data() : fill_user_layout(internal::_fill_user_vertex_type),
                        _fill_user_shader.empty() ? internal::_pos_col_layout.size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                        shader_tag);

                    if (_geometry_builder.is_active())
                    {
                        renderer->buffer_policy(render_buffer_policy::RETAINED);
                    }

                    renderer->draw_policy(render_draw_policy::CUSTOM);
                    internal::_custom_geometry_batch_renderers.emplace(shader_tag, std::move(renderer));
                }

                internal::_custom_geometry_batch_renderers.at(shader_tag)->append_drawing_data(topology, item, brush::fill(), transform::active_world());
            }
            //-------------------------------------------------------------------------
            void submit_custom_render_item(topology_type topology, const irender_item* item)
            {
                if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false && brush::fill_enabled() == false)
                {
                    // When there is no "stroke" and there is no "fill" the object would be invisible.
                    // So we don't add anything to the drawing list.
                    return;
                }

                const std::string& shader_tag = _fill_user_shader.empty() ? _geometry_builder.shader_tag() : _fill_user_shader;

                if (internal::_custom_geometry_batch_renderers.find(shader_tag) == std::cend(internal::_custom_geometry_batch_renderers))
                {
                    auto renderer = std::make_unique<primitive_batch_renderer>(
                        _fill_user_shader.empty() ? internal::_pos_col_layout.data() : fill_user_layout(internal::_fill_user_vertex_type),
                        _fill_user_shader.empty() ? internal::_pos_col_layout.size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                        shader_tag);

                    if (_geometry_builder.is_active())
                    {
                        renderer->buffer_policy(render_buffer_policy::RETAINED);
                    }

                    renderer->draw_policy(render_draw_policy::CUSTOM);
                    internal::_custom_geometry_batch_renderers.emplace(shader_tag, std::move(renderer));
                }

                internal::_custom_geometry_batch_renderers.at(shader_tag)->append_drawing_data(topology, item, brush::fill(), transform::active_world());
            }
            //-------------------------------------------------------------------------
            void submit_custom_image_item(const render_item& item)
            {
                const std::string& shader_tag = _fill_user_shader.empty() ? _geometry_builder.shader_tag() : _fill_user_shader;

                if (internal::_custom_geometry_batch_renderers.find(shader_tag) == std::cend(internal::_custom_geometry_batch_renderers))
                {
                    auto renderer = std::make_unique<texture_batch_renderer>(
                        _fill_user_shader.empty() ? internal::_pos_tex_col_layout.data() : fill_user_layout(internal::_fill_user_vertex_type),
                        _fill_user_shader.empty() ? internal::_pos_tex_col_layout.size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                        shader_tag);

                    if (_geometry_builder.is_active())
                    {
                        renderer->buffer_policy(render_buffer_policy::RETAINED);
                    }

                    renderer->draw_policy(render_draw_policy::CUSTOM);
                    internal::_custom_geometry_batch_renderers.emplace(shader_tag, std::move(renderer));
                }

                internal::_custom_geometry_batch_renderers.at(shader_tag)->append_drawing_data(topology_type::TRIANGLES, item, brush::tint(), transform::active_world());
            }
            //-------------------------------------------------------------------------
            void submit_custom_image_item(const irender_item* item)
            {
                const std::string& shader_tag = _fill_user_shader.empty() ? _geometry_builder.shader_tag() : _fill_user_shader;

                if (internal::_custom_geometry_batch_renderers.find(shader_tag) == std::cend(internal::_custom_geometry_batch_renderers))
                {
                    auto renderer = std::make_unique<texture_batch_renderer>(
                        _fill_user_shader.empty() ? internal::_pos_tex_col_layout.data() : fill_user_layout(internal::_fill_user_vertex_type),
                        _fill_user_shader.empty() ? internal::_pos_tex_col_layout.size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                        shader_tag);

                    if (_geometry_builder.is_active())
                    {
                        renderer->buffer_policy(render_buffer_policy::RETAINED);
                    }

                    renderer->draw_policy(render_draw_policy::CUSTOM);
                    internal::_custom_geometry_batch_renderers.emplace(shader_tag, std::move(renderer));
                }

                internal::_custom_geometry_batch_renderers.at(shader_tag)->append_drawing_data(topology_type::TRIANGLES, item, brush::tint(), transform::active_world());
            }
        }

        //-------------------------------------------------------------------------
        void draw_mode(render_draw_mode mode)
        {
            internal::_draw_mode = mode;
        }

        //-------------------------------------------------------------------------
        bool initialize(s32 w, s32 h, void* user_data)
        {
            // glad: load all OpenGL function pointers
            // ---------------------------------------
            if (!gladLoadGLLoader((GLADloadproc)user_data))
            {
                log::error("Failed to initialize GLAD");
                return false;
            }

            internal::_frame_buffer_width = w;
            internal::_frame_buffer_height = h;

            internal::_scissor_x = 0;
            internal::_scissor_y = 0;
            internal::_scissor_width = w;
            internal::_scissor_height = h;
            internal::_scissor_enable = false;

            internal::_active_camera.eye = glm::vec3(0.0f, 0.0f, 10.0f);
            internal::_active_camera.center = glm::vec3(0.0f, 0.0f, 0.0f);
            internal::_active_camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
            internal::_active_camera.proj = glm::ortho(0.0f, (f32)w, 0.0f, (f32)h, -100.0f, 100.0f);

            internal::_image_camera.eye = glm::vec3(0.0f, 0.0f, 10.0f);
            internal::_image_camera.center = glm::vec3(0.0f, 0.0f, 0.0f);
            internal::_image_camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
            internal::_image_camera.proj = glm::ortho(0.0f, (f32)w, 0.0f, (f32)h, -100.0f, 100.0f);

            internal::_font_camera.eye = glm::vec3(0.0f, 0.0f, 10.0f);
            internal::_font_camera.center = glm::vec3(0.0f, 0.0f, 0.0f);
            internal::_font_camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
            internal::_font_camera.proj = glm::ortho(0.0f, (f32)w, 0.0f, (f32)h, -100.0f, 100.0f);

            internal::create_frame_buffer();

            internal::_primitive_instance_renderer = std::make_unique<primitive_instance_renderer>(internal::_pos_col_layout.data(), internal::_pos_col_layout.size(), shader_pool::tags::unlit_color);

            internal::_primitive_batch_renderer = std::make_unique<primitive_batch_renderer>(internal::_pos_col_layout.data(), internal::_pos_col_layout.size(), shader_pool::tags::unlit_color);
            internal::_primitive_stroke_batch_renderer = std::make_unique<primitive_batch_renderer>(internal::_pos_col_layout.data(), internal::_pos_col_layout.size(), shader_pool::tags::unlit_color);
            internal::_image_batch_renderer = std::make_unique<texture_batch_renderer>(internal::_pos_tex_col_layout.data(), internal::_pos_tex_col_layout.size(), shader_pool::tags::unlit_texture);
            internal::_image_stroke_batch_renderer = std::make_unique<primitive_batch_renderer>(internal::_pos_col_layout.data(), internal::_pos_col_layout.size(), shader_pool::tags::unlit_color);
            internal::_font_batch_renderer = std::make_unique<texture_batch_renderer>(internal::_pos_tex_col_layout.data(), internal::_pos_tex_col_layout.size(), shader_pool::tags::unlit_font);

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            internal::_primitive_instance_renderer->terminate();

            internal::_font_batch_renderer->terminate();
            internal::_image_batch_renderer->terminate();
            internal::_image_stroke_batch_renderer->terminate();
            internal::_primitive_batch_renderer->terminate();
            internal::_primitive_stroke_batch_renderer->terminate();

            for (auto& pair : internal::_custom_geometry_batch_renderers)
            {
                pair.second->terminate();
            }
        }

        //-------------------------------------------------------------------------
        void begin()
        {
            internal::_primitive_instance_renderer->begin();

            internal::_primitive_batch_renderer->begin();
            internal::_primitive_stroke_batch_renderer->begin();
            internal::_image_batch_renderer->begin();
            internal::_image_stroke_batch_renderer->begin();
            internal::_font_batch_renderer->begin();

            for (auto& pair : internal::_custom_geometry_batch_renderers)
            {
                pair.second->begin();
            }

            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, internal::_render_fbo));

            // Clear the background to black
            GL_CALL(glViewport(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height));

            GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            GL_CALL(glClearDepth(1.0));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            // Reset to the user clear color
            glm::vec4 bg_color = brush::background();
            GL_CALL(glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a));

            if (internal::_scissor_enable)
            {
                GL_CALL(glEnable(GL_SCISSOR_TEST));
                GL_CALL(glScissor(
                    std::clamp(internal::_scissor_x, 0, internal::_frame_buffer_width),
                    std::clamp(internal::_scissor_y, 0, internal::_frame_buffer_height),
                    std::clamp(internal::_scissor_width, internal::_min_frame_buffer_width, internal::_frame_buffer_width),
                    std::clamp(internal::_scissor_height, internal::_min_frame_buffer_height, internal::_frame_buffer_height)));
            }
            else
            {
                GL_CALL(glDisable(GL_SCISSOR_TEST));
            }

            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            if (internal::_draw_begin_subs.empty() == false)
            {
                for (auto& fn : internal::_draw_begin_subs)
                {
                    fn();
                }
            }
        }

        //-------------------------------------------------------------------------
        void render()
        {
            f32 w = static_cast<f32>(internal::_frame_buffer_width);
            f32 h = static_cast<f32>(internal::_frame_buffer_height);

            glm::mat4 active_p = internal::_active_camera.proj;
            glm::mat4 active_v = glm::lookAt(internal::_active_camera.eye, internal::_active_camera.center, internal::_active_camera.up);
            glm::mat4 active_vp = active_p * active_v;

            GL_CALL(glEnable(GL_BLEND));
            GL_CALL(glEnable(GL_CULL_FACE));
            GL_CALL(glEnable(GL_DEPTH_TEST));

            GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            GL_CALL(glCullFace(GL_BACK));
            GL_CALL(glDepthFunc(GL_LESS));

            internal::_primitive_instance_renderer->render(active_vp);

            internal::_primitive_batch_renderer->render(active_vp);
            internal::_primitive_stroke_batch_renderer->render(active_vp);

            glm::mat4 image_p = internal::_image_camera.proj;
            glm::mat4 image_v = glm::lookAt(internal::_image_camera.eye, internal::_image_camera.center, internal::_image_camera.up);
            glm::mat4 image_vp = image_p * image_v;

            internal::_image_batch_renderer->render(image_vp);
            internal::_image_stroke_batch_renderer->render(image_vp);

            glm::mat4 font_p = internal::_font_camera.proj;
            glm::mat4 font_v = glm::lookAt(internal::_font_camera.eye, internal::_font_camera.center, internal::_font_camera.up);
            glm::mat4 font_vp = font_p * font_v;

            internal::_font_batch_renderer->render(font_vp);

            for (auto& pair : internal::_custom_geometry_batch_renderers)
            {
                pair.second->render(active_vp);
            }

            GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, internal::_render_fbo));
            GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
            GL_CALL(glBlitFramebuffer(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, 0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }

        //-------------------------------------------------------------------------
        void end()
        {
            if (internal::_draw_end_subs.empty() == false)
            {
                for (auto& fn : internal::_draw_end_subs)
                {
                    fn();
                }
            }

            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }

        //-------------------------------------------------------------------------
        void push_active_shader(const std::string& tag, vertex_type vertex_type)
        {
            internal::_fill_user_shader = tag;
            internal::_fill_user_vertex_type = vertex_type;
        }

        //-------------------------------------------------------------------------
        void push_reset_shader()
        {
            internal::_fill_user_shader = {};
            internal::_fill_user_vertex_type = (vertex_type)-1; // invalid type
        }

        //-------------------------------------------------------------------------
        void begin_geometry_builder(const std::string& tag)
        {
            assert(!internal::_geometry_builder.is_active() && "Construction of previous shape has not been finished, call end_geometry_builder first");

            internal::_geometry_builder.activate(tag);
        }

        //-------------------------------------------------------------------------
        void end_geometry_builder()
        {
            internal::_geometry_builder.deactivate();
        }

        //-------------------------------------------------------------------------
        void push_active_camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj)
        {
            internal::_active_camera.eye = eye;
            internal::_active_camera.center = center;
            internal::_active_camera.up = up;

            internal::_active_camera.proj = proj;
        }

        //-------------------------------------------------------------------------
        void push_solid_rendering(bool enable)
        {
            internal::_primitive_instance_renderer->enable_solid_rendering(enable);

            internal::_primitive_batch_renderer->enable_solid_rendering(enable);
            internal::_primitive_stroke_batch_renderer->enable_solid_rendering(enable);

            internal::_image_batch_renderer->enable_solid_rendering(enable);
            internal::_image_stroke_batch_renderer->enable_solid_rendering(enable);

            internal::_font_batch_renderer->enable_solid_rendering(enable);

            for (auto& pair : internal::_custom_geometry_batch_renderers)
            {
                pair.second->enable_solid_rendering(enable);
            }
        }

        //-------------------------------------------------------------------------
        void push_wireframe_rendering(bool enable)
        {
            internal::_primitive_instance_renderer->enable_wireframe_rendering(enable);

            internal::_primitive_batch_renderer->enable_wireframe_rendering(enable);
            internal::_primitive_stroke_batch_renderer->enable_wireframe_rendering(enable);

            internal::_image_batch_renderer->enable_wireframe_rendering(enable);
            internal::_image_stroke_batch_renderer->enable_wireframe_rendering(enable);

            internal::_font_batch_renderer->enable_wireframe_rendering(enable);

            for (auto& pair : internal::_custom_geometry_batch_renderers)
            {
                pair.second->enable_wireframe_rendering(enable);
            }
        }

        //-------------------------------------------------------------------------
        void push_wireframe_linewidth(f32 line_width)
        {
            batch_renderer::set_wireframe_linewidth(line_width);
        }

        //-------------------------------------------------------------------------
        void push_wireframe_color(const glm::vec4& color)
        {
            batch_renderer::set_wireframe_linecolor(color::convert_color(color));
        }

        //-------------------------------------------------------------------------
        void push_scissor(s32 x, s32 y, s32 width, s32 height)
        {
            internal::_scissor_x = x;
            internal::_scissor_y = y;
            internal::_scissor_width = width;
            internal::_scissor_height = height;
        }

        //-------------------------------------------------------------------------
        void push_scissor_enable(bool enable)
        {
            internal::_scissor_enable = enable;
        }

        //-------------------------------------------------------------------------
        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data)
        {
            return create_image_item(width, height, channels, data, image_filter_type::NEAREST, image_wrap_type::REPEAT);
        }

        //-------------------------------------------------------------------------
        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data, image_filter_type filter_type, image_wrap_type wrap_type)
        {
            GLint format = GL_INVALID_VALUE;
            GLint usage = GL_INVALID_VALUE;

            switch (channels)
            {
            case 1:
                format = GL_R8;
                usage = GL_RED;
                break;
            case 2:
                format = GL_RG8;
                usage = GL_RG;
                break;
            case 3:
                format = GL_RGB;
                usage = GL_RGBA;
                break;
            case 4:
                format = GL_RGBA;
                usage = GL_RGBA;
                break;
            default:
                assert(false);
            }

            GLint filter = GL_INVALID_VALUE;

            switch (filter_type)
            {
            case image_filter_type::NEAREST: filter = GL_NEAREST; break;
            case image_filter_type::LINEAR: filter = GL_LINEAR; break;
            default:
                assert(false);
            }

            GLint wrap = GL_INVALID_VALUE;

            switch (wrap_type)
            {
            case image_wrap_type::CLAMP_TO_EDGE: wrap = GL_CLAMP_TO_EDGE; break;
            case image_wrap_type::REPEAT: wrap = GL_REPEAT; break;
            default:
                assert(false);
            }

            u32 texture_id;

            GL_CALL(glGenTextures(1, &texture_id));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));

            GL_CALL(glTexImage2D(
                GL_TEXTURE_2D,						// What (target)
                0,									// Mip-map level
                format,								// Internal format
                static_cast<GLsizei>(width),		// Width
                static_cast<GLsizei>(height),		// Height
                0,									// Border
                usage,								// Format (how to use)
                GL_UNSIGNED_BYTE,					// Type   (how to interpret)
                data));								// Data

            GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

            return texture_id;
        }

        //-------------------------------------------------------------------------
        void update_image_item(u32 id, f32 x, f32 y, f32 width, f32 height, s32 channels, u8* data)
        {
            GLint format = GL_INVALID_VALUE;

            switch (channels)
            {
            case 1:
                format = GL_R8;
                break;
            case 2:
                format = GL_RG8;
                break;
            case 3:
                format = GL_RGB;
                break;
            case 4:
                format = GL_RGBA;
                break;
            default:
                assert(false);
            }

            GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
            GL_CALL(glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                static_cast<GLint>(x),
                static_cast<GLint>(y),
                static_cast<GLsizei>(width),
                static_cast<GLsizei>(height),
                format,
                GL_UNSIGNED_BYTE,
                data));

            GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        }

        //-------------------------------------------------------------------------
        void submit_render_item(topology_type topology, const irender_item* item)
        {
            if (internal::_draw_mode == render_draw_mode::BATCHED)
            {
                if (internal::_geometry_builder.is_active() || !internal::_fill_user_shader.empty())
                {
                    internal::submit_custom_render_item(topology, item);
                }
                else
                {
                    if (brush::fill_enabled() == false)
                    {
                        // When there is no "stroke" and there is no "fill" the object would be invisible.
                        // So we don't add anything to the drawing list.
                        return;
                    }

                    internal::_primitive_batch_renderer->append_drawing_data(topology, item, brush::fill(), transform::active_world());
                }
            }
            else
            {
                if (internal::_geometry_builder.is_active() || !internal::_fill_user_shader.empty())
                {
                    return;
                }
                else
                {
                    if (brush::fill_enabled() == false)
                    {
                        // When there is no "stroke" and there is no "fill" the object would be invisible.
                        // So we don't add anything to the drawing list.
                        return;
                    }

                    internal::_primitive_instance_renderer->append_instance(topology, item);
                    internal::_primitive_instance_renderer->append_drawing_data(item->id(), brush::fill(), transform::active_world());
                }
            }
        }

        //-------------------------------------------------------------------------
        void submit_render_item(topology_type topology, const render_item& item)
        {
            if (internal::_geometry_builder.is_active() || !internal::_fill_user_shader.empty())
            {
                internal::submit_custom_render_item(topology, item);
            }
            else
            {
                if (brush::fill_enabled() == false)
                {
                    // When there is no "stroke" and there is no "fill" the object would be invisible.
                    // So we don't add anything to the drawing list.
                    return;
                }

                internal::_primitive_batch_renderer->append_drawing_data(topology, item, brush::fill(), transform::active_world());
            }
        }

        //-------------------------------------------------------------------------
        void submit_stroke_render_item(topology_type topology, const irender_item* item, bool outer)
        {
            if (internal::_geometry_builder.is_active())
            {
                log::warn("Stroking custom geometry is currently not supported");
                return;
            }

            if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 stroke_color = outer ? brush::stroke() : brush::inner_stroke();

            internal::_primitive_stroke_batch_renderer->append_drawing_data(topology, item, stroke_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_stroke_render_item(topology_type topology, const render_item& item, bool outer)
        {
            if (internal::_geometry_builder.is_active())
            {
                log::warn("Stroking custom geometry is currently not supported");
                return;
            }

            if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 stroke_color = outer ? brush::stroke() : brush::inner_stroke();

            internal::_primitive_stroke_batch_renderer->append_drawing_data(topology, item, stroke_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_font_item(const render_item& item)
        {
            internal::_font_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, brush::fill(), transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_font_item(const irender_item* item)
        {
            internal::_font_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, brush::fill(), transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_image_item(const render_item& item)
        {
            if (internal::_geometry_builder.is_active() || !internal::_fill_user_shader.empty())
            {
                internal::submit_custom_image_item(item);
            }
            else
            {
                internal::_image_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, brush::tint(), transform::active_world());
            }
        }

        //-------------------------------------------------------------------------
        void submit_image_item(const irender_item* item)
        {
            if (internal::_geometry_builder.is_active() || !internal::_fill_user_shader.empty())
            {
                internal::submit_custom_image_item(item);
            }
            else
            {
                internal::_image_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, brush::tint(), transform::active_world());
            }
        }

        //-------------------------------------------------------------------------
        void submit_stroke_image_item(const render_item& item, bool outer)
        {
            if (internal::_geometry_builder.is_active())
            {
                log::warn("Stroking custom image geometry is currently not supported");
                return;
            }

            glm::vec4 stroke_color = outer ? brush::stroke() : brush::inner_stroke();

            internal::_image_stroke_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, stroke_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_stroke_image_item(const irender_item* item, bool outer)
        {
            if (internal::_geometry_builder.is_active())
            {
                log::warn("Stroking custom image geometry is currently not supported");
                return;
            }

            glm::vec4 stroke_color = outer ? brush::stroke() : brush::inner_stroke();

            internal::_image_stroke_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, stroke_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void clear_color(f32 r, f32 g, f32 b, f32 a)
        {
            brush::push_background_color(glm::vec4(r, g, b, a));

            GL_CALL(glClearColor(r, g, b, a));
        }

        //-------------------------------------------------------------------------
        void clear(u32 flags)
        {
            GL_CALL(glClear(flags));
        }

        //-------------------------------------------------------------------------
        void register_on_draw_begin(std::function<void()> draw_begin)
        {
            internal::_draw_begin_subs.push_back(draw_begin);
        }

        //-------------------------------------------------------------------------
        void register_on_draw_end(std::function<void()> draw_end)
        {
            internal::_draw_end_subs.push_back(draw_end);
        }

        //-------------------------------------------------------------------------
        bool scissor_enabled()
        {
            return internal::_scissor_enable;
        }
        
        //-------------------------------------------------------------------------
        ScissorRect scissor()
        {
            return { internal::_scissor_x, internal::_scissor_y, internal::_scissor_width, internal::_scissor_height };
        }
    }
}