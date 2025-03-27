#include "render/render.h"
#include "render/render_batch.h"
#include "render/render_instance.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"
#include "render/render_shader.h"
#include "render/render_context.h"
#include "render/render_scissor.h"
#include "render/render_pipeline.h"

#include "render/render_predepth_pass.h"
#include "render/render_shadow_pass.h"
#include "render/render_forward_shading_pass.h"
#include "render/render_ui_pass.h"
#include "render/render_unlit_pass.h"
#include "render/render_unlit_wireframe_pass.h"
#include "render/render_blit_pass.h"

#include "render/helpers/render_vertex_layouts.h"
#include "render/helpers/render_instance_layouts.h"
#include "render/helpers/render_event_dispatcher.h"

#include "render/opengl/render_gl_error.h"
#include "render/opengl/render_gl_api.h"

#include "resources/shader_pool.h"
#include "resources/framebuffer_pool.h"

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
#include <iostream>
#include <sstream>

#define LOG_GL_NOTIFICATIONS 0
#define LOG_GL_LOW 0
#define LOG_GL_MEDIUM 1

namespace ppp
{
    namespace render
    {
        namespace opengl
        {
            enum class debug_message_severity
            {
                UNKNOWN,
                NOTIFICATION,
                LOW,
                MEDIUM,
                HIGH
            };

            //-------------------------------------------------------------------------
            void APIENTRY debug_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* uerParam)
            {
                static std::string             source_str = "UNKNOWN";
                static std::string             type_str = "UNKNOWN";
                static std::string             severity_str = "UNKNOWN";

                switch (source)
                {
                case GL_DEBUG_SOURCE_API:               source_str = "API"; break;
                case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     source_str = "WINDOW_SYSTEM"; break;
                case GL_DEBUG_SOURCE_SHADER_COMPILER:   source_str = "SHADER_COMPILER"; break;
                case GL_DEBUG_SOURCE_THIRD_PARTY:       source_str = "THIRD_PARTY"; break;
                case GL_DEBUG_SOURCE_APPLICATION:       source_str = "APPLICATION"; break;
                case GL_DEBUG_SOURCE_OTHER:             source_str = "OTHER"; break;
                }

                switch (type)
                {
                case GL_DEBUG_TYPE_ERROR:               type_str = "ERROR"; break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_str = "DEPRECATED_BEHAVIOR"; break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_str = "UNDEFINED_BEHAVIOR"; break;
                case GL_DEBUG_TYPE_PERFORMANCE:         type_str = "PERFORMANCE"; break;
                case GL_DEBUG_TYPE_PORTABILITY:         type_str = "PORTABILITY"; break;
                case GL_DEBUG_TYPE_MARKER:              type_str = "MARKER"; break;
                case GL_DEBUG_TYPE_PUSH_GROUP:          type_str = "PUSH_GROUP"; break;
                case GL_DEBUG_TYPE_POP_GROUP:           type_str = "POP_GROUP"; break;
                case GL_DEBUG_TYPE_OTHER:               type_str = "OTHER"; break;
                }

                auto lvl = debug_message_severity::UNKNOWN;
                switch (severity)
                {
                case GL_DEBUG_SEVERITY_HIGH:
                    severity_str = "HIGH";
                    ppp::log::error("OpenGL: Severity: [{}], Source: [{}], Type: [{}], ID: [{}]\nMessage: {}", severity_str, source_str, type_str, id, message);
                    break;
                    #if LOG_GL_MEDIUM
                case GL_DEBUG_SEVERITY_MEDIUM:
                    severity_str = "MEDIUM";
                    ppp::log::warn("OpenGL: Severity: [{}], Source: [{}], Type: [{}], ID: [{}]\nMessage: {}", severity_str, source_str, type_str, id, message);
                    break;
                    #endif
                    #if LOG_GL_LOW
                case GL_DEBUG_SEVERITY_LOW:
                    severity_str = "LOW";
                    ppp::log::warn("OpenGL: Severity: [{}], Source: [{}], Type: [{}], ID: [{}]\nMessage: {}", severity_str, source_str, type_str, id, message);
                    break;
                    #endif
                    #if LOG_GL_NOTIFICATIONS
                case GL_DEBUG_SEVERITY_NOTIFICATION:
                    severity_str = "NOTIFICATION";
                    ppp::log::info("OpenGL: Severity: [{}], Source: [{}], Type: [{}], ID: [{}]\nMessage: {}", severity_str, source_str, type_str, id, message);
                    break;
                    #endif
                }

                if (severity == GL_DEBUG_SEVERITY_HIGH)
                {
                    throw std::runtime_error("An OpenGL runtime error occurred.");
                }
            }
        }

