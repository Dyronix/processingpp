#include "render/render.h"
#include "render/render_transform.h"
#include "render/render_batch.h"
#include "render/render_batch_renderer.h"
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
            s32 _fill_color = 0xFFFFFFFF;
            bool _fill_enable = true;
            f32 _stroke_width = 1.0f;
            s32 _stroke_color = 0xFF000000;
            bool _stroke_enable = false;
            f32 _inner_stroke_width = 1.0f;
            s32 _inner_stroke_color = 0xFF000000;
            bool _inner_stroke_enable = false;
            s32 _tint_color = 0xFFFFFFFF;
            bool _tint_enable = false;

            s32 _bg_color = 0xFF000000;

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
            void create_frame_buffer()
            {
                glGenFramebuffers(1, &_render_fbo);
                glBindFramebuffer(GL_FRAMEBUFFER, _render_fbo);

                glGenTextures(1, &_render_texture);
                glBindTexture(GL_TEXTURE_2D, _render_texture);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _frame_buffer_width, _frame_buffer_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _render_texture, 0);

                glGenRenderbuffers(1, &_render_depth_rbo);
                glBindRenderbuffer(GL_RENDERBUFFER, _render_depth_rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _frame_buffer_width, _frame_buffer_height);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _render_depth_rbo);

                u32 attachments[1] = { GL_COLOR_ATTACHMENT0 };
                glDrawBuffers(1, attachments);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    log::error("FramebufferStatus != FRAMEBUFFER_COMPLETE");
                    return;
                }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

            //-------------------------------------------------------------------------
            std::unique_ptr<batch_renderer> _primitive_batch_renderer;
            std::unique_ptr<batch_renderer> _primitive_stroke_batch_renderer;
            std::unique_ptr<batch_renderer> _image_batch_renderer;
            std::unique_ptr<batch_renderer> _image_stroke_batch_renderer;
            std::unique_ptr<batch_renderer> _font_batch_renderer;
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
            internal::_font_batch_renderer->terminate();
            internal::_image_batch_renderer->terminate();
            internal::_image_stroke_batch_renderer->terminate();
            internal::_primitive_batch_renderer->terminate();
            internal::_primitive_stroke_batch_renderer->terminate();
        }

        //-------------------------------------------------------------------------
        void begin()
        {
            internal::_primitive_batch_renderer->begin();
            internal::_primitive_stroke_batch_renderer->begin();
            internal::_image_batch_renderer->begin();
            internal::_image_stroke_batch_renderer->begin();
            internal::_font_batch_renderer->begin();

            glBindFramebuffer(GL_FRAMEBUFFER, internal::_render_fbo);

            // Clear the background to black
            glViewport(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Reset to the user clear color
            glm::vec4 bg_color = color::convert_color(internal::_bg_color);
            glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a);

            if (internal::_scissor_enable)
            {
                glEnable(GL_SCISSOR_TEST);
                glScissor(
                    std::clamp(internal::_scissor_x, 0, internal::_frame_buffer_width),
                    std::clamp(internal::_scissor_y, 0, internal::_frame_buffer_height),
                    std::clamp(internal::_scissor_width, internal::_min_frame_buffer_width, internal::_frame_buffer_width),
                    std::clamp(internal::_scissor_height, internal::_min_frame_buffer_height, internal::_frame_buffer_height));
            }
            else
            {
                glDisable(GL_SCISSOR_TEST);
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        //-------------------------------------------------------------------------
        void render()
        {
            f32 w = static_cast<f32>(internal::_frame_buffer_width);
            f32 h = static_cast<f32>(internal::_frame_buffer_height);

            glm::mat4 p = internal::_active_camera.proj;
            glm::mat4 v = glm::lookAt(internal::_active_camera.eye, internal::_active_camera.center, internal::_active_camera.up);
            glm::mat4 vp = p * v;

            glEnable(GL_BLEND);
            glEnable(GL_CULL_FACE);
            glEnable(GL_DEPTH_TEST);
            
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glCullFace(GL_BACK);
            glDepthFunc(GL_LESS);

            internal::_primitive_batch_renderer->render(vp);
            internal::_primitive_stroke_batch_renderer->render(vp);

            p = internal::_image_camera.proj;
            v = glm::lookAt(internal::_image_camera.eye, internal::_image_camera.center, internal::_image_camera.up);
            vp = p * v;

            internal::_image_batch_renderer->render(vp);
            internal::_image_stroke_batch_renderer->render(vp);

            p = internal::_font_camera.proj;
            v = glm::lookAt(internal::_font_camera.eye, internal::_font_camera.center, internal::_font_camera.up);
            vp = p * v;

            internal::_font_batch_renderer->render(vp);

            glBindFramebuffer(GL_READ_FRAMEBUFFER, internal::_render_fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, 0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        //-------------------------------------------------------------------------
        void end()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
            internal::_primitive_batch_renderer->enable_solid_rendering(enable);
            internal::_primitive_stroke_batch_renderer->enable_solid_rendering(enable);

            internal::_image_batch_renderer->enable_solid_rendering(enable);
            internal::_image_stroke_batch_renderer->enable_solid_rendering(enable);

            internal::_font_batch_renderer->enable_solid_rendering(enable);
        }

        //-------------------------------------------------------------------------
        void push_wireframe_rendering(bool enable)
        {
            internal::_primitive_batch_renderer->enable_wireframe_rendering(enable);
            internal::_primitive_stroke_batch_renderer->enable_wireframe_rendering(enable);
            
            internal::_image_batch_renderer->enable_wireframe_rendering(enable);
            internal::_image_stroke_batch_renderer->enable_wireframe_rendering(enable);

            internal::_font_batch_renderer->enable_wireframe_rendering(enable);
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
        void push_fill_color(const glm::vec4& color)
        {
            internal::_fill_color = color::convert_color(color);
        }

        //-------------------------------------------------------------------------
        void push_fill_enable(bool enable)
        {
            internal::_fill_enable = enable;
        }

        //-------------------------------------------------------------------------
        void push_stroke_width(f32 w)
        {
            internal::_stroke_width = w;
        }

        //-------------------------------------------------------------------------
        void push_stroke_color(const glm::vec4& color)
        {
            internal::_stroke_color = color::convert_color(color);
        }

        //-------------------------------------------------------------------------
        void push_stroke_enable(bool enable)
        {
            internal::_stroke_enable = enable;
        }

        //-------------------------------------------------------------------------
        void push_inner_stroke_width(f32 w)
        {
            internal::_inner_stroke_width = w;
        }

        //-------------------------------------------------------------------------
        void push_inner_stroke_color(const glm::vec4& color)
        {
            internal::_inner_stroke_color = color::convert_color(color);
        }

        //-------------------------------------------------------------------------
        void push_inner_stroke_enable(bool enable)
        {
            internal::_inner_stroke_enable = enable;
        }

        //-------------------------------------------------------------------------
        void push_tint_color(const glm::vec4& color)
        {
            internal::_tint_color = color::convert_color(color);
        }

        //-------------------------------------------------------------------------
        void push_tint_enable(bool enable)
        {
            internal::_tint_enable = enable;
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

            switch(filter_type)
            {
                case image_filter_type::NEAREST: filter = GL_NEAREST; break;
                case image_filter_type::LINEAR: filter = GL_LINEAR; break;
                default:
                    assert(false);
            }

            GLint wrap = GL_INVALID_VALUE;

            switch(wrap_type)
            {
                case image_wrap_type::CLAMP_TO_EDGE: wrap = GL_CLAMP_TO_EDGE; break;
                case image_wrap_type::REPEAT: wrap = GL_REPEAT; break;
                default:
                    assert(false);
            }

            u32 texture_id;

            glGenTextures(1, &texture_id);
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

            glTexImage2D(
                GL_TEXTURE_2D,						// What (target)
                0,									// Mip-map level
                format,								// Internal format
                static_cast<GLsizei>(width),		// Width
                static_cast<GLsizei>(height),		// Height
                0,									// Border
                usage,								// Format (how to use)
                GL_UNSIGNED_BYTE,					// Type   (how to interpret)
                data);								// Data

            glBindTexture(GL_TEXTURE_2D, 0);

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

            glBindTexture(GL_TEXTURE_2D, id);
            glTexSubImage2D(
                GL_TEXTURE_2D,
                0,
                static_cast<GLint>(x),
                static_cast<GLint>(y),
                static_cast<GLsizei>(width),
                static_cast<GLsizei>(height),
                format,
                GL_UNSIGNED_BYTE,
                data);

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        //-------------------------------------------------------------------------
        void submit_render_item(topology_type topology, const render_item& item)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 fill_color = color::convert_color(internal::_fill_enable ? internal::_fill_color : internal::_bg_color);

            internal::_primitive_batch_renderer->append_drawing_data(topology, item, fill_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_stroke_render_item(topology_type topology, const render_item& item, bool outer)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 stroke_color = outer ? color::convert_color(internal::_stroke_color) : color::convert_color(internal::_inner_stroke_color);

            internal::_primitive_stroke_batch_renderer->append_drawing_data(topology, item, stroke_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_font_item(const render_item& item)
        {
            glm::vec4 fill_color = color::convert_color(internal::_fill_color);

            internal::_font_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, fill_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_image_item(const render_item& item)
        {
            glm::vec4 tint_color = color::convert_color(internal::_tint_color);

            internal::_image_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, tint_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void submit_stroke_image_item(const render_item& item, bool outer)
        {
            glm::vec4 stroke_color = outer ? color::convert_color(internal::_stroke_color) : color::convert_color(internal::_inner_stroke_color);

            internal::_image_stroke_batch_renderer->append_drawing_data(topology_type::TRIANGLES, item, stroke_color, transform::active_world());
        }

        //-------------------------------------------------------------------------
        void clear_color(f32 r, f32 g, f32 b, f32 a)
        {
            internal::_bg_color = color::convert_color(glm::vec4(r, g, b, a));
            glClearColor(r, g, b, a);
        }

        //-------------------------------------------------------------------------
        void clear(u32 flags)
        {           
            glClear(flags);
        }

        //-------------------------------------------------------------------------
        bool fill_enabled()
        {
            return internal::_fill_enable;
        }
        
        //-------------------------------------------------------------------------
        bool stroke_enabled()
        {
            return internal::_stroke_enable;
        }

        //-------------------------------------------------------------------------
        bool inner_stroke_enabled()
        {
            return internal::_inner_stroke_enable;
        }

        //-------------------------------------------------------------------------
        f32 stroke_width()
        {
            return internal::_stroke_width;
        }

        //-------------------------------------------------------------------------
        f32 inner_stroke_width()
        {
            return internal::_inner_stroke_width;
        }
        
        //-------------------------------------------------------------------------
        bool tint_enabled()
        {
            return internal::_tint_enable;
        }

        //-------------------------------------------------------------------------
        bool scissor_enabled()
        {
            return internal::_scissor_enable;
        }

        //-------------------------------------------------------------------------
        glm::vec4 fill()
        {
            return color::convert_color(internal::_fill_color);
        }
        
        //-------------------------------------------------------------------------
        glm::vec4 stroke()
        {
            return color::convert_color(internal::_stroke_color);
        }

        //-------------------------------------------------------------------------
        glm::vec4 inner_stroke()
        {
            return color::convert_color(internal::_inner_stroke_color);
        }
        
        //-------------------------------------------------------------------------
        glm::vec4 tint()
        {
            return color::convert_color(internal::_tint_color);
        }
        
        //-------------------------------------------------------------------------
        ScissorRect scissor()
        {
            return { internal::_scissor_x, internal::_scissor_y, internal::_scissor_width, internal::_scissor_height };
        }
    }
}