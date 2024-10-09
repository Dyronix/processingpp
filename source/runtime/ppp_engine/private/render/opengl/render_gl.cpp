#include "render/render.h"
#include "render/render_transform.h"
#include "render/opengl/render_gl_batch.h"
#include "resources/shader_pool.h"
#include "util/log.h"

#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <vector>

namespace ppp
{
    namespace render
    {
        namespace internal
        {
            struct camera
            {
                glm::vec3 eye;
                glm::vec3 center;
                glm::vec3 up;

                glm::mat4 proj;
            };

            camera _active_camera = {};

            u32 index_type()
            {
                if (typeid(Index).hash_code() == typeid(u32).hash_code()) return GL_UNSIGNED_INT;
                if (typeid(Index).hash_code() == typeid(u16).hash_code()) return GL_UNSIGNED_SHORT;

                log::error("Invalid index type specified: {}, using UNSIGNED_INT", typeid(Index).name());
                return GL_UNSIGNED_INT;
            }

            u32 convert_color(const glm::vec4 color)
            {
                u32 out;
                out = (conversions::f32_to_uint8(color.x)) << 24;
                out |= (conversions::f32_to_uint8(color.y)) << 16;
                out |= (conversions::f32_to_uint8(color.z)) << 8;
                out |= (conversions::f32_to_uint8(color.w)) << 0;
                return out;
            }

            glm::vec4 convert_color(u32 color)
            {
                f32 scale = 1.0f / 255.0f;
                return glm::vec4(
                    ((color >> 24) & 0xFF) * scale,
                    ((color >> 16) & 0xFF) * scale,
                    ((color >> 8) & 0xFF) * scale,
                    ((color >> 0) & 0xFF) * scale);
            }

            bool _solid_rendering = true;
            bool _wireframe_rendering = true;
            f32 _wireframe_linewidth = 3.0f;
            s32 _wireframe_color = 0x000000FF;

            s32 _scissor_x = -1;
            s32 _scissor_y = -1;
            s32 _scissor_width = -1;
            s32 _scissor_height = -1;
            bool _scissor_enable = false;

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

            struct point_vertex_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };
            struct line_vertex_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };
            struct triangle_vertex_format
            {
                glm::vec3 position;
                glm::vec4 color;
            };
            struct image_vertex_format
            {
                glm::vec3 position;
                glm::vec2 texcoord;
                glm::vec4 color;
                f32       texture_idx;
            };

            constexpr s32 _max_points = 9'000;
            constexpr s32 _max_lines = 9'000;
            constexpr s32 _max_triangles = 9'000;
            constexpr s32 _max_texture_units = 8;

            constexpr s32 _min_frame_buffer_width = 32;
            constexpr s32 _min_frame_buffer_height = 32;

            s32 _frame_buffer_width = -1;
            s32 _frame_buffer_height = -1;

            u32 _render_fbo;
            u32 _render_depth_rbo;
            u32 _render_texture;

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

            std::unique_ptr<PrimitiveDrawingData<point_vertex_format>> _points_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<point_vertex_format>> _points_stroke_drawing_data;

            std::unique_ptr<PrimitiveDrawingData<line_vertex_format>> _lines_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<line_vertex_format>> _lines_stroke_drawing_data;

            std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>> _triangle_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>> _triangle_stroke_drawing_data;

            std::unique_ptr<TextureDrawingData<image_vertex_format>> _image_drawing_data;
            std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>> _image_stroke_drawing_data;

            std::unique_ptr<TextureDrawingData<image_vertex_format>> _font_drawing_data;