        using font_renderer = std::unique_ptr<texture_batch_renderer>;

        //-------------------------------------------------------------------------
        struct context
        {
            // drawing
            render_draw_mode            draw_mode = render_draw_mode::BATCHED;
            render_rendering_mode       rendering_mode = render_rendering_mode::FORWARD;
            render_scissor              scissor = {};

            // shaders
            string::string_id           fill_user_shader = string::string_id::create_invalid();
            string::string_id           stroke_user_shader = string::string_id::create_invalid();

            // frame buffer
            string::string_id           main_framebuffer_tag = string::store_sid("main_framebuffer");
            string::string_id           shadow_framebuffer_tag = string::store_sid("shadow_framebuffer");

            // rendering
            render_pipeline             render_pipeline;

            // renderers
            instance_renderers_hash_map instance_renderers = {};
            batch_renderers_hash_map    batch_renderers = {};

            font_renderer               font_renderer = nullptr;

            // opengl version       
            s32                         major = 0;
            s32                         minor = 0;
        } g_ctx;

        //-------------------------------------------------------------------------
        render_context make_render_context(const camera_context* camera_context)
        {
            render_context render_context;
            render_context.camera_context = camera_context;
            render_context.font_renderer = g_ctx.font_renderer.get();
            render_context.batch_renderers = &g_ctx.batch_renderers;
            render_context.instance_renderers = &g_ctx.instance_renderers;
            render_context.scissor = &g_ctx.scissor;

            return render_context;
        }

        //-------------------------------------------------------------------------
        void submit_custom_render_item(topology_type topology, const irender_item* item, const glm::vec4& color)
        {
            if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false && brush::fill_enabled() == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            string::string_id shader_tag = g_ctx.fill_user_shader;

            if (g_ctx.draw_mode == render_draw_mode::BATCHED)
            {
                auto& batch_ren = g_ctx.batch_renderers;

                if (g_ctx.batch_renderers.find(shader_tag) == std::cend(g_ctx.batch_renderers))
                {
                    std::unique_ptr<batch_renderer> renderer = nullptr;

                    if (item->has_textures() == false)
                    {
                        assert(g_ctx.fill_user_shader.is_none() == false);

                        renderer = std::make_unique<primitive_batch_renderer>(shader_tag);
                    }
                    else
                    {
                        renderer = std::make_unique<texture_batch_renderer>(shader_tag);
                    }

                    g_ctx.batch_renderers.emplace(shader_tag, std::move(renderer));
                }

                g_ctx.batch_renderers.at(shader_tag)->append_drawing_data(topology, item, color, transform_stack::active_world());
            }
            else
            {
                if (g_ctx.instance_renderers.find(shader_tag) == std::cend(g_ctx.instance_renderers))
                {
                    std::unique_ptr<instance_renderer> renderer = nullptr;
                    if (item->has_textures() == false)
                    {
                        renderer = std::make_unique<primitive_instance_renderer>(
                            color_world_layout().data(),
                            color_world_layout().size(),
                            shader_tag);
                    }
                    else
                    {
                        renderer = std::make_unique<texture_instance_renderer>(
                            color_world_matid_layout().data(),
                            color_world_matid_layout().size(),
                            shader_tag);
                    }

                    g_ctx.instance_renderers.emplace(shader_tag, std::move(renderer));
                }

                g_ctx.instance_renderers.at(shader_tag)->append_drawing_data(topology, item, color, transform_stack::active_world());
            }
        }

