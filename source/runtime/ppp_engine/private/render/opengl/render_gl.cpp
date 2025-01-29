#include "render/render.h"
#include "render/render_batch.h"
#include "render/render_instance.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"

#include "render/helpers/render_vertex_layouts.h"
#include "render/helpers/render_instance_layouts.h"

#include "render/opengl/render_gl_error.h"

#include "resources/shader_pool.h"

#include "util/log.h"
#include "util/color_ops.h"
#include "util/transform_stack.h"
#include "util/brush.h"

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
            s32 _scissor_x = -1;
            s32 _scissor_y = -1;
            s32 _scissor_width = -1;
            s32 _scissor_height = -1;
            bool _scissor_enable = false;

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
            std::unordered_map<std::string, std::unique_ptr<instance_renderer>> _instance_renderers;
            std::unordered_map<std::string, std::unique_ptr<batch_renderer>> _batch_renderers;

            //-------------------------------------------------------------------------
            std::unique_ptr<batch_renderer> _font_renderer;

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
            void submit_custom_render_item(topology_type topology, const irender_item* item, const glm::vec4& color)
            {
                if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false && brush::fill_enabled() == false)
                {
                    // When there is no "stroke" and there is no "fill" the object would be invisible.
                    // So we don't add anything to the drawing list.
                    return;
                }

                const std::string& shader_tag = _fill_user_shader.empty() ? _geometry_builder.shader_tag() : _fill_user_shader;

                if (internal::_draw_mode == render_draw_mode::BATCHED)
                {
                    if (internal::_batch_renderers.find(shader_tag) == std::cend(internal::_batch_renderers))
                    {
                        std::unique_ptr<batch_renderer> renderer = nullptr;
                        if (item->has_textures() == false)
                        {
                            renderer = std::make_unique<primitive_batch_renderer>(
                                _fill_user_shader.empty() ? pos_col_layout().data() : fill_user_layout(internal::_fill_user_vertex_type),
                                _fill_user_shader.empty() ? pos_col_layout().size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                                shader_tag);
                        }
                        else
                        {
                            renderer = std::make_unique<texture_batch_renderer>(
                                _fill_user_shader.empty() ? pos_tex_col_layout().data() : fill_user_layout(internal::_fill_user_vertex_type),
                                _fill_user_shader.empty() ? pos_tex_col_layout().size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                                shader_tag);
                        }

                        if (_geometry_builder.is_active())
                        {
                            renderer->buffer_policy(render_buffer_policy::RETAINED);
                        }

                        renderer->draw_policy(render_draw_policy::CUSTOM);
                        internal::_batch_renderers.emplace(shader_tag, std::move(renderer));
                    }

                    internal::_batch_renderers.at(shader_tag)->append_drawing_data(topology, item, color, transform_stack::active_world());
                }
                else
                {
                    if (internal::_instance_renderers.find(shader_tag) == std::cend(internal::_instance_renderers))
                    {
                        std::unique_ptr<instance_renderer> renderer = nullptr;
                        if (item->has_textures() == false)
                        {
                            renderer = std::make_unique<primitive_instance_renderer>(
                                _fill_user_shader.empty() ? pos_layout().data() : fill_user_layout(internal::_fill_user_vertex_type),
                                _fill_user_shader.empty() ? pos_layout().size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                                color_world_layout().data(),
                                color_world_layout().size(),
                                shader_tag);
                        }
                        else
                        {
                            renderer = std::make_unique<texture_instance_renderer>(
                                _fill_user_shader.empty() ? pos_tex_layout().data() : fill_user_layout(internal::_fill_user_vertex_type),
                                _fill_user_shader.empty() ? pos_tex_layout().size() : fill_user_layout_count(internal::_fill_user_vertex_type),
                                color_world_matid_layout().data(),
                                color_world_matid_layout().size(),
                                shader_tag);
                        }

                        if (_geometry_builder.is_active())
                        {
                            renderer->buffer_policy(render_buffer_policy::RETAINED);
                        }

                        renderer->draw_policy(render_draw_policy::CUSTOM);

                        internal::_instance_renderers.emplace(shader_tag, std::move(renderer));
                    }

                    internal::_instance_renderers.at(shader_tag)->append_drawing_data(topology, item, color, transform_stack::active_world());
                }
            }
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

            internal::create_frame_buffer();

            internal::_font_renderer = std::make_unique<texture_batch_renderer>(pos_tex_col_layout().data(), pos_tex_col_layout().size(), shader_pool::tags::unlit_font);

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            // Font
            internal::_font_renderer->terminate();

            // Custom
            for (auto& pair : internal::_batch_renderers)
            {
                pair.second->terminate();
            }
            for (auto& pair : internal::_instance_renderers)
            {
                pair.second->terminate();
            }
        }

        //-------------------------------------------------------------------------
        void begin()
        {
            // Font
            internal::_font_renderer->begin();

            // Custom
            for (auto& pair : internal::_batch_renderers)
            {
                pair.second->begin();
            }
            for (auto& pair : internal::_instance_renderers)
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
        void render(const render_context& context)
        {
            f32 w = static_cast<f32>(internal::_frame_buffer_width);
            f32 h = static_cast<f32>(internal::_frame_buffer_height);

            GL_CALL(glDisable(GL_BLEND));
            GL_CALL(glEnable(GL_CULL_FACE));
            GL_CALL(glEnable(GL_DEPTH_TEST));

            GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            GL_CALL(glCullFace(GL_BACK));
            GL_CALL(glDepthFunc(GL_LESS));

            const glm::mat4& cam_font_p = context.mat_proj_font;
            const glm::mat4& cam_font_v = context.mat_view_font;
            glm::mat4 cam_font_vp = cam_font_p * cam_font_v;

            internal::_font_renderer->render(cam_font_vp);

            const glm::mat4& cam_active_p = context.mat_proj_active;
            const glm::mat4& cam_active_v = context.mat_view_active;
            glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            for (auto& pair : internal::_batch_renderers)
            {
                pair.second->render(cam_active_vp);
            }

            for (auto& pair : internal::_instance_renderers)
            {
                pair.second->render(cam_active_vp);
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
        void draw_mode(render_draw_mode mode)
        {
            internal::_draw_mode = mode;
        }

        //-------------------------------------------------------------------------
        render_draw_mode draw_mode()
        {
            return internal::_draw_mode;
        }

        //-------------------------------------------------------------------------
        void push_active_shader(const std::string& tag, vertex_type type)
        {
            internal::_fill_user_shader = tag;
            internal::_fill_user_vertex_type = type;
        }

        //-------------------------------------------------------------------------
        const std::string& active_shader()
        {
            return internal::_fill_user_shader;
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
        void push_solid_rendering(bool enable)
        {
            // Font
            internal::_font_renderer->enable_solid_rendering(enable);

            // Custom
            for (auto& pair : internal::_batch_renderers)
            {
                pair.second->enable_solid_rendering(enable);
            }
            for (auto& pair : internal::_instance_renderers)
            {
                pair.second->enable_solid_rendering(enable);
            }
        }

        //-------------------------------------------------------------------------
        void push_wireframe_rendering(bool enable)
        {
            // Font
            internal::_font_renderer->enable_wireframe_rendering(enable);

            // Custom
            for (auto& pair : internal::_batch_renderers)
            {
                pair.second->enable_wireframe_rendering(enable);
            }

            for (auto& pair : internal::_instance_renderers)
            {
                pair.second->enable_wireframe_rendering(enable);
            }
        }

        //-------------------------------------------------------------------------
        void push_wireframe_linewidth(f32 line_width)
        {
            batch_renderer::set_wireframe_linewidth(line_width);
            instance_renderer::set_wireframe_linewidth(line_width);
        }

        //-------------------------------------------------------------------------
        void push_wireframe_color(const glm::vec4& color)
        {
            batch_renderer::set_wireframe_linecolor(color::convert_color(color));
            instance_renderer::set_wireframe_linewidth(color::convert_color(color));
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
            internal::submit_custom_render_item(topology, item, item->has_textures() ? brush::tint() : brush::fill());
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

            internal::submit_custom_render_item(topology, item, stroke_color);
        }

        //-------------------------------------------------------------------------
        void submit_font_item(const irender_item* item)
        {
            internal::_font_renderer->append_drawing_data(topology_type::TRIANGLES, item, brush::fill(), transform_stack::active_world());
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
        scissor_rect scissor()
        {
            return { internal::_scissor_x, internal::_scissor_y, internal::_scissor_width, internal::_scissor_height };
        }
    }
}