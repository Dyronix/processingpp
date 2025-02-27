#include "render/render.h"
#include "render/render_batch.h"
#include "render/render_instance.h"
#include "render/render_batch_renderer.h"
#include "render/render_instance_renderer.h"

#include "render/helpers/render_vertex_layouts.h"
#include "render/helpers/render_instance_layouts.h"
#include "render/helpers/render_event_dispatcher.h"

#include "render/opengl/render_gl_error.h"

#include "resources/shader_pool.h"
#include "resources/framebuffer_pool.h"

#include "memory/memory_placement_new.h"
#include "memory/memory_unique_ptr_util.h"

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

#define LOG_GL_NOTIFICATIONS 0
#define LOG_GL_LOW 0
#define LOG_GL_MEDIUM 1

namespace ppp
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
            static debug_scratch_string             source_str = "UNKNOWN";
            static debug_scratch_string             type_str = "UNKNOWN";
            static debug_scratch_string             severity_str = "UNKNOWN";

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

    namespace render
    {
        using instance_renderers_hash_map = graphics_hash_map<string::string_id, graphics_unique_ptr<instance_renderer>>;
        using batch_renderers_hash_map = graphics_hash_map<string::string_id, graphics_unique_ptr<batch_renderer>>;

        using font_renderer = graphics_unique_ptr<texture_batch_renderer>;

        //-------------------------------------------------------------------------
        class geometry_builder
        {
        public:
            void activate(string::string_id tag)
            {
                m_is_active = true;
                m_shader_tag = tag;
            }

            void deactivate()
            {
                m_is_active = false;
                m_shader_tag = string::string_id::create_invalid();
            }

            bool is_active() const
            {
                return m_is_active;
            }

            string::string_id shader_tag() const
            {
                return m_shader_tag;
            }

        private:
            bool m_is_active = false;
            string::string_id m_shader_tag = string::string_id::create_invalid();
        };

        geometry_builder _geometry_builder;

        //-------------------------------------------------------------------------
        struct render_scissor
        {
            s32     x = -1;
            s32     y = -1;
            s32     width = -1;
            s32     height = -1;

            bool    enable = false;
        };

        //-------------------------------------------------------------------------
        struct context
        {
            // drawing
            render_draw_mode            draw_mode = render_draw_mode::BATCHED;
            render_scissor              scissor = {};

            // shaders
            string::string_id           fill_user_shader = string::string_id::create_invalid();
            string::string_id           stroke_user_shader = string::string_id::create_invalid();

            vertex_type                 fill_user_vertex_type = vertex_type::POSITION_TEXCOORD_NORMAL_COLOR;

            // frame buffer
            string::string_id           main_framebuffer_tag = string::string_id::create_invalid();

            // renderers
            instance_renderers_hash_map instance_renderers = {};
            batch_renderers_hash_map    batch_renderers = {};

            font_renderer               font_renderer = nullptr;

            // opengl version       
            s32                         major = 0;
            s32                         minor = 0;
        } g_ctx;

        //-------------------------------------------------------------------------
        void submit_custom_render_item(topology_type topology, const irender_item* item, const glm::vec4& color)
        {
            if (brush::stroke_enabled() == false && brush::inner_stroke_enabled() == false && brush::fill_enabled() == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            string::string_id shader_tag = g_ctx.fill_user_shader.is_none() ? _geometry_builder.shader_tag() : g_ctx.fill_user_shader;

            if (g_ctx.draw_mode == render_draw_mode::BATCHED)
            {
                auto& batch_ren = g_ctx.batch_renderers;

                if (g_ctx.batch_renderers.find(shader_tag) == std::cend(g_ctx.batch_renderers))
                {
                    graphics_unique_ptr<batch_renderer> renderer = nullptr;

                    if (item->has_textures() == false)
                    {
                        renderer = memory::make_unique<primitive_batch_renderer, memory::persistent_graphics_tagged_allocator<primitive_batch_renderer>>(
                            g_ctx.fill_user_shader.is_none() ? pos_col_layout().data() : fill_user_layout(g_ctx.fill_user_vertex_type),
                            g_ctx.fill_user_shader.is_none() ? pos_col_layout().size() : fill_user_layout_count(g_ctx.fill_user_vertex_type),
                            shader_tag);
                    }
                    else
                    {
                        renderer = memory::make_unique<texture_batch_renderer, memory::persistent_graphics_tagged_allocator<texture_batch_renderer>>(
                            g_ctx.fill_user_shader.is_none() ? pos_tex_col_layout().data() : fill_user_layout(g_ctx.fill_user_vertex_type),
                            g_ctx.fill_user_shader.is_none() ? pos_tex_col_layout().size() : fill_user_layout_count(g_ctx.fill_user_vertex_type),
                            shader_tag);
                    }

                    if (_geometry_builder.is_active())
                    {
                        renderer->buffer_policy(render_buffer_policy::RETAINED);
                    }

                    renderer->draw_policy(render_draw_policy::CUSTOM);
                    g_ctx.batch_renderers.emplace(shader_tag, std::move(renderer));
                }

                g_ctx.batch_renderers.at(shader_tag)->append_drawing_data(topology, item, color, transform_stack::active_world());
            }
            else
            {
                if (g_ctx.instance_renderers.find(shader_tag) == std::cend(g_ctx.instance_renderers))
                {
                    graphics_unique_ptr<instance_renderer> renderer = nullptr;
                    if (item->has_textures() == false)
                    {
                        renderer = memory::make_unique<primitive_instance_renderer, memory::persistent_graphics_tagged_allocator<primitive_instance_renderer>>(
                            g_ctx.fill_user_shader.is_none() ? pos_layout().data() : fill_user_layout(g_ctx.fill_user_vertex_type),
                            g_ctx.fill_user_shader.is_none() ? pos_layout().size() : fill_user_layout_count(g_ctx.fill_user_vertex_type),
                            color_world_layout().data(),
                            color_world_layout().size(),
                            shader_tag);
                    }
                    else
                    {
                        renderer = memory::make_unique<texture_instance_renderer, memory::persistent_graphics_tagged_allocator<texture_instance_renderer>>(
                            g_ctx.fill_user_shader.is_none() ? pos_tex_layout().data() : fill_user_layout(g_ctx.fill_user_vertex_type),
                            g_ctx.fill_user_shader.is_none() ? pos_tex_layout().size() : fill_user_layout_count(g_ctx.fill_user_vertex_type),
                            color_world_matid_layout().data(),
                            color_world_matid_layout().size(),
                            shader_tag);
                    }

                    if (_geometry_builder.is_active())
                    {
                        renderer->buffer_policy(render_buffer_policy::RETAINED);
                    }

                    renderer->draw_policy(render_draw_policy::CUSTOM);

                    g_ctx.instance_renderers.emplace(shader_tag, std::move(renderer));
                }

                g_ctx.instance_renderers.at(shader_tag)->append_drawing_data(topology, item, color, transform_stack::active_world());
            }
        }

        //-------------------------------------------------------------------------
        void parse_gl_version(const char* version_string)
        {
            temp_stringstream ss(version_string);
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

            GL_CALL(auto opengl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION)));

            parse_gl_version(opengl_version);
            print_gl_version(opengl_version);

#if _DEBUG
            // glDebugMessageCallback is only available on OpenGL 4.3 or higher
            if (g_ctx.major >= 4 && g_ctx.minor >= 3)
            {
                s32 flags = 0; 
                GL_CALL(glGetIntegerv(GL_CONTEXT_FLAGS, &flags));

                if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
                {
                    GL_CALL(glEnable(GL_DEBUG_OUTPUT));
                    GL_CALL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
                    GL_CALL(glDebugMessageCallback(opengl::debug_message_callback, nullptr));
                    GL_CALL(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE));
                }
            }
