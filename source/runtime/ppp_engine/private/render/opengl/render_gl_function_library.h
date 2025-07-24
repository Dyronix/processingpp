#pragma once

#include "util/types.h"

namespace ppp
{
    namespace render
    {
        namespace opengl
        {
            class ifunction_library
            {
            public:
                virtual ~ifunction_library() = default;

                virtual void viewport(s32 x, s32 y, s32 width, s32 height) = 0;
                virtual void scissor(s32 x, s32 y, s64 width, s64 height) = 0;
                virtual void read_pixels(s32 x, s32 y, s32 width, s32 height, u32 format, u32 type, void* data) = 0;

                virtual void clear_color(f32 red, f32 green, f32 blue, f32 alpha) = 0;
                virtual void clear_depth(f32 depth) = 0;
                virtual void clear(u32 bitfield) = 0;

                virtual void enable(u32 value) = 0;
                virtual void disable(u32 value) = 0;

                virtual void depth_mask(bool flag) = 0;
                virtual void color_mask(bool red, bool green, bool blue, bool alpha) = 0;

                virtual void pixel_store_i(u32 pname, s32 param) = 0;

                virtual void blend_func(u32 sfactor, u32 dfactor) = 0;
                virtual void cull_face(u32 mode) = 0;
                virtual void front_face(u32 mode) = 0;
                virtual void depth_func(u32 fn) = 0;
                virtual void polygon_mode(u32 face, u32 mode) = 0;
                virtual void line_width(f32 width) = 0;

                // Vertex Arrays
                virtual void enable_vertex_attrib_array(u32 index) = 0;
                virtual void vertex_attrib_i_pointer(u32 index, s32 size, u32 type, u64 stride, const void* ptr) = 0;
                virtual void vertex_attrib_pointer(u32 index, s32 size, u32 type, bool normalized, u64 stride, const void* ptr) = 0;
                virtual void delete_vertex_arrays(u64 count, const u32* arrays) = 0;
                virtual void generate_vertex_arrays(u64 count, u32* arrays) = 0;
                virtual void bind_vertex_array(u32 arrayID) = 0;
                virtual void vertex_attrib_divisor(u32 index, u32 divisor) = 0;

                // Drawing
                virtual void draw_elements(u32 mode, u64 count, u32 type, const void* indices) = 0;
                virtual void draw_arrays(u32 mode, s32 first, u64 count) = 0;
                virtual void draw_elements_instanced(u32 mode, u64 count, u32 type, const void* indices, u64 instance_count) = 0;
                virtual void draw_arrays_instanced(u32 mode, s32 first, u64 count, u64 instance_count) = 0;

                // Device state
                virtual const char* get_string_value(u32 name) = 0;

                virtual void get_integer_value(u32 pname, s32* data) = 0;

                virtual u32 get_error() = 0;

                // Framebuffers
                virtual u32 check_framebuffer_status(u32 target) = 0;
                
                virtual void delete_framebuffers(u64 count, const u32* framebuffers) = 0;
                virtual void generate_framebuffers(u64 count, u32* framebuffers) = 0;
                virtual void bind_framebuffer(u32 target, s32 index) = 0;
                virtual void blit_framebuffer(s32 srcx, s32 srcy, s32 srcWidth, s32 srcHeight, s32 dstx, s32 dsty, s32 dstWidth, s32 dstHeight, u32 mask, u32 filter) = 0;
                virtual void framebuffer_texture2D(u32 target, u32 attachment, u32 textureTarget, u32 texture, s32 level) = 0;
                virtual void draw_buffer(u32 buffer) = 0;
                virtual void draw_buffers(u64 count, const u32* buffers) = 0;

                // Renderbuffers
                virtual void generate_renderbuffers(u64 count, u32* renderbuffers) = 0;
                virtual void bind_renderbuffer(u32 target, s32 renderbuffer) = 0;
                virtual void renderbuffer_storage(u32 target, u32 internal_format, u64 width, u64 height) = 0;
                virtual void framebuffer_renderbuffer(u32 target, u32 attachement, u32 renderbuffer_target, u32 renderbuffer) = 0;
                virtual void delete_renderbuffers(u64 count, const u32* renderbuffers) = 0;