            void draw_points(const std::unique_ptr<PrimitiveDrawingData<point_vertex_format>>& point_drawing_data)
            {
                auto points_batch = point_drawing_data->next_batch();
                if (points_batch != nullptr)
                {
                    glBindVertexArray(point_drawing_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, point_drawing_data->vbo());

                    while (points_batch != nullptr)
                    {
                        glBufferSubData(GL_ARRAY_BUFFER, 0, points_batch->vertex_buffer_byte_size(), points_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, points_batch->index_buffer_byte_size(), points_batch->indices());

                        glDrawElements(GL_POINTS, points_batch->active_index_count(), internal::index_type(), nullptr);

                        points_batch = point_drawing_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
            void draw_lines(const std::unique_ptr<PrimitiveDrawingData<line_vertex_format>>& line_drawing_data)
            {
                auto lines_batch = line_drawing_data->next_batch();
                if (lines_batch != nullptr)
                {
                    glBindVertexArray(line_drawing_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, line_drawing_data->vbo());

                    while (lines_batch != nullptr)
                    {
                        #ifndef NDEBUG
                        if (lines_batch->active_index_count() % 2 != 0)
                        {
                            log::error("Trying to render invalid number of lines: {}", lines_batch->active_index_count());
                            return;
                        }
                        #endif

                        glBufferSubData(GL_ARRAY_BUFFER, 0, lines_batch->vertex_buffer_byte_size(), lines_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, lines_batch->index_buffer_byte_size(), lines_batch->indices());

                        glDrawElements(GL_LINES, lines_batch->active_index_count(), internal::index_type(), nullptr);

                        lines_batch = line_drawing_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
            void draw_triangles(const std::unique_ptr<PrimitiveDrawingData<triangle_vertex_format>>& triangle_drawing_data)
            {
                auto triangle_batch = triangle_drawing_data->next_batch();
                if (triangle_batch != nullptr)
                {
                    glBindVertexArray(triangle_drawing_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, triangle_drawing_data->vbo());

                    while (triangle_batch != nullptr)
                    {
                        #ifndef NDEBUG
                        if (triangle_batch->active_index_count() % 3 != 0)
                        {
                            log::error("Trying to render invalid number of triangles: {}", triangle_batch->active_index_count());
                            return;
                        }
                        #endif

                        glBufferSubData(GL_ARRAY_BUFFER, 0, triangle_batch->vertex_buffer_byte_size(), triangle_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, triangle_batch->index_buffer_byte_size(), triangle_batch->indices());

                        glDrawElements(GL_TRIANGLES, triangle_batch->active_index_count(), internal::index_type(), nullptr);

                        triangle_batch = triangle_drawing_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }
            void draw_images(const std::unique_ptr<TextureDrawingData<image_vertex_format>>& image_draw_data, s32 shader_program)
            {
                auto image_batch = image_draw_data->next_batch();
                if (image_batch != nullptr)
                {
                    glBindVertexArray(image_draw_data->vao());
                    glBindBuffer(GL_ARRAY_BUFFER, image_draw_data->vbo());

                    while (image_batch != nullptr)
                    {
                        u32 u_tex_samplers_loc = glGetUniformLocation(shader_program, "s_image");
                        glUniform1iv(u_tex_samplers_loc, image_batch->active_texture_count(), image_batch->samplers());

                        #ifndef NDEBUG
                        if (image_batch->active_index_count() % 3 != 0)
                        {
                            log::error("Trying to render invalid number of triangles: {}", image_batch->active_index_count());
                            return;
                        }
                        #endif


                        s32 i = 0;
                        s32 offset = GL_TEXTURE1 - GL_TEXTURE0;
                        for (const auto& pair : image_batch->image_ids())
                        {
                            u32 id = pair.first;
                            s32 idx = pair.second;

                            glActiveTexture(GL_TEXTURE0 + (offset * i));
                            glBindTexture(GL_TEXTURE_2D, id);

                            ++i;
                        }

                        glBufferSubData(GL_ARRAY_BUFFER, 0, image_batch->vertex_buffer_byte_size(), image_batch->vertices());
                        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, image_batch->index_buffer_byte_size(), image_batch->indices());

                        glDrawElements(GL_TRIANGLES, image_batch->active_index_count(), internal::index_type(), nullptr);

                        image_batch = image_draw_data->next_batch();
                    }

                    glBindBuffer(GL_ARRAY_BUFFER, 0);
                    glBindVertexArray(0);
                }
            }

            u32 _white_texture_image_id;
            u32 _black_texture_image_id;
        }

        bool initialize(s32 w, s32 h, void* user_data)
        {
            // glad: load all OpenGL function pointers```
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

            internal::create_frame_buffer();

            // Primitive Drawing Data
            s32 max_vertex_elements = 0;
            s32 max_index_elements = 0;
            glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertex_elements);
            glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_index_elements);

            s32 max_point_vertices = std::min(max_vertex_elements, internal::_max_points);
            s32 max_point_indices = std::min(max_index_elements, internal::_max_points * 1);
            s32 max_line_vertices = std::min(max_vertex_elements, internal::_max_lines);
            s32 max_line_indices = std::min(max_index_elements, internal::_max_lines * 2);
            s32 max_triangle_vertices = std::min(max_vertex_elements, internal::_max_triangles);
            s32 max_triangle_indices = std::min(max_index_elements, internal::_max_triangles * 3);

            internal::_points_drawing_data = std::make_unique<PrimitiveDrawingData<internal::point_vertex_format>>(max_point_vertices, max_point_indices);
            internal::_points_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::point_vertex_format>>(max_point_vertices, max_point_indices);
            internal::_lines_drawing_data = std::make_unique<PrimitiveDrawingData<internal::line_vertex_format>>(max_line_vertices, max_line_indices);
            internal::_lines_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::line_vertex_format>>(max_line_vertices, max_line_indices);
            internal::_triangle_drawing_data = std::make_unique<PrimitiveDrawingData<internal::triangle_vertex_format>>(max_triangle_vertices, max_triangle_indices);
            internal::_triangle_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::triangle_vertex_format>>(max_triangle_vertices, max_triangle_indices);

            s32 max_texture_units = 0;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_units);
            
            s32 max_images = std::min(max_texture_units, internal::_max_texture_units);
            s32 size_vertex_buffer = max_images * 4;
            s32 size_index_buffer = max_images * 4 * 3;

            // Image Drawing Data
            internal::_image_drawing_data = std::make_unique<TextureDrawingData<internal::image_vertex_format>>(max_images);
            internal::_image_stroke_drawing_data = std::make_unique<PrimitiveDrawingData<internal::triangle_vertex_format>>(size_vertex_buffer, size_index_buffer);

            s32 white = 0xFFFFFFFF;
            internal::_white_texture_image_id = create_image_item(1, 1, 4, (u8*)&white);
            s32 black = 0xFF000000;
            internal::_black_texture_image_id = create_image_item(1, 1, 4, (u8*)&black);

            // Font Drawing Data
            internal::_font_drawing_data = std::make_unique<TextureDrawingData<internal::image_vertex_format>>(max_images);

            return true;
        }

        void terminate()
        {
            internal::_font_drawing_data->release();
            internal::_image_drawing_data->release();
            internal::_image_stroke_drawing_data->release();
            internal::_triangle_drawing_data->release();
            internal::_triangle_stroke_drawing_data->release();
            internal::_lines_drawing_data->release();
            internal::_lines_stroke_drawing_data->release();
            internal::_points_drawing_data->release();
            internal::_points_stroke_drawing_data->release();
        }

        void begin()
        {
            internal::_points_drawing_data->reset();
            internal::_points_stroke_drawing_data->reset();
            internal::_lines_drawing_data->reset();
            internal::_lines_stroke_drawing_data->reset();
            internal::_triangle_drawing_data->reset();
            internal::_triangle_stroke_drawing_data->reset();
            internal::_image_drawing_data->reset();
            internal::_image_stroke_drawing_data->reset();
            internal::_font_drawing_data->reset();

            glBindFramebuffer(GL_FRAMEBUFFER, internal::_render_fbo);

            // Clear the background to black
            glViewport(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClearDepth(1.0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Reset to the user clear color
            glm::vec4 bg_color = internal::convert_color(internal::_bg_color);
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

        void render()
        {
            shader_program unlit_color_shader_program = shader_pool::get_shader_program(shader_pool::tags::unlit_color);
            shader_program unlit_texture_shader_program = shader_pool::get_shader_program(shader_pool::tags::unlit_texture);
            shader_program unlit_font_shader_program = shader_pool::get_shader_program(shader_pool::tags::unlit_font);

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

            if(internal::_font_drawing_data->batch_count() > 0)
            {
                glUseProgram(unlit_font_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(unlit_font_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                // We draw all images first this 
                // An effect of this will be that filled shapes are drawn on top of images ( a z-index might be in order here )
                // This however will make sure inner-stroke is possible
                if (internal::_solid_rendering)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glUniform1i(glGetUniformLocation(unlit_font_shader_program, "u_wireframe"), GL_FALSE);

                    internal::draw_images(internal::_font_drawing_data, unlit_font_shader_program);
                }
                
                if (internal::_wireframe_rendering)
                {
                    internal::_font_drawing_data->load_first_batch();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glLineWidth(internal::_wireframe_linewidth);
                    glUniform1i(glGetUniformLocation(unlit_font_shader_program, "u_wireframe"), GL_TRUE);
                    glm::vec4 wireframe_color = internal::convert_color(internal::_wireframe_color);
                    glUniform4fv(glGetUniformLocation(unlit_font_shader_program, "u_wireframe_color"), 1, &wireframe_color[0]);

                    internal::draw_images(internal::_font_drawing_data, unlit_font_shader_program);
                }

                glUseProgram(0);
            }

            if (internal::_image_drawing_data->batch_count() > 0)
            {
                glUseProgram(unlit_texture_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(unlit_texture_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                // We draw all images first this 
                // An effect of this will be that filled shapes are drawn on top of images ( a z-index might be in order here )
                // This however will make sure inner-stroke is possible
                if (internal::_solid_rendering)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glUniform1i(glGetUniformLocation(unlit_texture_shader_program, "u_wireframe"), GL_FALSE);

                    internal::draw_images(internal::_image_drawing_data, unlit_texture_shader_program);
                }

                if(internal::_wireframe_rendering)
                {
                    internal::_image_drawing_data->load_first_batch();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glLineWidth(internal::_wireframe_linewidth);
                    glUniform1i(glGetUniformLocation(unlit_texture_shader_program, "u_wireframe"), GL_TRUE);
                    glm::vec4 wireframe_color = internal::convert_color(internal::_wireframe_color);
                    glUniform4fv(glGetUniformLocation(unlit_texture_shader_program, "u_wireframe_color"), 1, &wireframe_color[0]);

                    internal::draw_images(internal::_image_drawing_data, unlit_texture_shader_program);
                }

                glUseProgram(0);
            }

            if (internal::_points_drawing_data->batch_count() > 0
                || internal::_lines_drawing_data->batch_count() > 0
                || internal::_triangle_drawing_data->batch_count() > 0)
            {
                glUseProgram(unlit_color_shader_program);
                u32 u_mpv_loc = glGetUniformLocation(unlit_color_shader_program, "u_worldviewproj");
                glUniformMatrix4fv(u_mpv_loc, 1, false, value_ptr(vp));

                if (internal::_solid_rendering)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glUniform1i(glGetUniformLocation(unlit_color_shader_program, "u_wireframe"), GL_FALSE);

                    // Points
                    internal::draw_points(internal::_points_drawing_data);
                    internal::draw_points(internal::_points_stroke_drawing_data);

                    // Lines
                    internal::draw_lines(internal::_lines_drawing_data);
                    internal::draw_lines(internal::_lines_stroke_drawing_data);

                    // Triangles
                    // We draw the triangles first and afterwards the stroke
                    // This only has to happen for the triangles because of the inner stroke capabilities
                    // If we would draw the triangles last no inner stroke would be visible
                    internal::draw_triangles(internal::_triangle_drawing_data);
                    internal::draw_triangles(internal::_triangle_stroke_drawing_data);

                    internal::draw_triangles(internal::_image_stroke_drawing_data);
                }

                if (internal::_wireframe_rendering)
                {
                    internal::_triangle_drawing_data->load_first_batch();
                    internal::_triangle_stroke_drawing_data->load_first_batch();

                    internal::_image_stroke_drawing_data->load_first_batch();

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glLineWidth(internal::_wireframe_linewidth);
                    glUniform1i(glGetUniformLocation(unlit_color_shader_program, "u_wireframe"), GL_TRUE);
                    glm::vec4 wireframe_color = internal::convert_color(internal::_wireframe_color);
                    glUniform4fv(glGetUniformLocation(unlit_color_shader_program, "u_wireframe_color"), 1, &wireframe_color[0]);

                    internal::draw_triangles(internal::_triangle_drawing_data);
                    internal::draw_triangles(internal::_triangle_stroke_drawing_data);

                    internal::draw_triangles(internal::_image_stroke_drawing_data);
                }

                glUseProgram(0);
            }

            glBindFramebuffer(GL_READ_FRAMEBUFFER, internal::_render_fbo);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, 0, 0, internal::_frame_buffer_width, internal::_frame_buffer_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void end()
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void push_active_camera(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up, const glm::mat4& proj)
        {
            internal::_active_camera.eye = eye;
            internal::_active_camera.center = center;
            internal::_active_camera.up = up;

            internal::_active_camera.proj = proj;
        }

        void push_solid_rendering(bool enable)
        {
            internal::_solid_rendering = enable;
        }

        void push_wireframe_rendering(bool enable)
        {
            internal::_wireframe_rendering = enable;
        }

        void push_wireframe_linewidth(f32 line_width)
        {
            internal::_wireframe_linewidth = line_width;
        }

        void push_wireframe_color(const glm::vec4& color)
        {
            internal::_wireframe_color = internal::convert_color(color);
        }

        void push_fill_color(const glm::vec4& color)
        {
            internal::_fill_color = internal::convert_color(color);
        }

        void push_fill_enable(bool enable)
        {
            internal::_fill_enable = enable;
        }

        void push_stroke_width(f32 w)
        {
            internal::_stroke_width = w;
        }

        void push_stroke_color(const glm::vec4& color)
        {
            internal::_stroke_color = internal::convert_color(color);
        }

        void push_stroke_enable(bool enable)
        {
            internal::_stroke_enable = enable;
        }

        void push_inner_stroke_width(f32 w)
        {
            internal::_inner_stroke_width = w;
        }

        void push_inner_stroke_color(const glm::vec4& color)
        {
            internal::_inner_stroke_color = internal::convert_color(color);
        }

        void push_inner_stroke_enable(bool enable)
        {
            internal::_inner_stroke_enable = enable;
        }

        void push_tint_color(const glm::vec4& color)
        {
            internal::_tint_color = internal::convert_color(color);
        }

        void push_tint_enable(bool enable)
        {
            internal::_tint_enable = enable;
        }

        void push_scissor(s32 x, s32 y, s32 width, s32 height)
        {
            internal::_scissor_x = x;
            internal::_scissor_y = y;
            internal::_scissor_width = width;
            internal::_scissor_height = height;
        }

        void push_scissor_enable(bool enable)
        {
            internal::_scissor_enable = enable;
        }

        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data)
        {
            return create_image_item(width, height, channels, data, ImageFilterType::NEAREST, ImageWrapType::REPEAT);
        }

        u32 create_image_item(f32 width, f32 height, s32 channels, u8* data, ImageFilterType filter_type, ImageWrapType wrap_type)
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
                case ImageFilterType::NEAREST: filter = GL_NEAREST; break;
                case ImageFilterType::LINEAR: filter = GL_LINEAR; break;
                default:
                    assert(false);
            }

            GLint wrap = GL_INVALID_VALUE;

            switch(wrap_type)
            {
                case ImageWrapType::CLAMP_TO_EDGE: wrap = GL_CLAMP_TO_EDGE; break;
                case ImageWrapType::REPEAT: wrap = GL_REPEAT; break;
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

        void submit_render_item(TopologyType topology, const render_item& item)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 fill_color = internal::convert_color(internal::_fill_enable ? internal::_fill_color : internal::_bg_color);

            switch (topology)
            {
            case TopologyType::POINTS:
                internal::_points_drawing_data->append(item, fill_color, transform::active_world());
                break;
            case TopologyType::LINES:
                internal::_lines_drawing_data->append(item, fill_color, transform::active_world());
                break;
            case TopologyType::TRIANGLES:
                internal::_triangle_drawing_data->append(item, fill_color, transform::active_world());
                break;

            default:
                log::critical("Invalid topology type!");
                break;
            }
        }

        void submit_stroke_render_item(TopologyType topology, const render_item& item, bool outer)
        {
            if (internal::_stroke_enable == false && internal::_fill_enable == false)
            {
                // When there is no "stroke" and there is no "fill" the object would be invisible.
                // So we don't add anything to the drawing list.
                return;
            }

            glm::vec4 stroke_color = outer ? internal::convert_color(internal::_stroke_color) : internal::convert_color(internal::_inner_stroke_color);

            switch (topology)
            {
            case TopologyType::POINTS:
                internal::_points_stroke_drawing_data->append(item, stroke_color, transform::active_world());
                break;
            case TopologyType::LINES:
                internal::_lines_stroke_drawing_data->append(item, stroke_color, transform::active_world());
                break;
            case TopologyType::TRIANGLES:
                internal::_triangle_stroke_drawing_data->append(item, stroke_color, transform::active_world());
                break;

            default:
                log::critical("Invalid topology type!");
                break;
            }
        }

        void submit_font_item(const render_item& item)
        {
            glm::vec4 fill_color = internal::convert_color(internal::_fill_color);

            internal::_font_drawing_data->append(item, fill_color, transform::active_world());
        }

        void submit_image_item(const render_item& item)
        {
            glm::vec4 tint_color = internal::convert_color(internal::_tint_color);

            internal::_image_drawing_data->append(item, tint_color, transform::active_world());
        }

        void submit_stroke_image_item(const render_item& item, bool outer)
        {
            glm::vec4 stroke_color = outer ? internal::convert_color(internal::_stroke_color) : internal::convert_color(internal::_inner_stroke_color);

            internal::_image_stroke_drawing_data->append(item, stroke_color, transform::active_world());
        }

        void clear_color(f32 r, f32 g, f32 b, f32 a)
        {
            internal::_bg_color = internal::convert_color(glm::vec4(r, g, b, a));
            glClearColor(r, g, b, a);
        }

        void clear(u32 flags)
        {           
            glClear(flags);
        }

        bool fill_enabled()
        {
            return internal::_fill_enable;
        }
        
        bool stroke_enabled()
        {
            return internal::_stroke_enable;
        }

        bool inner_stroke_enabled()
        {
            return internal::_inner_stroke_enable;
        }

        f32 stroke_width()
        {
            return internal::_stroke_width;
        }

        f32 inner_stroke_width()
        {
            return internal::_inner_stroke_width;
        }
        
        bool tint_enabled()
        {
            return internal::_tint_enable;
        }

        bool scissor_enabled()
        {
            return internal::_scissor_enable;
        }

        glm::vec4 fill()
        {
            return internal::convert_color(internal::_fill_color);
        }
        
        glm::vec4 stroke()
        {
            return internal::convert_color(internal::_stroke_color);
        }

        glm::vec4 inner_stroke()
        {
            return internal::convert_color(internal::_inner_stroke_color);
        }
        
        glm::vec4 tint()
        {
            return internal::convert_color(internal::_tint_color);
        }
        
        ScissorRect scissor()
        {
            return { internal::_scissor_x, internal::_scissor_y, internal::_scissor_width, internal::_scissor_height };
        }
    }
}