#endif

            g_ctx.scissor.x = 0;
            g_ctx.scissor.y = 0;
            g_ctx.scissor.width = w;
            g_ctx.scissor.height = h;
            g_ctx.scissor.enable = false;

            g_ctx.font_renderer = memory::make_unique<texture_batch_renderer, memory::persistent_graphics_tagged_allocator<texture_batch_renderer>>(
                pos_tex_col_layout().data(),
                pos_tex_col_layout().size(),
                shader_pool::tags::unlit_font());

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            // Release the framebuffer
            framebuffer_pool::release({ g_ctx.main_framebuffer_tag, true});

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
        void begin()
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

            auto framebuffer = framebuffer_pool::bind({ g_ctx.main_framebuffer_tag, true });

            // Clear the background to black
            GL_CALL(glViewport(0, 0, framebuffer->width(), framebuffer->height()));

            GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
            GL_CALL(glClearDepth(1.0));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            // Reset to the user clear color
            glm::vec4 bg_color = brush::background();
            GL_CALL(glClearColor(bg_color.r, bg_color.g, bg_color.b, bg_color.a));

            if (g_ctx.scissor.enable)
            {
                GL_CALL(glEnable(GL_SCISSOR_TEST));
                GL_CALL(glScissor(
                    std::clamp(g_ctx.scissor.x, 0, framebuffer->width()),
                    std::clamp(g_ctx.scissor.y, 0, framebuffer->height()),
                    std::clamp(g_ctx.scissor.width, framebuffer::min_framebuffer_width(), framebuffer->width()),
                    std::clamp(g_ctx.scissor.height, framebuffer::min_framebuffer_height(), framebuffer->height())));
            }
            else
            {
                GL_CALL(glDisable(GL_SCISSOR_TEST));
            }

            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

            broadcast_on_draw_begin();
        }

        //-------------------------------------------------------------------------
        void render(const render_context& context)
        {
            auto system_framebuffer = framebuffer_pool::get_system();
            auto target_framebuffer = framebuffer_pool::get({ g_ctx.main_framebuffer_tag, true });

            f32 w = static_cast<f32>(target_framebuffer->width());
            f32 h = static_cast<f32>(target_framebuffer->height());

            GL_CALL(glDisable(GL_BLEND));
            GL_CALL(glEnable(GL_CULL_FACE));
            GL_CALL(glEnable(GL_DEPTH_TEST));

            GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            GL_CALL(glCullFace(GL_BACK));
            GL_CALL(glDepthFunc(GL_LESS));

            const glm::mat4& cam_font_p = context.mat_proj_font;
            const glm::mat4& cam_font_v = context.mat_view_font;
            glm::mat4 cam_font_vp = cam_font_p * cam_font_v;

            g_ctx.font_renderer->render(cam_font_vp);

            const glm::mat4& cam_active_p = context.mat_proj_active;
            const glm::mat4& cam_active_v = context.mat_view_active;
            glm::mat4 cam_active_vp = cam_active_p * cam_active_v;

            for (auto& pair : g_ctx.batch_renderers)
            {
                pair.second->render(cam_active_vp);
            }

            for (auto& pair : g_ctx.instance_renderers)
            {
                pair.second->render(cam_active_vp);
            }

            target_framebuffer->bind(framebuffer_bound_target::READ);
            system_framebuffer->bind(framebuffer_bound_target::WRITE);

            GL_CALL(glBlitFramebuffer(
                0, 
                0, 
                target_framebuffer->width(), 
                target_framebuffer->height(), 
                
                0, 
                0, 
                system_framebuffer->width(), 
                system_framebuffer->height(), 
                
                GL_COLOR_BUFFER_BIT, 
                GL_NEAREST));
            
            system_framebuffer->bind();
        }

        //-------------------------------------------------------------------------
        void end()
        {
            broadcast_on_draw_end();

            framebuffer_pool::unbind({ g_ctx.main_framebuffer_tag, true });
        }

        //-------------------------------------------------------------------------
        void draw_mode(render_draw_mode mode)
        {
            g_ctx.draw_mode = mode;
        }

        //-------------------------------------------------------------------------
        render_draw_mode draw_mode()
        {
            return g_ctx.draw_mode;
        }

        //-------------------------------------------------------------------------
        void push_active_shader(string::string_id tag, vertex_type type)
        {
            g_ctx.fill_user_shader = tag;
            g_ctx.fill_user_vertex_type = type;
        }

        //-------------------------------------------------------------------------
        string::string_id active_shader()
        {
            return g_ctx.fill_user_shader;
        }

        //-------------------------------------------------------------------------
        void begin_geometry_builder(string::string_id tag)
        {
            assert(!_geometry_builder.is_active() && "Construction of previous shape has not been finished, call end_geometry_builder first");

            _geometry_builder.activate(tag);
        }

        //-------------------------------------------------------------------------
        void end_geometry_builder()
        {
            _geometry_builder.deactivate();
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
            submit_custom_render_item(topology, item, item->has_textures() ? brush::tint() : brush::fill());
        }

        //-------------------------------------------------------------------------
        void submit_stroke_render_item(topology_type topology, const irender_item* item, bool outer)
        {
            if (_geometry_builder.is_active())
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

            GL_CALL(glClearColor(r, g, b, a));
        }

        //-------------------------------------------------------------------------
        void clear(u32 flags)
        {
            GL_CALL(glClear(flags));
        }
    }
}