                // Buffers
                virtual void delete_buffers(u64 count, const u32* buffers) = 0;
                virtual void generate_buffers(u64 count, u32* buffers) = 0;
                virtual void bind_buffer(u32 target, u32 index) = 0;
                virtual void bind_buffer_base(u32 target, u32 index, u32 buffer) = 0;
                virtual void buffer_data(u32 target, u32 size, const void* data, u32 usage) = 0;
                virtual void buffer_sub_data(u32 target, s64 offset, s64 size, const void* data) = 0;

                // Textures
                virtual void delete_textures(u64 count, const u32* textures) = 0;
                virtual void generate_textures(u64 count, u32* textures) = 0;
                virtual void activate_texture(u32 texture) = 0;
                virtual void bind_texture(u32 target, u32 texture) = 0;
                virtual void texture_image_2D(u32 target, s32 level, s32 internalformat, u64 width, u64 height, s32 border, u32 format, u32 type, const void* data) = 0;
                virtual void texture_sub_image_2D(u32 target, s32 level, s32 xoffset, s32 yoffset, u64 width, u64 height, u32 format, u32 type, const void* data) = 0;
                virtual void set_texture_integer_parameter(u32 target, u32 pname, s32 param) = 0;
                virtual void set_texture_float_array_parameter(u32 target, u32 pname, f32* param) = 0;

                // Shaders
                virtual u32 create_shader(u32 shaderType) = 0;
                
                virtual void delete_shader(u32 shader) = 0;
                virtual void shader_source(u32 shader, u64 count, const char8** code, const s32* length) = 0;
                virtual void compile_shader(u32 shader) = 0;
                virtual void attach_shader(u32 program, u32 shader) = 0;
                virtual void detach_shader(u32 program, u32 shader) = 0;

                virtual void get_shader_integer_value(u32 shader, u32 pname, s32* params) = 0;
                virtual void get_shader_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog) = 0;

                // Programs
                virtual u32 create_program() = 0;

                virtual void use_program(u32 program) = 0;
                virtual void link_program(u32 program) = 0;
                virtual void delete_program(u32 program) = 0;

                virtual void get_program_integer_value(u32 shader, u32 pname, s32* params) = 0;
                virtual void get_program_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog) = 0;

                // Uniforms               
                virtual s32 get_uniform_location(u32 program, const char8* name) = 0;

                virtual void uniform_matrix_2fv(s32 location, u64 count, bool transpose, const f32* value) = 0;
                virtual void uniform_matrix_3fv(s32 location, u64 count, bool transpose, const f32* value) = 0;
                virtual void uniform_matrix_4fv(s32 location, u64 count, bool transpose, const f32* value) = 0;
                virtual void uniform_1f(s32 location, f32 value) = 0;
                virtual void uniform_2f(s32 location, f32 x, f32 y) = 0;
                virtual void uniform_3f(s32 location, f32 x, f32 y, f32 z) = 0;
                virtual void uniform_4f(s32 location, f32 x, f32 y, f32 z, f32 w) = 0;
                virtual void uniform_1fv(s32 location, u64 count, const f32* value) = 0;
                virtual void uniform_2fv(s32 location, u64 count, const f32* value) = 0;
                virtual void uniform_3fv(s32 location, u64 count, const f32* value) = 0;
                virtual void uniform_4fv(s32 location, u64 count, const f32* value) = 0;
                virtual void uniform_1i(s32 location, s32 x) = 0;
                virtual void uniform_2i(s32 location, s32 x, s32 y) = 0;
                virtual void uniform_3i(s32 location, s32 x, s32 y, s32 z) = 0;
                virtual void uniform_4i(s32 location, s32 x, s32 y, s32 z, s32 w) = 0;
                virtual void uniform_1iv(s32 location, u64 count, const s32* x) = 0;
                virtual void uniform_1ui(s32 location, u32 x) = 0;
                virtual void uniform_1uiv(s32 location, u64 count, const u32* x) = 0;
            };

            class function_library : public ifunction_library
            {
            public:
                ~function_library() override = default;

                // General
                void viewport(s32 x, s32 y, s32 width, s32 height) override;
                void scissor(s32 x, s32 y, s64 width, s64 height) override;
                void read_pixels(s32 x, s32 y, s32 width, s32 height, u32 format, u32 type, void* data) override;

                void clear_color(f32 red, f32 green, f32 blue, f32 alpha) override;
                void clear_depth(f32 depth) override;
                void clear(u32 bitfield) override;