        //-------------------------------------------------------------------------
        void parse_gl_version(const char* version_string)
        {
            std::stringstream ss(version_string);
            char dot;
            ss >> g_ctx.major >> dot >> g_ctx.minor;
        }
        //-------------------------------------------------------------------------
        void print_gl_version(const char* version_string)
        {
            log::info("OpenGL version format: <major_version>.<minor_version>.<release_number> <vendor-specific information>");
            log::info("OpenGL version: {}", version_string);
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

            auto opengl_version = reinterpret_cast<const char*>(opengl::api::instance().get_string_value(GL_VERSION));

            parse_gl_version(opengl_version);
            print_gl_version(opengl_version);

#if _DEBUG
            // glDebugMessageCallback is only available on OpenGL 4.3 or higher
            if (g_ctx.major >= 4 && g_ctx.minor >= 3)
            {
                s32 flags = 0; 
                opengl::api::instance().get_integer_value(GL_CONTEXT_FLAGS, &flags);

                if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                {
                    opengl::api::instance().enable(GL_DEBUG_OUTPUT);
                    opengl::api::instance().enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

                    glDebugMessageCallback(opengl::debug_message_callback, nullptr);
                    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
                }
            }
#endif

            g_ctx.scissor.x = 0;
            g_ctx.scissor.y = 0;
            g_ctx.scissor.width = w;
            g_ctx.scissor.height = h;
            g_ctx.scissor.enable = false;

            g_ctx.font_renderer = std::make_unique<texture_batch_renderer>(shader_pool::tags::unlit::font());

            g_ctx.render_pipeline.add_pass(std::make_unique<predepth_pass>(shader_pool::tags::unlit::predepth(), framebuffer_pool::tags::composite()));
            g_ctx.render_pipeline.add_pass(std::make_unique<shadow_pass>(shader_pool::tags::unlit::shadow(), framebuffer_pool::tags::shadow_map()));
            g_ctx.render_pipeline.add_pass(std::make_unique<forward_shading_pass>(shader_pool::tags::lit::lit(), framebuffer_pool::tags::composite()));
            g_ctx.render_pipeline.add_pass(std::make_unique<blit_pass>(framebuffer_pool::tags::composite(), framebuffer_flags::COLOR | framebuffer_flags::DEPTH));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            // Font
            g_ctx.font_renderer->terminate();

            // Custom
            for (auto& pair : g_ctx.batch_renderers)
            {
                pair.second->terminate();
            }
            g_ctx.batch_renderers.clear();

            for (auto& pair : g_ctx.instance_renderers)
            {
                pair.second->terminate();
            }
            g_ctx.instance_renderers.clear();
        }

        //-------------------------------------------------------------------------
        void begin(const camera_context* context)
        {
            // Font
            g_ctx.font_renderer->begin();

            // Custom
            for (auto& pair : g_ctx.batch_renderers)
            {
                pair.second->begin();
            }
            for (auto& pair : g_ctx.instance_renderers)
            {
                pair.second->begin();
            }

            broadcast_on_draw_begin();
        }

        //-------------------------------------------------------------------------
        void render(const camera_context* context)
        {
            g_ctx.render_pipeline.execute(make_render_context(context));
        }

        //-------------------------------------------------------------------------
        void end(const camera_context* context)
        {
            broadcast_on_draw_end();
        }

        //-------------------------------------------------------------------------
        void draw_mode(render_draw_mode mode)
        {
            g_ctx.draw_mode = mode;
        }

        //-------------------------------------------------------------------------
        void rendering_mode(render_rendering_mode mode)
        {
            g_ctx.rendering_mode = mode;
        }

        //-------------------------------------------------------------------------
        render_draw_mode draw_mode()
        {
            return g_ctx.draw_mode;
        }

        //-------------------------------------------------------------------------
        render_rendering_mode rendering_mode()
        {
            return g_ctx.rendering_mode;
        }

        //-------------------------------------------------------------------------
        void push_active_shader(string::string_id tag)
        {
            g_ctx.fill_user_shader = tag;
        }

