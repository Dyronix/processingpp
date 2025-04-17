#include "render/render.h"
#include "render/render_batch_data_table.h"
#include "render/render_instance_data_table.h"
#include "render/render_context.h"
#include "render/render_scissor.h"
#include "render/render_pipeline.h"

#include "render/render_pass_ui.h"
#include "render/render_pass_blit.h"
#include "render/render_pass_clear.h"
#include "render/render_pass_composite_factory.h"
#include "render/render_shader_tags.h"

#include "render/helpers/render_vertex_layouts.h"
#include "render/helpers/render_instance_layouts.h"
#include "render/helpers/render_event_dispatcher.h"

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
#include <array>
#include <functional>

#include "material.h"
#include "render/render_features.h"

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

        using font_batch_data = std::unique_ptr<batch_data_table>;

        //-------------------------------------------------------------------------
        struct context
        {
            // shadows
            bool                        shadows = true;

            // drawing
            render_draw_mode            draw_mode = render_draw_mode::BATCHED;
            render_scissor              scissor = {};

            // shaders
            string::string_id           fill_user_shader = string::string_id::create_invalid();

            // rendering
            render_pipeline             render_pipeline;

            // renderers
            instance_data_hash_map      instance_data = {};
            batch_data_hash_map         batch_data = {};

            font_batch_data             font_batch_data = nullptr;
        } g_ctx;

        //-------------------------------------------------------------------------
        render_context make_render_context(const camera_context* camera_context)
        {
            render_context render_context;
            render_context.camera_context = camera_context;
            render_context.font_batch_data = g_ctx.font_batch_data.get();
            render_context.batch_data = &g_ctx.batch_data;
            render_context.instance_data = &g_ctx.instance_data;
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

            assert(g_ctx.fill_user_shader.is_none() == false);

            string::string_id shader_tag = g_ctx.fill_user_shader;

            if (g_ctx.draw_mode == render_draw_mode::BATCHED)
            {
                shading_model_type shading_model = shader_pool::shading_model_for_shader(shader_tag);
                shading_blending_type shading_blend = shader_pool::shading_blending_for_shader(shader_tag);

                batch_data_key data_key = { shader_tag, shading_model,shading_blend, g_ctx.shadows };

                if (g_ctx.batch_data.find(data_key) == std::cend(g_ctx.batch_data))
                {
                    std::unique_ptr<batch_data_table> data_table = std::make_unique<batch_data_table>(shader_tag);

                    g_ctx.batch_data.emplace(data_key, std::move(data_table));
                }

                g_ctx.batch_data.at(data_key)->append(topology, item, color, transform_stack::active_world());
            }
            else
            {
                shading_model_type shading_model = shader_pool::shading_model_for_shader(shader_tag);
                shading_blending_type shading_blend = shader_pool::shading_blending_for_shader(shader_tag);

                instance_data_key data_key = { shader_tag, shading_model,shading_blend, g_ctx.shadows };

                if (g_ctx.instance_data.find(data_key) == std::cend(g_ctx.instance_data))
                {
                    std::unique_ptr<instance_data_table> data_table = std::make_unique<instance_data_table>(
                        color_world_layout().data(),
                        color_world_layout().size(),
                        shader_tag);

                    g_ctx.instance_data.emplace(data_key, std::move(data_table));
                }

                g_ctx.instance_data.at(data_key)->append(topology, item, color, transform_stack::active_world());
            }
        }

        //-------------------------------------------------------------------------
        static void print_gl_version()
        {
            auto opengl_version = reinterpret_cast<const char*>(opengl::api::instance().get_string_value(GL_VERSION));

            log::info("OpenGL version format: <major_version>.<minor_version>.<release_number> <vendor-specific information>");
            log::info("OpenGL version: {}", opengl_version);
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

            print_gl_version();

#if _DEBUG
            // glDebugMessageCallback is only available on OpenGL 4.3 or higher
            if (has_debugging_capabilities())
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

            g_ctx.font_batch_data = std::make_unique<batch_data_table>(unlit::tags::font::batched());

            // Clear depth only and do a predepth pass
            g_ctx.render_pipeline.add_pass(std::make_unique<clear_pass>(make_depth_clear_state(), framebuffer_pool::tags::composite()));
            g_ctx.render_pipeline.add_pass(create_predepth_composite_pass(unlit::tags::predepth{}, framebuffer_pool::tags::composite()));

            // Calculate things that are in shadow
            g_ctx.render_pipeline.add_pass(memory::make_unique<clear_pass, memory::persistent_graphics_tagged_allocator<clear_pass>>(make_depth_clear_state(), framebuffer_pool::tags::shadow_map(), framebuffer_flags::SAMPLED_DEPTH));
            g_ctx.render_pipeline.add_pass(create_shadow_composite_pass(unlit::tags::shadow{}, framebuffer_pool::tags::shadow_map()));

            // Clear color only and do a geometry pass
            g_ctx.render_pipeline.add_pass(std::make_unique<clear_pass>(make_rtv_clear_state(), framebuffer_pool::tags::composite()));

            g_ctx.render_pipeline.add_insertion_point(insertion_point::BEFORE_UNLIT_OPAQUE);

            g_ctx.render_pipeline.add_pass(create_unlit_composite_pass(unlit::tags::color{}, framebuffer_pool::tags::composite()));
            g_ctx.render_pipeline.add_pass(create_unlit_composite_pass(unlit::tags::texture{}, framebuffer_pool::tags::composite()));

            g_ctx.render_pipeline.add_insertion_point(insertion_point::AFTER_UNLIT_OPAQUE);
            g_ctx.render_pipeline.add_insertion_point(insertion_point::BEFORE_LIT_OPAQUE);

            g_ctx.render_pipeline.add_pass(create_forward_shading_composite_pass(lit::tags::color{}, framebuffer_pool::tags::composite()));
            g_ctx.render_pipeline.add_pass(create_forward_shading_composite_pass(lit::tags::texture{}, framebuffer_pool::tags::composite()));

            g_ctx.render_pipeline.add_insertion_point(insertion_point::AFTER_LIT_OPAQUE);

            // Clear color only and do an ui pass
            g_ctx.render_pipeline.add_pass(std::make_unique<ui_pass>(unlit::tags::font{}, framebuffer_pool::tags::composite()));

            // Blit to backbuffer
            g_ctx.render_pipeline.add_pass(std::make_unique<blit_pass>(framebuffer_pool::tags::composite(), framebuffer_flags::COLOR | framebuffer_flags::DEPTH));

            return true;
        }

        //-------------------------------------------------------------------------
        void terminate()
        {
            // Font
            g_ctx.font_batch_data->clear();

            // Custom
            for (auto& pair : g_ctx.batch_data)
            {
                pair.second->clear();
            }
            g_ctx.batch_data.clear();

            for (auto& pair : g_ctx.instance_data)
            {
                pair.second->clear();
            }
            g_ctx.instance_data.clear();
        }

        //-------------------------------------------------------------------------
        void begin()
        {
            // Font
            g_ctx.font_batch_data->reset();

            // Custom
            for (auto& pair : g_ctx.batch_data)
            {
                pair.second->reset();
            }
            for (auto& pair : g_ctx.instance_data)
            {
                pair.second->reset();
            }

            broadcast_on_draw_begin();
        }

        //-------------------------------------------------------------------------
        void render(const camera_context* context)
        {
            g_ctx.render_pipeline.execute(make_render_context(context));
        }

        //-------------------------------------------------------------------------
        void end()
        {
            broadcast_on_draw_end();
        }

        //-------------------------------------------------------------------------
        void draw_mode(render_draw_mode mode)
        {
            g_ctx.draw_mode = mode;
        }

        //-------------------------------------------------------------------------
        void enable_shadows()
        {
            g_ctx.shadows = true;
        }
        //-------------------------------------------------------------------------
        void disable_shadows()
        {
            g_ctx.shadows = false;
        }

        //-------------------------------------------------------------------------
        bool shadows_enabled()
        {
            return g_ctx.shadows;
        }

        //-------------------------------------------------------------------------
        render_draw_mode draw_mode()
        {
            return g_ctx.draw_mode;
        }

        //-------------------------------------------------------------------------
        void push_active_shader(string::string_id shader_tag, shading_model_type shading_model, shading_blending_type shading_blending)
        {
            g_ctx.fill_user_shader = shader_tag;

            if (g_ctx.render_pipeline.has_pass_with_shader_tag(shader_tag) == false)
            {
                string::string_id               framebuffer_tag = framebuffer_pool::tags::composite();
                u32                             framebuffer_flags = framebuffer_flags::COLOR | framebuffer_flags::DEPTH;
                geometry_render_pass::draw_mode draw_mode = g_ctx.draw_mode == render_draw_mode::BATCHED
                    ? geometry_render_pass::draw_mode::BATCHED
                    : geometry_render_pass::draw_mode::INSTANCED;

                insertion_point                 insertion = insertion_point::AFTER_LIT_OPAQUE;
                std::unique_ptr<irender_pass>   render_pass = nullptr;

                switch (shading_model)
                {
                    case shading_model::UNLIT:
                    {
                        switch (shading_blending)
                        {
                        case shading_blending::OPAQUE:
                            insertion = insertion_point::AFTER_UNLIT_OPAQUE;
                            render_pass = std::make_unique<unlit_pass>(
                                shader_tag,
                                framebuffer_tag,
                                framebuffer_flags,
                                draw_mode);
                            break;
                        case shading_blending::TRANSPARENT:
                            assert(false && "Transparent objects are not supported right now.");
                            break;
                        }
                        break;
                    }
                    case shading_model::LIT:
                    {
                        switch (shading_blending)
                        {
                        case shading_blending::OPAQUE:
                            insertion = insertion_point::AFTER_LIT_OPAQUE;
                            render_pass = std::make_unique<forward_shading_pass>(
                                shader_tag,
                                framebuffer_tag,
                                framebuffer_flags,
                                draw_mode);
                            break;
                        case shading_blending::TRANSPARENT:
                            assert(false && "Transparent objects are not supported right now.");
                            break;
                        }
                        break;
                    }
                }

                g_ctx.render_pipeline.insert_pass(insertion, std::move(render_pass));
            }
        }

        //-------------------------------------------------------------------------
        string::string_id active_shader()
        {
            return g_ctx.fill_user_shader;
        }

        //-------------------------------------------------------------------------
        void push_solid_rendering(bool enable)
        {
            //// Font
            //g_ctx.font_batch_data->enable_solid_rendering(enable);
            //
            //// Custom
            //for (auto& pair : g_ctx.batch_data)
            //{
            //    pair.second->enable_solid_rendering(enable);
            //}
            //for (auto& pair : g_ctx.instance_data)
            //{
            //    pair.second->enable_solid_rendering(enable);
            //}
        }
        //-------------------------------------------------------------------------
        void push_wireframe_rendering(bool enable)
        {
            //// Font
            //g_ctx.font_batch_data->enable_wireframe_rendering(enable);
            //
            // Custom
            //for (auto& pair : g_ctx.batch_data)
            //{
            //    pair.second->enable_wireframe_rendering(enable);
            //}
            //for (auto& pair : g_ctx.instance_data)
            //{
            //    pair.second->enable_wireframe_rendering(enable);
            //}
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
        bool scissor_rect_enabled()
        {
            return g_ctx.scissor.enable;
        }

        //-------------------------------------------------------------------------
        render_scissor scissor_rect()
        {
            return { g_ctx.scissor.x, g_ctx.scissor.y, g_ctx.scissor.width, g_ctx.scissor.height };
        }

        //-------------------------------------------------------------------------
        u32 create_image_item(f32 width, f32 height, s32 channels, const u8* data)
        {
            return create_image_item(width, height, channels, data, image_filter_type::NEAREST, image_wrap_type::REPEAT);
        }

        //-------------------------------------------------------------------------
        u32 create_image_item(f32 width, f32 height, s32 channels, const u8* data, image_filter_type filter_type, image_wrap_type wrap_type)
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
            g_ctx.font_batch_data->append(topology_type::TRIANGLES, item, brush::fill(), transform_stack::active_world());
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