                void enable(u32 value) override;
                void disable(u32 value) override;

                void depth_mask(bool flag) override;
                void color_mask(bool red, bool green, bool blue, bool alpha) override;

                void pixel_store_i(u32 pname, s32 param) override;

                void blend_func(u32 sfactor, u32 dfactor) override;
                void cull_face(u32 mode) override;
                void front_face(u32 mode) override;
                void depth_func(u32 fn) override;
                void polygon_mode(u32 face, u32 mode) override;
                void line_width(f32 width) override;

                // Vertex Arrays
                void enable_vertex_attrib_array(u32 index) override;
                void vertex_attrib_i_pointer(u32 index, s32 size, u32 type, u64 stride, const void* ptr) override;
                void vertex_attrib_pointer(u32 index, s32 size, u32 type, bool normalized, u64 stride, const void* ptr) override;
                void delete_vertex_arrays(u64 count, const u32* arrays) override;
                void generate_vertex_arrays(u64 count, u32* arrays) override;
                void bind_vertex_array(u32 arrayID) override;
                void vertex_attrib_divisor(u32 index, u32 divisor) override;

                // Drawing
                void draw_elements(u32 mode, u64 count, u32 type, const void* indices) override;
                void draw_arrays(u32 mode, s32 first, u64 count) override;
                void draw_elements_instanced(u32 mode, u64 count, u32 type, const void* indices, u64 instance_count) override;
                void draw_arrays_instanced(u32 mode, s32 first, u64 count, u64 instance_count) override;

                // Device state
                const char* get_string_value(u32 name) override;

                void get_integer_value(u32 pname, s32* data) override;

                u32 get_error() override;

                // Framebuffers
                u32 check_framebuffer_status(u32 target) override;

                void delete_framebuffers(u64 count, const u32* framebuffers) override;
                void generate_framebuffers(u64 count, u32* framebuffers) override;
                void bind_framebuffer(u32 target, s32 index) override;
                void blit_framebuffer(s32 srcx, s32 srcy, s32 srcWidth, s32 srcHeight, s32 dstx, s32 dsty, s32 dstWidth, s32 dstHeight, u32 mask, u32 filter) override;
                void framebuffer_texture2D(u32 target, u32 attachment, u32 textureTarget, u32 texture, s32 level) override;
                void draw_buffer(u32 buffer) override;
                void draw_buffers(u64 count, const u32* buffers) override;

                // Renderbuffers
                void generate_renderbuffers(u64 count, u32* renderbuffers) override;
                void bind_renderbuffer(u32 target, s32 renderbuffer) override;
                void renderbuffer_storage(u32 target, u32 internal_format, u64 width, u64 height) override;
                void framebuffer_renderbuffer(u32 target, u32 attachement, u32 renderbuffer_target, u32 renderbuffer) override;
                void delete_renderbuffers(u64 count, const u32* renderbuffers) override;

                // Buffers
                void delete_buffers(u64 count, const u32* buffers) override;
                void generate_buffers(u64 count, u32* buffers) override;
                void bind_buffer(u32 target, u32 index) override;
                void bind_buffer_base(u32 target, u32 index, u32 buffer) override;
                void buffer_data(u32 target, u32 size, const void* data, u32 usage) override;
                void buffer_sub_data(u32 target, s64 offset, s64 size, const void* data) override;

                // Textures
                void delete_textures(u64 count, const u32* textures) override;
                void generate_textures(u64 count, u32* textures) override;
                void activate_texture(u32 texture) override;
                void bind_texture(u32 target, u32 texture) override;
                void texture_image_2D(u32 target, s32 level, s32 internalformat, u64 width, u64 height, s32 border, u32 format, u32 type, const void* data) override;
                void texture_sub_image_2D(u32 target, s32 level, s32 xoffset, s32 yoffset, u64 width, u64 height, u32 format, u32 type, const void* data) override;
                void set_texture_integer_parameter(u32 target, u32 pname, s32 param) override;
                void set_texture_float_array_parameter(u32 target, u32 pname, f32* param) override;

                // Shaders
                u32 create_shader(u32 shaderType) override;

                void delete_shader(u32 shader) override;
                void shader_source(u32 shader, u64 count, const char8** code, const s32* length) override;
                void compile_shader(u32 shader) override;
                void attach_shader(u32 program, u32 shader) override;
                void detach_shader(u32 program, u32 shader) override;