        //-------------------------------------------------------------------------
        string::string_id active_shader()
        {
            return g_ctx.fill_user_shader;
        }

        //-------------------------------------------------------------------------
        void push_solid_rendering(bool enable)
        {
            // Font
            g_ctx.font_renderer->enable_solid_rendering(enable);

            // Custom
            for (auto& pair : g_ctx.batch_renderers)
            {
                pair.second->enable_solid_rendering(enable);
            }
            for (auto& pair : g_ctx.instance_renderers)
            {
                pair.second->enable_solid_rendering(enable);
            }
        }

        //-------------------------------------------------------------------------
        void push_wireframe_rendering(bool enable)
        {
            // Font
            g_ctx.font_renderer->enable_wireframe_rendering(enable);

            // Custom
            for (auto& pair : g_ctx.batch_renderers)
            {
                pair.second->enable_wireframe_rendering(enable);
            }

            for (auto& pair : g_ctx.instance_renderers)
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
            g_ctx.scissor.x = x;
            g_ctx.scissor.y = y;
            g_ctx.scissor.width = width;
            g_ctx.scissor.height = height;
        }

        //-------------------------------------------------------------------------
        void push_scissor_enable(bool enable)
        {
            g_ctx.scissor.enable = enable;
        }

        //-------------------------------------------------------------------------
        bool scissor_enabled()
        {
            return g_ctx.scissor.enable;
        }

        //-------------------------------------------------------------------------
        scissor_rect scissor()
        {
            return { g_ctx.scissor.x, g_ctx.scissor.y, g_ctx.scissor.width, g_ctx.scissor.height };
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

            opengl::api::instance().generate_textures(1, &texture_id);
            opengl::api::instance().bind_texture(GL_TEXTURE_2D, texture_id);
            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            opengl::api::instance().set_texture_integer_parameter(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

            opengl::api::instance().texture_image_2D(
                GL_TEXTURE_2D,						// What (target)
                0,									// Mip-map level
                format,								// Internal format
                static_cast<GLsizei>(width),		// Width
                static_cast<GLsizei>(height),		// Height
                0,									// Border
                usage,								// Format (how to use)
                GL_UNSIGNED_BYTE,					// Type   (how to interpret)
                data);								// Data

            opengl::api::instance().bind_texture(GL_TEXTURE_2D, 0);

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

            opengl::api::instance().bind_texture(GL_TEXTURE_2D, id);
            opengl::api::instance().texture_sub_image_2D(
                GL_TEXTURE_2D,
                0,
                static_cast<GLint>(x),
                static_cast<GLint>(y),
                static_cast<GLsizei>(width),
                static_cast<GLsizei>(height),
                format,
                GL_UNSIGNED_BYTE,
                data);

            opengl::api::instance().bind_texture(GL_TEXTURE_2D, 0);
        }

        //-------------------------------------------------------------------------
        void read_pixels(s32 x, s32 y, s32 width, s32 height, u8* data)
        {
            opengl::api::instance().read_pixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }

        //-------------------------------------------------------------------------
        void submit_render_item(topology_type topology, const irender_item* item)
        {
            submit_custom_render_item(topology, item, item->has_textures() ? brush::tint() : brush::fill());
        }

        //-------------------------------------------------------------------------
        void submit_stroke_render_item(topology_type topology, const irender_item* item, bool outer)
        {
            if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 stroke_color = outer ? brush::stroke() : brush::inner_stroke();

            submit_custom_render_item(topology, item, stroke_color);
        }

        //-------------------------------------------------------------------------
        void submit_font_item(const irender_item* item)
        {
            g_ctx.font_renderer->append_drawing_data(topology_type::TRIANGLES, item, brush::fill(), transform_stack::active_world());
        }

        //-------------------------------------------------------------------------
        void clear_color(f32 r, f32 g, f32 b, f32 a)
        {
            brush::push_background_color(glm::vec4(r, g, b, a));

            opengl::api::instance().clear_color(r, g, b, a);
        }

        //-------------------------------------------------------------------------
        void clear(u32 flags)
        {
            opengl::api::instance().clear(flags);
        }
    }
}