                void get_shader_integer_value(u32 shader, u32 pname, s32* params) override;
                void get_shader_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog) override;

                // Programs
                u32 create_program() override;

                void use_program(u32 program) override;
                void link_program(u32 program) override;
                void delete_program(u32 program) override;

                void get_program_integer_value(u32 shader, u32 pname, s32* params) override;
                void get_program_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog) override;

                // Uniforms               
                s32 get_uniform_location(u32 program, const char8* name) override;

                void uniform_matrix_2fv(s32 location, u64 count, bool transpose, const f32* value) override;
                void uniform_matrix_3fv(s32 location, u64 count, bool transpose, const f32* value) override;
                void uniform_matrix_4fv(s32 location, u64 count, bool transpose, const f32* value) override;
                void uniform_1f(s32 location, f32 value) override;
                void uniform_2f(s32 location, f32 x, f32 y) override;
                void uniform_3f(s32 location, f32 x, f32 y, f32 z) override;
                void uniform_4f(s32 location, f32 x, f32 y, f32 z, f32 w) override;
                void uniform_1fv(s32 location, u64 count, const f32* value) override;
                void uniform_2fv(s32 location, u64 count, const f32* value) override;
                void uniform_3fv(s32 location, u64 count, const f32* value) override;
                void uniform_4fv(s32 location, u64 count, const f32* value) override;
                void uniform_1i(s32 location, s32 x) override;
                void uniform_2i(s32 location, s32 x, s32 y) override;
                void uniform_3i(s32 location, s32 x, s32 y, s32 z) override;
                void uniform_4i(s32 location, s32 x, s32 y, s32 z, s32 w) override;
                void uniform_1iv(s32 location, u64 count, const s32* x) override;
                void uniform_1ui(s32 location, u32 x) override;
                void uniform_1uiv(s32 location, u64 count, const u32* x) override;
            };

            class mock_function_library : public ifunction_library
            {
            public:
                ~mock_function_library() override = default;

                // General
                void viewport(s32 x, s32 y, s32 width, s32 height) override;
                void scissor(s32 x, s32 y, s64 width, s64 height) override;
                void read_pixels(s32 x, s32 y, s32 width, s32 height, u32 format, u32 type, void* data) override;

                void clear_color(f32 red, f32 green, f32 blue, f32 alpha) override;
                void clear_depth(f32 depth) override;
                void clear(u32 bitfield) override;

                void enable(u32 value) override;
                void disable(u32 value) override;

                void depth_mask(bool flag) override;
                void color_mask(bool red, bool green, bool blue, bool alpha) override;

                void pixel_store_i(u32 pname, s32 param) override;

                void blend_func(u32 sfactor, u32 dfactor) override;
                void cull_face(u32 mode) override;
                void front_face(u32 mode) override;
                void depth_func(u32 fn) override;
                void polygon_mode(u32 face, u32 mode) override;
                void line_width(f32 width) override;

                // Vertex Arrays
                void enable_vertex_attrib_array(u32 index) override;
                void vertex_attrib_i_pointer(u32 index, s32 size, u32 type, u64 stride, const void* ptr) override;
                void vertex_attrib_pointer(u32 index, s32 size, u32 type, bool normalized, u64 stride, const void* ptr) override;
                void delete_vertex_arrays(u64 count, const u32* arrays) override;
                void generate_vertex_arrays(u64 count, u32* arrays) override;
                void bind_vertex_array(u32 arrayID) override;
                void vertex_attrib_divisor(u32 index, u32 divisor) override;

                // Drawing
                void draw_elements(u32 mode, u64 count, u32 type, const void* indices) override;
                void draw_arrays(u32 mode, s32 first, u64 count) override;
                void draw_elements_instanced(u32 mode, u64 count, u32 type, const void* indices, u64 instance_count) override;
                void draw_arrays_instanced(u32 mode, s32 first, u64 count, u64 instance_count) override;

                // Device state
                const char* get_string_value(u32 name) override;

                void get_integer_value(u32 pname, s32* data) override;

                u32 get_error() override;

                // Framebuffers
                u32 check_framebuffer_status(u32 target) override;

                void delete_framebuffers(u64 count, const u32* framebuffers) override;
                void generate_framebuffers(u64 count, u32* framebuffers) override;
                void bind_framebuffer(u32 target, s32 index) override;
                void blit_framebuffer(s32 srcx, s32 srcy, s32 srcWidth, s32 srcHeight, s32 dstx, s32 dsty, s32 dstWidth, s32 dstHeight, u32 mask, u32 filter) override;
                void framebuffer_texture2D(u32 target, u32 attachment, u32 textureTarget, u32 texture, s32 level) override;
                void draw_buffer(u32 buffer) override;
                void draw_buffers(u64 count, const u32* buffers) override;

                // Renderbuffers
                void generate_renderbuffers(u64 count, u32* renderbuffers) override;
                void bind_renderbuffer(u32 target, s32 renderbuffer) override;
                void renderbuffer_storage(u32 target, u32 internal_format, u64 width, u64 height) override;
                void framebuffer_renderbuffer(u32 target, u32 attachement, u32 renderbuffer_target, u32 renderbuffer) override;
                void delete_renderbuffers(u64 count, const u32* renderbuffers) override;

                // Buffers
                void delete_buffers(u64 count, const u32* buffers) override;
                void generate_buffers(u64 count, u32* buffers) override;
                void bind_buffer(u32 target, u32 index) override;
                void bind_buffer_base(u32 target, u32 index, u32 buffer) override;
                void buffer_data(u32 target, u32 size, const void* data, u32 usage) override;
                void buffer_sub_data(u32 target, s64 offset, s64 size, const void* data) override;

                // Textures
                void delete_textures(u64 count, const u32* textures) override;
                void generate_textures(u64 count, u32* textures) override;
                void activate_texture(u32 texture) override;
                void bind_texture(u32 target, u32 texture) override;
                void texture_image_2D(u32 target, s32 level, s32 internalformat, u64 width, u64 height, s32 border, u32 format, u32 type, const void* data) override;
                void texture_sub_image_2D(u32 target, s32 level, s32 xoffset, s32 yoffset, u64 width, u64 height, u32 format, u32 type, const void* data) override;
                void set_texture_integer_parameter(u32 target, u32 pname, s32 param) override;
                void set_texture_float_array_parameter(u32 target, u32 pname, f32* param) override;

                // Shaders
                u32 create_shader(u32 shaderType) override;

                void delete_shader(u32 shader) override;
                void shader_source(u32 shader, u64 count, const char8** code, const s32* length) override;
                void compile_shader(u32 shader) override;
                void attach_shader(u32 program, u32 shader) override;
                void detach_shader(u32 program, u32 shader) override;

                void get_shader_integer_value(u32 shader, u32 pname, s32* params) override;
                void get_shader_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog) override;

                // Programs
                u32 create_program() override;

                void use_program(u32 program) override;
                void link_program(u32 program) override;
                void delete_program(u32 program) override;

                void get_program_integer_value(u32 shader, u32 pname, s32* params) override;
                void get_program_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog) override;

                // Uniforms               
                s32 get_uniform_location(u32 program, const char8* name) override;

                void uniform_matrix_2fv(s32 location, u64 count, bool transpose, const f32* value) override;
                void uniform_matrix_3fv(s32 location, u64 count, bool transpose, const f32* value) override;
                void uniform_matrix_4fv(s32 location, u64 count, bool transpose, const f32* value) override;
                void uniform_1f(s32 location, f32 value) override;
                void uniform_2f(s32 location, f32 x, f32 y) override;
                void uniform_3f(s32 location, f32 x, f32 y, f32 z) override;
                void uniform_4f(s32 location, f32 x, f32 y, f32 z, f32 w) override;
                void uniform_1fv(s32 location, u64 count, const f32* value) override;
                void uniform_2fv(s32 location, u64 count, const f32* value) override;
                void uniform_3fv(s32 location, u64 count, const f32* value) override;
                void uniform_4fv(s32 location, u64 count, const f32* value) override;
                void uniform_1i(s32 location, s32 x) override;
                void uniform_2i(s32 location, s32 x, s32 y) override;
                void uniform_3i(s32 location, s32 x, s32 y, s32 z) override;
                void uniform_4i(s32 location, s32 x, s32 y, s32 z, s32 w) override;
                void uniform_1iv(s32 location, u64 count, const s32* x) override;
                void uniform_1ui(s32 location, u32 x) override;
                void uniform_1uiv(s32 location, u64 count, const u32* x) override;
            };
        }
    }
}