#include "render/opengl/render_gl_function_library.h"
#include "render/opengl/render_gl_error.h"

#include "util/log.h"

#include "string/string_conversions.h"

#include <glad/glad.h>
#include <assert.h>
#include <sstream>

#include <gsl/narrow>

#define ENABLE_GL_FUNCTION_LOGGING 0
#define ENABLE_GL_PARAMETER_LOGGING 1

#if ENABLE_GL_FUNCTION_LOGGING
#define GL_LOG log::info
#else
#define GL_LOG(...) (void*)(__VA_ARGS__)
#endif

namespace ppp
{
    namespace render
    {
        namespace opengl
        {
            //-------------------------------------------------------------------------
            std::string pname_to_string(const u32 pname)
            {
                switch (pname)
                {
                case GL_VERSION: return "OpenGL Version";
                case GL_CONTEXT_FLAGS: return "OpenGL Context Flags";
                case GL_INFO_LOG_LENGTH: return "OpenGL Info Log Length";
                case GL_COMPILE_STATUS: return "OpenGL Compile Status";
                case GL_LINK_STATUS: return "OpenGL Link Status";
                case GL_CURRENT_PROGRAM: return "OpenGL Current Program";
                case GL_TEXTURE_MIN_FILTER: return "Texture Min Filter";
                case GL_TEXTURE_MAG_FILTER: return "Texture Mag Filter";
                case GL_MAX_TEXTURE_IMAGE_UNITS: return "Max Texture Image Units";
                case GL_MAX_ELEMENTS_INDICES: return "Max Elements Indices";
                case GL_MAX_ELEMENTS_VERTICES: return "Max Elements Vertices";
                }

                log::warn("Unknown state, this list is incomplete maybe you need to add another element: {0}", pname);
                return string::to_string<std::string>(pname);
            }

            //-------------------------------------------------------------------------
            std::string pixeltype_to_string(const u32 pixelType)
            {
                switch (pixelType)
                {
                case GL_UNSIGNED_BYTE: return "GL_UNSIGNED_BYTE";
                case GL_BYTE: return "GL_BYTE";

                case GL_UNSIGNED_SHORT: return "GL_UNSIGNED_SHORT";
                case GL_SHORT: return "GL_SHORT";

                case GL_INT: return "GL_INT";
                case GL_UNSIGNED_INT: return "GL_UNSIGNED_INT";

                case GL_HALF_FLOAT: return "GL_HALF_FLOAT";
                case GL_FLOAT: return "GL_FLOAT";

                case GL_UNSIGNED_INT_24_8: return "GL_UNSIGNED_INT_24_8";
                case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: return "GL_FLOAT_32_UNSIGNED_INT_24_8_REV";
                }

                log::warn("Unknown pxiel type: {0}", pixelType);
                return string::to_string<std::string>(pixelType);
            }

            //-------------------------------------------------------------------------
            std::string format_to_string(const u32 format)
            {
                switch (format)
                {
                case GL_RED: return "GL_RED";
                case GL_RG: return "GL_RG";
                case GL_RGB: return "GL_RGB";
                case GL_RGBA: return "GL_RGBA";

                case GL_DEPTH_COMPONENT: return "GL_DEPTH_COMPONENT";
                case GL_DEPTH_STENCIL: return "GL_DEPTH_STENCIL";
                }

                log::warn("Unknown format: {0}", format);
                return string::to_string<std::string>(format);
            }

            //-------------------------------------------------------------------------
            std::string internal_format_to_string(const u32 format)
            {
                switch (format)
                {
                case GL_R8: return "GL_R8";
                case GL_R8I: return "GL_R8I";
                case GL_R8UI: return "GL_R8UI";
                case GL_R16: return "GL_R16";

                case GL_R16I: return "GL_R16I";
                case GL_R16UI: return "GL_R16UI";
                case GL_R16F: return "GL_R16F";
                case GL_R32I: return "GL_R32I";

                case GL_R32UI: return "GL_R32UI";
                case GL_R32F: return "GL_R32F";
                case GL_RG8: return "GL_RG8";

                case GL_RG8I: return "GL_RG8I";
                case GL_RG8UI: return "GL_RG8UI";

                case GL_RG16: return "GL_RG16";
                case GL_RG16I: return "GL_RG16I";
                case GL_RG16UI: return "GL_RG16UI";
                case GL_RG16F: return "GL_RG16F";
                case GL_RG32I: return "GL_RG32I";
                case GL_RG32UI: return "GL_RG32UI";
                case GL_RG32F: return "GL_RG32F";
                case GL_RGB8: return "GL_RGB8";

                case GL_RGB8I: return "GL_RGB8I";
                case GL_RGB8UI: return "GL_RGB8UI";

                case GL_RGB16: return "GL_RGB16";
                case GL_RGB16I: return "GL_RGB16I";
                case GL_RGB16UI: return "GL_RGB16UI";
                case GL_RGB16F: return "GL_RGB16F";

                case GL_RGB32I: return "GL_RGB32I";
                case GL_RGB32UI: return "GL_RGB32UI";
                case GL_RGB32F: return "GL_RGB32F";

                case GL_RGBA: return "GL_RGBA";

                case GL_RGBA8: return "GL_RGBA8";
                case GL_RGBA8I: return "GL_RGBA8I";
                case GL_RGBA8UI: return "GL_RGBA8UI";

                case GL_RGBA16: return "GL_RGBA16";
                case GL_RGBA16I: return "GL_RGBA16I";
                case GL_RGBA16UI: return "GL_RGBA16UI";
                case GL_RGBA16F: return "GL_RGBA16F";

                case GL_RGBA32I: return "GL_RGBA32I";
                case GL_RGBA32UI: return "GL_RGBA32UI";
                case GL_RGBA32F: return "GL_RGBA32F";
                case GL_DEPTH_COMPONENT16: return "GL_DEPTH_COMPONENT16";
                case GL_DEPTH_COMPONENT24: return "GL_DEPTH_COMPONENT24";

                case GL_DEPTH_COMPONENT32: return "GL_DEPTH_COMPONENT32";
                case GL_DEPTH_COMPONENT32F: return "GL_DEPTH_COMPONENT32F";
                case GL_DEPTH24_STENCIL8: return "GL_DEPTH24_STENCIL8";
                case GL_DEPTH32F_STENCIL8: return "GL_DEPTH32F_STENCIL8";
                }

                log::warn("Unknown internal format: {0}", format);
                return string::to_string<std::string>(format);
            }

            //-------------------------------------------------------------------------
            std::string buffer_target_to_string(const u32 target)
            {
                switch (target)
                {
                case GL_ARRAY_BUFFER: return "Vertex attributes";
                case GL_ATOMIC_COUNTER_BUFFER: return "Atomic counter storage";
                case GL_COPY_READ_BUFFER: return "Buffer copy source";
                case GL_COPY_WRITE_BUFFER: return "Buffer copy destination";
                case GL_DISPATCH_INDIRECT_BUFFER: return "Indirect compute dispatch commands";
                case GL_DRAW_INDIRECT_BUFFER: return "Indirect command arguments";
                case GL_ELEMENT_ARRAY_BUFFER: return "Vertex array indices";
                case GL_PIXEL_PACK_BUFFER: return "Pixel read target";
                case GL_PIXEL_UNPACK_BUFFER: return "Texture data source";
                case GL_QUERY_BUFFER: return "Query result buffer";
                case GL_SHADER_STORAGE_BUFFER: return "Read-write storage for shaders";
                case GL_TEXTURE_BUFFER: return "Texture data buffer";
                case GL_TRANSFORM_FEEDBACK_BUFFER: return "Transform feedback buffer";
                case GL_UNIFORM_BUFFER: return "Uniform block storage";
                }

                log::warn("Unknown buffer target: {0}", target);
                return string::to_string<std::string>(target);
            }
            
            //-------------------------------------------------------------------------
            std::string texture_target_to_string(const u32 target)
            {
                switch (target)
                {
                case GL_TEXTURE_1D: return "Texture 1D";
                case GL_TEXTURE_2D: return "Texture 2D";
                case GL_TEXTURE_3D: return "Texture 3D";
                case GL_TEXTURE_1D_ARRAY: return "Texture 1D Array";
                case GL_TEXTURE_2D_ARRAY: return "Texture 2D Array";
                case GL_TEXTURE_RECTANGLE: return "Texture Rectangle";
                case GL_TEXTURE_CUBE_MAP: return "Texture Cube Map";
                case GL_TEXTURE_CUBE_MAP_ARRAY: return "Texture Cube Map Array";
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X: return "Texture Cube Map Positive X";
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X: return "Texture Cube Map Negative X";
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y: return "Texture Cube Map Positive Y";
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y: return "Texture Cube Map Negative Y";
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z: return "Texture Cube Map Positive Z";
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z: return "Texture Cube Map Negative Z";
                case GL_TEXTURE_BUFFER: return "Texture Buffer";
                case GL_TEXTURE_2D_MULTISAMPLE: return "Texture 2D Multisample";
                case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return "Texture 2D Multisample Array";
                }

                log::warn("Unknown texture target: {0}", target);
                return string::to_string<std::string>(target);
            }
            
            //-------------------------------------------------------------------------
            std::string shadertype_to_string(const u32 shaderType)
            {
                switch (shaderType)
                {
                case GL_COMPUTE_SHADER: return "Compute Shader";
                case GL_VERTEX_SHADER: return "Vertex Shader";
                case GL_TESS_CONTROL_SHADER: return "Tessalation Control Shader";
                case GL_TESS_EVALUATION_SHADER: return "Tessalation Evaluation Shader";
                case GL_GEOMETRY_SHADER: return "Geometry Shader";
                case GL_FRAGMENT_SHADER: return "Fragment Shader";
                }

                log::warn("Unknown shader type: {0}", shaderType);
                return string::to_string<std::string>(shaderType);
            }
            
            //-------------------------------------------------------------------------
            std::string framebuffer_target_to_string(const u32 target)
            {
                switch (target)
                {
                case GL_DRAW_FRAMEBUFFER: return "Draw Framebuffer";
                case GL_READ_FRAMEBUFFER: return "Read Framebuffer";
                case GL_FRAMEBUFFER: return "Framebuffer";
                }

                log::warn("Unknown framebuffer target: {0}", target);
                return string::to_string<std::string>(target);
            }

            //-------------------------------------------------------------------------
            std::string framebuffer_attachment_to_string(const u32 attachment)
            {
                for (int i = GL_COLOR_ATTACHMENT0; i <= GL_COLOR_ATTACHMENT31; i += (GL_COLOR_ATTACHMENT1 - GL_COLOR_ATTACHMENT0))
                {
                    if (attachment == i)
                    {
                        std::stringstream stream;
                        stream << "Color Attachment";
                        stream << i;
                        return stream.str();
                    }
                }

                switch (attachment)
                {
                case GL_DEPTH_ATTACHMENT: return "Depth Attachment";
                case GL_STENCIL_ATTACHMENT: return "Stencil Attachment";
                case GL_DEPTH_STENCIL_ATTACHMENT: return "Depth Stencil Attachment";
                }

                log::warn("Unknown renderbuffer attachment: {0}", attachment);
                return string::to_string<std::string>(attachment);
            }

            //-------------------------------------------------------------------------
            std::string renderbuffer_target_to_string(const u32 target)
            {
                switch (target)
                {
                case GL_RENDERBUFFER: return "Render buffer";
                }

                log::warn("Unknown renderbuffer target: {0}", target);
                return string::to_string<std::string>(target);
            }
            
            //-------------------------------------------------------------------------
            std::string drawbuffer_target_to_string(const u32 target)
            {
                switch (target)
                {
                case GL_NONE: return "GL_NONE, No color buffers are written.";
                case GL_FRONT_LEFT: return "GL_FRONT_LEFT, Only the front left color buffer is written.";
                case GL_FRONT_RIGHT: return "GL_FRONT_RIGHT, Only the front right color buffer is written.";
                case GL_BACK_LEFT: return "GL_BACK_LEFT, Only the back left color buffer is written.";
                case GL_BACK_RIGHT: return "GL_BACK_RIGHT, Only the back right color buffer is written.";
                case GL_FRONT: return "GL_FRONT, Only the front left and front right color buffers are written.";
                case GL_BACK: return "GL_BACK, Only the back left and back right color buffers are written.";
                case GL_LEFT: return "GL_LEFT, Only the front left and back left color buffers are written.";
                case GL_RIGHT: return "GL_RIGHT, Only the front right and back right color buffers are written.";
                case GL_FRONT_AND_BACK: return "GL_FRONT_AND_BACK, All the front and back color buffers (front left, front right, back left, back right) are written.";
                }

                log::warn("Unknown drawbuffer target: {0}", target);
                return string::to_string<std::string>(target);
            }
            
            //-------------------------------------------------------------------------
            std::string buffer_usage_to_string(const u32 usage)
            {
                switch (usage)
                {
                case GL_STREAM_DRAW: return "Stream Draw";
                case GL_STREAM_READ: return "Stream Read";
                case GL_STREAM_COPY: return "Stream Copy";
                case GL_STATIC_DRAW: return "Static Draw";
                case GL_STATIC_READ: return "Static Read";
                case GL_STATIC_COPY: return "Static Copy";
                case GL_DYNAMIC_DRAW: return "Dynamic Draw";
                case GL_DYNAMIC_READ: return "Dynamic Read";
                case GL_DYNAMIC_COPY: return "Dynamic Copy";
                }

                log::warn("Unknown buffer usage: {0}", usage);
                return string::to_string<std::string>(usage);
            }

            //-------------------------------------------------------------------------
            std::string blend_func_factor_to_string(const u32 sfactor)
            {
                switch (sfactor)
                {
                case GL_ZERO: return "GL_ZERO";
                case GL_ONE: return "GL_ONE";
                case GL_SRC_COLOR: return "GL_SRC_COLOR";
                case GL_ONE_MINUS_SRC_COLOR: return "GL_ONE_MINUS_SRC_COLOR";
                case GL_DST_COLOR: return "GL_DST_COLOR";
                case GL_ONE_MINUS_DST_COLOR: return "GL_ONE_MINUS_DST_COLOR";
                case GL_SRC_ALPHA: return "GL_SRC_ALPHA";
                case GL_DST_ALPHA: return "GL_DST_ALPHA";
                case GL_ONE_MINUS_DST_ALPHA: return "GL_ONE_MINUS_DST_ALPHA";
                case GL_CONSTANT_COLOR: return "GL_CONSTANT_COLOR";
                case GL_ONE_MINUS_CONSTANT_COLOR: return "GL_ONE_MINUS_CONSTANT_COLOR";
                case GL_SRC_ALPHA_SATURATE: return "GL_SRC_ALPHA_SATURATE";
                }

                log::warn("Unknown blend func sfactor: {0}", sfactor);
                return string::to_string<std::string>(sfactor);
            }
            
            //-------------------------------------------------------------------------
            std::string cull_type_to_string(const u32 cullType)
            {
                switch (cullType)
                {
                case GL_BACK: return "Back";
                case GL_FRONT: return "Front";
                case GL_FRONT_AND_BACK: return "Front And Back";
                }

                log::warn("Unknown cull type: {0}", cullType);
                return string::to_string<std::string>(cullType);
            }
            
            //-------------------------------------------------------------------------
            std::string front_face_to_string(const u32 frontFace)
            {
                switch (frontFace)
                {
                case GL_CW: return "Clockwise";
                case GL_CCW: return "Counter Clockwise";
                }

                log::warn("Unknown front face type: {0}", frontFace);
                return string::to_string<std::string>(frontFace);
            }
            
            //-------------------------------------------------------------------------
            std::string depth_function_to_string(const u32 depthFn)
            {
                switch (depthFn)
                {
                case GL_ALWAYS: return "Always";
                case GL_NEVER: return "Never";
                case GL_LESS: return "Less Then";
                case GL_LEQUAL: return "Less Then Or Equal";
                case GL_GREATER: return "Greater Then";
                case GL_GEQUAL: return "Greater Then Or Equal";
                case GL_EQUAL: return "Equal";
                case GL_NOTEQUAL: return "Not Equal";
                }

                log::warn("Unknown depth function: {0}", depthFn);
                return string::to_string<std::string>(depthFn);
            }
            
            //-------------------------------------------------------------------------
            std::string fill_mode_to_string(const u32 fillMode)
            {
                switch (fillMode)
                {
                case GL_POINT: return "Point";
                case GL_LINE: return "Line";
                case GL_FILL: return "Fill";
                }

                log::warn("Unknown fill mode: {0}", fillMode);
                return string::to_string<std::string>(fillMode);
            }
            
            //-------------------------------------------------------------------------
            std::string enable_to_string(const u32 enable)
            {
                switch (enable)
                {
                case GL_BLEND: return "Blending";
                case GL_CULL_FACE: return "Cull Face";
                case GL_DEPTH_TEST: return "Depth Test";
                case GL_STENCIL_TEST: return "Stencil Test";
#if _DEBUG
                case GL_DEBUG_OUTPUT: return "Debug Output";
                case GL_DEBUG_OUTPUT_SYNCHRONOUS: return "Debug Output Synchronous";
#endif
                }

                log::warn("Unknown state, this list is incomplete maybe you need to add another element: {0}", enable);
                return string::to_string<std::string>(enable);
            }
            
            //-------------------------------------------------------------------------
            std::string disable_to_string(const u32 disable)
            {
                switch (disable)
                {
                case GL_BLEND: return "Blending";
                case GL_CULL_FACE: return "Cull Face";
                case GL_DEPTH_TEST: return "Depth Test";
                case GL_STENCIL_TEST: return "Stencil Test";
                }

                log::warn("Unknown state, this list is incomplete maybe you need to add another element: {0}", disable);
                return string::to_string<std::string>(disable);
            }
            
            //-------------------------------------------------------------------------
            std::string blit_framebuffer_mask(const u32 mask)
            {
                switch (mask)
                {
                case GL_COLOR_BUFFER_BIT: return "Color Buffer Bit";
                case GL_DEPTH_BUFFER_BIT: return "Depth Buffer Bit";
                case GL_STENCIL_BUFFER_BIT: return "Stencil Buffer Bit";
                }

                log::warn("Unknown mask, this list is incomplete maybe you need to add another element: {0}", mask);
                return string::to_string<std::string>(mask);
            }
            
            //-------------------------------------------------------------------------
            std::string blit_framebuffer_filter(const u32 filter)
            {
                switch (filter)
                {
                case GL_NEAREST: return "Nearest";
                case GL_LINEAR: return "Linear";
                }

                log::warn("Unknown filter, this list is incomplete maybe you need to add another element: {0}", filter);
                return string::to_string<std::string>(filter);
            }

#pragma region FUNCTION LIBRARY
            //-------------------------------------------------------------------------
            //-------------------------------------------------------------------------
            // FUNCTION LIBRARY
            //-------------------------------------------------------------------------
            // General
            //-------------------------------------------------------------------------
            void function_library::viewport(s32 x, s32 y, s32 width, s32 height)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for viewport resize");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for viewport resize");

                GL_LOG("glViewport");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
#endif

                GL_CALL(glViewport(x, y, width, height));
            }
            //-------------------------------------------------------------------------
            void function_library::scissor(s32 x, s32 y, s64 width, s64 height)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for scissor resize");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for scissor resize");

                GL_LOG("glScissor");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
#endif
                GL_CALL(glScissor(x, y, width, height));
            }
            //-------------------------------------------------------------------------
            void function_library::read_pixels(s32 x, s32 y, s32 width, s32 height, u32 format, u32 type, void* data)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for viewport resize");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for viewport resize");

                GL_LOG("glReadPixels");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
                GL_LOG("\tformat: {0}", format_to_string(format));
                GL_LOG("\ttype: {0}", pixeltype_to_string(type));
#endif

                GL_CALL(glReadPixels(x, y, width, height, format, type, data));
            }

            //-------------------------------------------------------------------------
            void function_library::clear_color(f32 red, f32 green, f32 blue, f32 alpha)
            {
                GL_LOG("glClearColor");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tred: {0}", red);
                GL_LOG("\tgreen: {0}", green);
                GL_LOG("\tblue: {0}", blue);
                GL_LOG("\talpha: {0}", alpha);
#endif

                GL_CALL(glClearColor(red, green, blue, alpha));
            }
            //-------------------------------------------------------------------------
            void function_library::clear_depth(f32 depth)
            {
                GL_LOG("glClearDepth");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tdepth: {0}", depth);
#endif

                GL_CALL(glClearDepth(depth));
            }
            //-------------------------------------------------------------------------
            void function_library::clear(u32 bitfield)
            {


                GL_LOG("glClear");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tbitfield: {0}", bitfield);
#endif

                GL_CALL(glClear(bitfield));
            }

            //-------------------------------------------------------------------------
            void function_library::enable(u32 value)
            {
                GL_LOG("glEnable");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tvalue: {0}", enable_to_string(value));
#endif

                GL_CALL(glEnable(value));
            }
            //-------------------------------------------------------------------------
            void function_library::disable(u32 value)
            {
                GL_LOG("glDisable");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tvalue: {0}", disable_to_string(value));
#endif

                GL_CALL(glDisable(value));
            }

            //-------------------------------------------------------------------------
            void function_library::depth_mask(bool flag)
            {
                GL_LOG("glDepthMask");
                #if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tflag: {0}", flag);
                #endif

                GL_CALL(glDepthMask(flag));
            }
            //-------------------------------------------------------------------------
            void function_library::color_mask(bool red, bool green, bool blue, bool alpha)
            {
                GL_LOG("glColorMask");
                #if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tred: {0}", red);
                GL_LOG("\tgreen: {0}", green);
                GL_LOG("\tblue: {0}", blue);
                GL_LOG("\talpha: {0}", alpha);
                #endif

                GL_CALL(glColorMask(red, green, blue, alpha));
            }

            //-------------------------------------------------------------------------
            void function_library::blend_func(u32 sfactor, u32 dfactor)
            {
                GL_LOG("glBlendFunc");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tsfactor: {0}", blend_func_factor_to_string(sfactor));
                GL_LOG("\tdfactor: {0}", blend_func_factor_to_string(dfactor));
#endif
                glBlendFunc(sfactor, dfactor);
            }
            //-------------------------------------------------------------------------
            void function_library::cull_face(u32 mode)
            {
                GL_LOG("glCullFace");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", cull_type_to_string(mode));
#endif

                GL_CALL(glCullFace(mode));
            }
            //-------------------------------------------------------------------------
            void function_library::front_face(u32 mode)
            {
                GL_LOG("glFrontFace");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", front_face_to_string(mode));
#endif

                GL_CALL(glFrontFace(mode));
            }
            //-------------------------------------------------------------------------
            void function_library::depth_func(u32 fn)
            {
                GL_LOG("glDepthFunc");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tfunction: {0}", depth_function_to_string(fn));
#endif

                GL_CALL(glDepthFunc(fn));
            }
            //-------------------------------------------------------------------------
            void function_library::polygon_mode(u32 face, u32 mode)
            {
                GL_LOG("glPolygonMode");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tface: {0}", cull_type_to_string(face));
                GL_LOG("\tmode: {0}", fill_mode_to_string(mode));
#endif

                GL_CALL(glPolygonMode(face, mode));
            }
            //-------------------------------------------------------------------------
            void function_library::line_width(f32 width)
            {
                GL_LOG("glLineWidth");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\twidth: {0}", width);
#endif
                glLineWidth(width);
            }

            //-------------------------------------------------------------------------
            // Vertex Arrays
            //-------------------------------------------------------------------------
            void function_library::enable_vertex_attrib_array(u32 index)
            {
                GL_LOG("glEnableVertexAttribArray");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
#endif

                GL_CALL(glEnableVertexAttribArray(index));
            }
            //-------------------------------------------------------------------------
            void function_library::vertex_attrib_i_pointer(u32 index, s32 size, u32 type, u64 stride, const void* ptr)
            {
                GL_LOG("glVertexAttribIPointer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tstride: {0}", stride);
                GL_LOG("\tpointer: {0}", ptr);
#endif

                GL_CALL(glVertexAttribIPointer(index, size, type, gsl::narrow<GLsizei>(stride), ptr));
            }
            //-------------------------------------------------------------------------
            void function_library::vertex_attrib_pointer(u32 index, s32 size, u32 type, bool normalized, u64 stride, const void* ptr)
            {
                GL_LOG("glVertexAttribPointer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tnormalized: {0}", normalized);
                GL_LOG("\tstride: {0}", stride);
                GL_LOG("\tpointer: {0}", ptr);
#endif

                GL_CALL(glVertexAttribPointer(index, size, type, normalized, gsl::narrow<GLsizei>(stride), ptr));
            }
            //-------------------------------------------------------------------------
            void function_library::delete_vertex_arrays(u64 count, const u32* arrays)
            {
                GL_LOG("glDeleteVertexArrays");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tarrays: {0}", fmt::ptr(arrays));
#endif

                GL_CALL(glDeleteVertexArrays(gsl::narrow<GLsizei>(count), arrays));
            }
            //-------------------------------------------------------------------------
            void function_library::generate_vertex_arrays(u64 count, u32* arrays)
            {
                GL_LOG("glGenVertexArrays");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tarrays: {0}", fmt::ptr(arrays));
#endif

                GL_CALL(glGenVertexArrays(gsl::narrow<GLsizei>(count), arrays));
            }
            //-------------------------------------------------------------------------
            void function_library::bind_vertex_array(u32 arrayID)
            {
                GL_LOG("glBindVertexArray");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tarray: {0}", arrayID);
#endif

                GL_CALL(glBindVertexArray(arrayID));
            }
            //-------------------------------------------------------------------------
            void function_library::vertex_attrib_divisor(u32 index, u32 divisor)
            {
                GL_LOG("glVertexAttribDivisor");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tdivisor: {0}", divisor);
#endif

                GL_CALL(glVertexAttribDivisor(index, divisor));
            }

            //-------------------------------------------------------------------------
            // Drawing
            //-------------------------------------------------------------------------
            void function_library::draw_elements(u32 mode, u64 count, u32 type, const void* indices)
            {
                GL_LOG("glDrawElements");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tindices: {0}", fmt::ptr(indices));
#endif

                GL_CALL(glDrawElements(mode, gsl::narrow<GLsizei>(count), type, indices));
            }
            //-------------------------------------------------------------------------
            void function_library::draw_arrays(u32 mode, s32 first, u64 count)
            {
                GL_LOG("glDrawArrays");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tfirst: {0}", first);
                GL_LOG("\tcount: {0}", count);
#endif

                GL_CALL(glDrawArrays(mode, first, gsl::narrow<GLsizei>(count)));
            }
            //-------------------------------------------------------------------------
            void function_library::draw_elements_instanced(u32 mode, u64 count, u32 type, const void* indices, u64 instance_count)
            {
                GL_LOG("glDrawElementsInstanced");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tindices: {0}", fmt::ptr(indices));
                GL_LOG("\tinstance_count: {0}", instance_count);
#endif

                GL_CALL(glDrawElementsInstanced(mode, gsl::narrow<GLsizei>(count), type, indices, gsl::narrow<GLsizei>(instance_count)));
            }
            //-------------------------------------------------------------------------
            void function_library::draw_arrays_instanced(u32 mode, s32 first, u64 count, u64 instance_count)
            {
                GL_LOG("glDrawArraysInstanced");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tfirst: {0}", first);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tinstance_count: {0}", instance_count);
#endif

                GL_CALL(glDrawArraysInstanced(mode, first, gsl::narrow<GLsizei>(count), gsl::narrow<GLsizei>(instance_count)));
            }

            //-------------------------------------------------------------------------
            // Device State
            //-------------------------------------------------------------------------
            const char* function_library::get_string_value(u32 name)
            {
                GL_LOG("glGetString");

                const GLubyte* s;
                GL_CALL(s = glGetString(name));

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tdata result: {0}", *s);
#endif

                return (const char*)s;
            }
            //-------------------------------------------------------------------------
            void function_library::get_integer_value(u32 pname, s32* data)
            {
                GL_LOG("glGetIntegerv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tpname: {0}", pname_to_string(pname));
#endif

                GL_CALL(glGetIntegerv(pname, data));
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tdata result: {0}", *data);
#endif
            }
            //-------------------------------------------------------------------------
            u32 function_library::get_error()
            {
#if ENABLE_GL_FUNCTION_LOGGING && ENABLE_GL_GET_ERROR_LOGGING
                GL_LOG("glGetError");
#endif

                u32 error = GL_NO_ERROR;

                error = glGetError();  // no GL_CALL here to avoid function_library::recursive function call.

#if ENABLE_GL_FUNCTION_LOGGING && ENABLE_GL_GET_ERROR_LOGGING && ENABLE_GL_PARAMETER_LOGGING
                GL_LOG("\tdata result: {0}", error);
#endif

                return error;
            }

            //-------------------------------------------------------------------------
            // Framebuffers
            //-------------------------------------------------------------------------
            u32 function_library::check_framebuffer_status(u32 target)
            {
                GL_LOG("glCheckFramebufferStatus");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
#endif

                u32 result = GL_FRAMEBUFFER_COMPLETE;

                GL_CALL(result = glCheckFramebufferStatus(target));

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tresult: {0}", result);
#endif

                return result;
            }

            //-------------------------------------------------------------------------
            void function_library::delete_framebuffers(u64 count, const u32* framebuffers)
            {
                GL_LOG("glDeleteFramebuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tframebuffers: {0}", fmt::ptr(framebuffers));
#endif

                GL_CALL(glDeleteFramebuffers(gsl::narrow<GLsizei>(count), framebuffers));
            }
            //-------------------------------------------------------------------------
            void function_library::generate_framebuffers(u64 count, u32* framebuffers)
            {
                GL_LOG("glGenFramebuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tframebuffers: {0}", fmt::ptr(framebuffers));
#endif

                GL_CALL(glGenFramebuffers(gsl::narrow<GLsizei>(count), framebuffers));
            }
            //-------------------------------------------------------------------------
            void function_library::bind_framebuffer(u32 target, s32 index)
            {
                GL_LOG("glBindFramebuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
                GL_LOG("\tindex: {0}", index);
#endif

                GL_CALL(glBindFramebuffer(target, index));
            }
            //-------------------------------------------------------------------------
            void function_library::blit_framebuffer(s32 srcx, s32 srcy, s32 srcWidth, s32 srcHeight, s32 dstx, s32 dsty, s32 dstWidth, s32 dstHeight, u32 mask, u32 filter)
            {
                GL_LOG("glBlitFramebuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tsource x: {0}", srcx);
                GL_LOG("\tsource y: {0}", srcy);
                GL_LOG("\tsource width: {0}", srcWidth);
                GL_LOG("\tsource height: {0}", srcHeight);

                GL_LOG("\tdestination x: {0}", dstx);
                GL_LOG("\tdestination y: {0}", dsty);
                GL_LOG("\tdestination width: {0}", dstWidth);
                GL_LOG("\tdestination height: {0}", dstHeight);

                GL_LOG("\tmask: {0}", blit_framebuffer_mask(mask));
                GL_LOG("\tfilter: {0}", blit_framebuffer_filter(filter));
#endif
                GL_CALL(glBlitFramebuffer(srcx, srcy, srcWidth, srcHeight, dstx, dsty, dstWidth, dstHeight, mask, filter));
            }
            //-------------------------------------------------------------------------
            void function_library::framebuffer_texture2D(u32 target, u32 attachment, u32 textureTarget, u32 texture, s32 level)
            {
                GL_LOG("glFramebufferTexture2D");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
                GL_LOG("\tattachment: {0}", attachment);
                GL_LOG("\ttexture target: {0}", texture_target_to_string(textureTarget));
                GL_LOG("\ttexture: {0}", texture);
                GL_LOG("\tlevel: {0}", level);
#endif

                GL_CALL(glFramebufferTexture2D(target, attachment, textureTarget, texture, level));
            }
            //-------------------------------------------------------------------------
            void function_library::draw_buffer(u32 target)
            {
                GL_LOG("glDrawBuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", drawbuffer_target_to_string(target));
#endif

                GL_CALL(glDrawBuffer(target));
            }
            //-------------------------------------------------------------------------
            void function_library::draw_buffers(u64 count, const u32* buffers)
            {
                GL_LOG("glDrawBuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(buffers));
#endif

                GL_CALL(glDrawBuffers(gsl::narrow<GLsizei>(count), buffers));
            }

            //-------------------------------------------------------------------------
            // Renderbuffers
            //-------------------------------------------------------------------------
            void function_library::generate_renderbuffers(u64 count, u32* renderbuffers)
            {
                GL_LOG("glGenRenderbuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(renderbuffers));
#endif

                glGenRenderbuffers(gsl::narrow<GLsizei>(count), renderbuffers);
            }
            //-------------------------------------------------------------------------
            void function_library::bind_renderbuffer(u32 target, s32 renderbuffer)
            {
                assert(target == GL_RENDERBUFFER && "Target must be GL_RENDERBUFFER!");

                GL_LOG("glBindRenderBuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", renderbuffer_target_to_string(target));
                GL_LOG("\trenderbuffer: {0}", renderbuffer);
#endif
                glBindRenderbuffer(target, renderbuffer);
            }
            //-------------------------------------------------------------------------
            void function_library::renderbuffer_storage(u32 target, u32 internal_format, u64 width, u64 height)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for renderbuffer size");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for renderbuffer size");

                assert(target == GL_RENDERBUFFER && "Target must be GL_RENDERBUFFER!");

                GL_LOG("glRenderbufferStorage");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", renderbuffer_target_to_string(target));
                GL_LOG("\tinternal format: {0}", internal_format_to_string(internal_format));
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
#endif
                glRenderbufferStorage(target, internal_format, gsl::narrow<GLsizei>(width), gsl::narrow<GLsizei>(height));
            }
            //-------------------------------------------------------------------------
            void function_library::framebuffer_renderbuffer(u32 target, u32 attachement, u32 renderbuffer_target, u32 renderbuffer)
            {
                assert(renderbuffer_target == GL_RENDERBUFFER && "Target must be GL_RENDERBUFFER!");

                GL_LOG("glFramebufferRenderbuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
                GL_LOG("\tattachment: {0}", framebuffer_attachment_to_string(attachement));
                GL_LOG("\trenderbuffer_target: {0}", renderbuffer_target_to_string(renderbuffer_target));
                GL_LOG("\trenderbuffer: {0}", renderbuffer);
#endif
                glFramebufferRenderbuffer(target, attachement, renderbuffer_target, renderbuffer);
            }
            //-------------------------------------------------------------------------
            void function_library::delete_renderbuffers(u64 count, const u32* renderbuffers)
            {
                GL_LOG("glDeleteRenderbuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(renderbuffers));
#endif
                glDeleteRenderbuffers(gsl::narrow<GLsizei>(count), renderbuffers);
            }

            //-------------------------------------------------------------------------
            void function_library::delete_buffers(u64 count, const u32* buffers)
            {
                GL_LOG("glDeleteBuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(buffers));
#endif

                GL_CALL(glDeleteBuffers(gsl::narrow<GLsizei>(count), buffers));
            }
            //-------------------------------------------------------------------------
            void function_library::generate_buffers(u64 count, u32* buffers)
            {
                GL_LOG("glGenBuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(buffers));
#endif

                GL_CALL(glGenBuffers(gsl::narrow<GLsizei>(count), buffers));
            }
            //-------------------------------------------------------------------------
            void function_library::bind_buffer(u32 target, u32 index)
            {
                GL_LOG("glBindBuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\tindex: {0}", index);
#endif

                GL_CALL(glBindBuffer(target, index));
            }
            //-------------------------------------------------------------------------
            void function_library::bind_buffer_base(u32 target, u32 index, u32 buffer)
            {
                GL_LOG("glBindBufferBase");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tbuffer: {0}", buffer);
#endif

                GL_CALL(glBindBufferBase(target, index, buffer));
            }
            //-------------------------------------------------------------------------
            void function_library::buffer_data(u32 target, u32 size, const void* data, u32 usage)
            {
                GL_LOG("glBufferData");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\tdata: {0}", fmt::ptr(data));
                GL_LOG("\tusage: {0}", buffer_usage_to_string(usage));
#endif

                GL_CALL(glBufferData(target, size, data, usage));
            }
            //-------------------------------------------------------------------------
            void function_library::buffer_sub_data(u32 target, s64 offset, s64 size, const void* data)
            {
                GL_LOG("glBufferSubData");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\toffset: {0}", offset);
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\tdata: {0}", fmt::ptr(data));
#endif

                GL_CALL(glBufferSubData(target, offset, size, data));
            }

            //-------------------------------------------------------------------------
            // Textures
            //-------------------------------------------------------------------------
            void function_library::delete_textures(u64 count, const u32* textures)
            {
                GL_LOG("glDeleteTextures");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttextures: {0}", fmt::ptr(textures));
#endif

                GL_CALL(glDeleteTextures(gsl::narrow<GLsizei>(count), textures));
            }
            //-------------------------------------------------------------------------
            void function_library::generate_textures(u64 count, u32* textures)
            {
                GL_LOG("glGenTextures");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttextures: {0}", fmt::ptr(textures));
#endif

                GL_CALL(glGenTextures(gsl::narrow<GLsizei>(count), textures));
            }
            //-------------------------------------------------------------------------
            void function_library::activate_texture(u32 texture)
            {
                GL_LOG("glActiveTexture");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttexture: {0}", texture);
#endif

                GL_CALL(glActiveTexture(texture));
            }
            //-------------------------------------------------------------------------
            void function_library::bind_texture(u32 target, u32 texture)
            {
                GL_LOG("glBindTexture");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\ttexture: {0}", texture);
#endif

                GL_CALL(glBindTexture(target, texture));
            }
            //-------------------------------------------------------------------------
            void function_library::texture_image_2D(u32 target, s32 level, s32 internalformat, u64 width, u64 height, s32 border, u32 format, u32 type, const void* data)
            {
                GL_LOG("glTexImage2D");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tlevel: {0}", level);
                GL_LOG("\tinternal format: {0}", internal_format_to_string(internalformat));
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
                GL_LOG("\tborder: {0}", border);
                GL_LOG("\tformat: {0}", format_to_string(format));
                GL_LOG("\ttype: {0}", pixeltype_to_string(type));
                GL_LOG("\tdata: {0}", fmt::ptr(data));
#endif

                GL_CALL(glTexImage2D(target, level, internalformat, gsl::narrow<GLsizei>(width), gsl::narrow<GLsizei>(height), border, format, type, data));
            }
            //-------------------------------------------------------------------------
            void function_library::texture_sub_image_2D(u32 target, s32 level, s32 xoffset, s32 yoffset, u64 width, u64 height, u32 format, u32 type, const void* data)
            {
                GL_LOG("glTexSubImage2D");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tlevel: {0}", level);
                GL_LOG("\txoffset: {0}", xoffset);
                GL_LOG("\tyoffset: {0}", yoffset);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
                GL_LOG("\tformat: {0}", format_to_string(format));
                GL_LOG("\ttype: {0}", pixeltype_to_string(type));
                GL_LOG("\tdata: {0}", fmt::ptr(data));
#endif

                GL_CALL(glTexSubImage2D(target, level, xoffset, yoffset, gsl::narrow<GLsizei>(width), gsl::narrow<GLsizei>(height), format, type, data));
            }
            //-------------------------------------------------------------------------
            void function_library::set_texture_integer_parameter(u32 target, u32 pname, s32 param)
            {
                GL_LOG("glTexParameteri");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tpname: {0}", pname_to_string(pname));
                GL_LOG("\tparam: {0}", param);
#endif

                GL_CALL(glTexParameteri(target, pname, param));
            }
            //-------------------------------------------------------------------------
            void function_library::set_texture_float_array_parameter(u32 target, u32 pname, f32* param)
            {
                GL_LOG("glTexParameterfv");
                #if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tpname: {0}", pname_to_string(pname));
                GL_LOG("\tparam: {0}", param);
                #endif

                GL_CALL(glTexParameterfv(target, pname, param));
            }

            //-------------------------------------------------------------------------
            // Shaders
            //-------------------------------------------------------------------------
            u32 function_library::create_shader(u32 shaderType)
            {
                GL_LOG("glCreateShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshaderType: {0}", shadertype_to_string(shaderType));
#endif

                u32 shader_id = 0;

                GL_CALL(shader_id = glCreateShader(shaderType));

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader id: {0}", shader_id);
#endif

                return shader_id;
            }
            //-------------------------------------------------------------------------
            void function_library::delete_shader(u32 shader)
            {
                GL_LOG("glDeleteShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
#endif

                GL_CALL(glDeleteShader(shader));
            }
            //-------------------------------------------------------------------------
            void function_library::shader_source(u32 shader, u64 count, const char8** code, const s32* length)
            {
                GL_LOG("glShaderSource");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tcode: {0}", fmt::ptr(code));
                GL_LOG("\tlength: {0}", fmt::ptr(length));
#endif

                GL_CALL(glShaderSource(shader, gsl::narrow<GLsizei>(count), (const GLchar* const*)code, length));
            }
            //-------------------------------------------------------------------------
            void function_library::compile_shader(u32 shader)
            {
                GL_LOG("glCompileShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
#endif

                GL_CALL(glCompileShader(shader));
            }
            //-------------------------------------------------------------------------
            void function_library::attach_shader(u32 program, u32 shader)
            {
                GL_LOG("glAttachShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tshader: {0}", shader);
#endif

                GL_CALL(glAttachShader(program, shader));
            }
            //-------------------------------------------------------------------------
            void function_library::detach_shader(u32 program, u32 shader)
            {
                GL_LOG("glDetachShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tshader: {0}", shader);
#endif

                GL_CALL(glDetachShader(program, shader));
            }
            
            //-------------------------------------------------------------------------
            void function_library::get_shader_integer_value(u32 shader, u32 pname, s32* params)
            {
                GL_LOG("glGetShaderiv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
                GL_LOG("\tpname: {0}", pname_to_string(pname));
#endif

                GL_CALL(glGetShaderiv(shader, pname, params));

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tparams result: {0}", *params);
#endif
            }
            //-------------------------------------------------------------------------
            void function_library::get_shader_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog)
            {
                GL_LOG("glGetShaderInfoLog");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
                GL_LOG("\tmaxLength: {0}", maxLength);
                GL_LOG("\tlength: {0}", fmt::ptr(length));
                GL_LOG("\tinfoLog: {0}", fmt::ptr(infoLog));
#endif

                GL_CALL(glGetShaderInfoLog(shader, gsl::narrow<GLsizei>(maxLength), length, (GLchar*)infoLog));
            }

            //-------------------------------------------------------------------------
            // Program
            //-------------------------------------------------------------------------
            void function_library::use_program(u32 program)
            {
                GL_LOG("glUseProgram");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
#endif

                GL_CALL(glUseProgram(program));
            }
            //-------------------------------------------------------------------------
            void function_library::delete_program(u32 program)
            {
                GL_LOG("glDeleteProgram");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
#endif

                GL_CALL(glDeleteProgram(program));
            }
            //-------------------------------------------------------------------------
            void function_library::link_program(u32 program)
            {
                GL_LOG("glLinkProgram");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
#endif

                GL_CALL(glLinkProgram(program));
            }
            //-------------------------------------------------------------------------
            u32 function_library::create_program()
            {
                GL_LOG("glCreateProgram");

                u32 program_id = 0;

                GL_CALL(program_id = glCreateProgram());

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram id: {0}", program_id);
#endif

                return program_id;
            }
            //-------------------------------------------------------------------------
            void function_library::get_program_integer_value(u32 program, u32 pname, s32* params)
            {
                GL_LOG("glGetProgramiv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tpname: {0}", pname_to_string(pname));
#endif

                GL_CALL(glGetProgramiv(program, pname, params));

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tparams: {0}", *params);
#endif
            }           
            //-------------------------------------------------------------------------
            void function_library::get_program_info_log(u32 program, u64 maxLength, s32* length, char8* infoLog)
            {
                GL_LOG("glGetProgramInfoLog");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tmax length: {0}", maxLength);
                GL_LOG("\tlength: {0}", fmt::ptr(length));
                GL_LOG("\tinfoLog: {0}", fmt::ptr(infoLog));
#endif

                GL_CALL(glGetProgramInfoLog(program, gsl::narrow<GLsizei>(maxLength), length, (GLchar*)infoLog));
            }

            //-------------------------------------------------------------------------
            // Uniforms
            //-------------------------------------------------------------------------
            s32 function_library::get_uniform_location(u32 program, const char8* name)
            {
                GL_LOG("glGetUniformLocation");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tname: {0}", name);
#endif

                s32 location;
                GL_CALL(location = glGetUniformLocation(program, (const GLchar*)name));

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
#endif

                return location;
            }

            //-------------------------------------------------------------------------
            void function_library::uniform_matrix_2fv(s32 location, u64 count, bool transpose, const f32* value)
            {
                GL_LOG("glUniformMatrix2fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttranspose: {0}", transpose);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniformMatrix2fv(location, gsl::narrow<GLsizei>(count), transpose, value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_matrix_3fv(s32 location, u64 count, bool transpose, const f32* value)
            {
                GL_LOG("glUniformMatrix3fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttranspose: {0}", transpose);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniformMatrix3fv(location, gsl::narrow<GLsizei>(count), transpose, value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_matrix_4fv(s32 location, u64 count, bool transpose, const f32* value)
            {
                GL_LOG("glUniformMatrix4fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttranspose: {0}", transpose);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniformMatrix4fv(location, gsl::narrow<GLsizei>(count), transpose, value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_1f(s32 location, f32 value)
            {
                GL_LOG("glUniform1f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tvalue: {0}", value);
#endif

                GL_CALL(glUniform1f(location, value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_2f(s32 location, f32 x, f32 y)
            {
                GL_LOG("glUniform2f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
#endif

                GL_CALL(glUniform2f(location, x, y));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_3f(s32 location, f32 x, f32 y, f32 z)
            {
                GL_LOG("glUniform3f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
#endif

                GL_CALL(glUniform3f(location, x, y, z));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_4f(s32 location, f32 x, f32 y, f32 z, f32 w)
            {
                GL_LOG("glUniform4f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
                GL_LOG("\tw: {0}", w);
#endif

                GL_CALL(glUniform4f(location, x, y, z, w));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_1fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform1fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniform1fv(location, gsl::narrow<GLsizei>(count), value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_2fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform2fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniform2fv(location, gsl::narrow<GLsizei>(count), value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_3fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform3fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniform3fv(location, gsl::narrow<GLsizei>(count), value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_4fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform4fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniform4fv(location, gsl::narrow<GLsizei>(count), value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_1i(s32 location, s32 value)
            {
                GL_LOG("glUniform1i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tvalue: {0}", value);
#endif

                GL_CALL(glUniform1i(location, value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_2i(s32 location, s32 x, s32 y)
            {
                GL_LOG("glUniform2i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
#endif

                GL_CALL(glUniform2i(location, x, y));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_3i(s32 location, s32 x, s32 y, s32 z)
            {
                GL_LOG("glUniform3i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
#endif

                GL_CALL(glUniform3i(location, x, y, z));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_4i(s32 location, s32 x, s32 y, s32 z, s32 w)
            {
                GL_LOG("glUniform4i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
                GL_LOG("\tw: {0}", w);
#endif

                GL_CALL(glUniform4i(location, x, y, z, w));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_1iv(s32 location, u64 count, const s32* value)
            {
                GL_LOG("glUniform1iv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif

                GL_CALL(glUniform1iv(location, gsl::narrow<GLsizei>(count), value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_1ui(s32 location, u32 value)
            {
                GL_LOG("glUniform1ui");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tvalue: {0}", value);
#endif

                GL_CALL(glUniform1ui(location, value));
            }
            //-------------------------------------------------------------------------
            void function_library::uniform_1uiv(s32 location, u64 count, const u32* value)
            {
                GL_LOG("glUniform1uiv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
                GL_CALL(glUniform1uiv(location, gsl::narrow<GLsizei>(count), value));
            }
#pragma endregion
#pragma region MOCK FUNCTION LIBRARY
            //-------------------------------------------------------------------------
            //-------------------------------------------------------------------------
            // MOCK FUNCTION LIBRARY
            //-------------------------------------------------------------------------
            // General
            //-------------------------------------------------------------------------
            void mock_function_library::viewport(s32 x, s32 y, s32 width, s32 height)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for viewport resize");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for viewport resize");

                GL_LOG("glViewport");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::scissor(s32 x, s32 y, s64 width, s64 height)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for scissor resize");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for scissor resize");

                GL_LOG("glScissor");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::read_pixels(s32 x, s32 y, s32 width, s32 height, u32 format, u32 type, void* data)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for pixel size");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for pixel size");

                GL_LOG("glReadPixels");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
                GL_LOG("\tformat: {0}", format_to_string(format));
                GL_LOG("\ttype: {0}", pixeltype_to_string(type));
#endif
            }

            //-------------------------------------------------------------------------
            void mock_function_library::clear_color(f32 red, f32 green, f32 blue, f32 alpha)
            {
                GL_LOG("glClearColor");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tred: {0}", red);
                GL_LOG("\tgreen: {0}", green);
                GL_LOG("\tblue: {0}", blue);
                GL_LOG("\talpha: {0}", alpha);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::clear_depth(f32 depth)
            {
                GL_LOG("glClearDepth");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tdepth: {0}", depth);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::clear(u32 bitfield)
            {
                GL_LOG("glClear");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tbitfield: {0}", bitfield);
#endif
            }

            //-------------------------------------------------------------------------
            void mock_function_library::enable(u32 value)
            {
                GL_LOG("glEnable");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tvalue: {0}", enable_to_string(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::disable(u32 value)
            {
                GL_LOG("glDisable");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tvalue: {0}", disable_to_string(value));
#endif
            }

            //-------------------------------------------------------------------------
            void mock_function_library::depth_mask(bool flag)
            {
                GL_LOG("glDepthMask");
                #if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tflag: {0}", flag);
                #endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::color_mask(bool red, bool green, bool blue, bool alpha)
            {
                GL_LOG("glColorMask");
                #if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tred: {0}", red);
                GL_LOG("\tgreen: {0}", green);
                GL_LOG("\tblue: {0}", blue);
                GL_LOG("\talpha: {0}", alpha);
                #endif
            }

            //-------------------------------------------------------------------------
            void mock_function_library::blend_func(u32 sfactor, u32 dfactor)
            {
                GL_LOG("glBlendFunc");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tsfactor: {0}", blend_func_factor_to_string(sfactor));
                GL_LOG("\tdfactor: {0}", blend_func_factor_to_string(dfactor));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::cull_face(u32 mode)
            {
                GL_LOG("glCullFace");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", cull_type_to_string(mode));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::front_face(u32 mode)
            {
                GL_LOG("glFrontFace");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", front_face_to_string(mode));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::depth_func(u32 fn)
            {
                GL_LOG("glDepthFunc");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tfunction: {0}", depth_function_to_string(fn));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::polygon_mode(u32 face, u32 mode)
            {
                GL_LOG("glPolygonMode");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tface: {0}", cull_type_to_string(face));
                GL_LOG("\tmode: {0}", fill_mode_to_string(mode));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::line_width(f32 width)
            {
                GL_LOG("glLineWidth");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\twidth: {0}", width);
#endif
            }

            //-------------------------------------------------------------------------
            // Vertex Arrays
            //-------------------------------------------------------------------------
            void mock_function_library::enable_vertex_attrib_array(u32 index)
            {
                GL_LOG("glEnableVertexAttribArray");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::vertex_attrib_i_pointer(u32 index, s32 size, u32 type, u64 stride, const void* ptr)
            {
                GL_LOG("glVertexAttribIPointer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tstride: {0}", stride);
                GL_LOG("\tpointer: {0}", ptr);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::vertex_attrib_pointer(u32 index, s32 size, u32 type, bool normalized, u64 stride, const void* ptr)
            {
                GL_LOG("glVertexAttribPointer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tnormalized: {0}", normalized);
                GL_LOG("\tstride: {0}", stride);
                GL_LOG("\tpointer: {0}", ptr);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::delete_vertex_arrays(u64 count, const u32* arrays)
            {
                GL_LOG("glDeleteVertexArrays");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tarrays: {0}", fmt::ptr(arrays));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::generate_vertex_arrays(u64 count, u32* arrays)
            {
                GL_LOG("glGenVertexArrays");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tarrays: {0}", fmt::ptr(arrays));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::bind_vertex_array(u32 arrayID)
            {
                GL_LOG("glBindVertexArray");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tarray: {0}", arrayID);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::vertex_attrib_divisor(u32 index, u32 divisor)
            {
                GL_LOG("glVertexAttribDivisor");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tdivisor: {0}", divisor);
#endif
            }

            //-------------------------------------------------------------------------
            // Drawing
            //-------------------------------------------------------------------------
            void mock_function_library::draw_elements(u32 mode, u64 count, u32 type, const void* indices)
            {
                GL_LOG("glDrawElements");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tindices: {0}", fmt::ptr(indices));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::draw_arrays(u32 mode, s32 first, u64 count)
            {
                GL_LOG("glDrawArrays");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tfirst: {0}", first);
                GL_LOG("\tcount: {0}", count);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::draw_elements_instanced(u32 mode, u64 count, u32 type, const void* indices, u64 instance_count)
            {
                GL_LOG("glDrawElementsInstanced");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttype: {0}", type);
                GL_LOG("\tindices: {0}", fmt::ptr(indices));
                GL_LOG("\tinstance_count: {0}", instance_count);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::draw_arrays_instanced(u32 mode, s32 first, u64 count, u64 instance_count)
            {
                GL_LOG("glDrawArraysInstanced");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tmode: {0}", mode);
                GL_LOG("\tfirst: {0}", first);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tinstance_count: {0}", instance_count);
#endif
            }

            //-------------------------------------------------------------------------
            // Device State
            //-------------------------------------------------------------------------
            const char* mock_function_library::get_string_value(u32 name)
            {
                GL_LOG("glGetString");

                const char* s = nullptr;

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\toutput data: {0}", *s);
#endif

                return (const char*)s;
            }
            //-------------------------------------------------------------------------
            void mock_function_library::get_integer_value(u32 pname, s32* data)
            {
                GL_LOG("glGetIntegerv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tpname: {0}", pname_to_string(pname));

                GL_LOG("\toutput data: {0}", *data);
#endif
            }
            //-------------------------------------------------------------------------
            u32 mock_function_library::get_error()
            {
#if ENABLE_GL_FUNCTION_LOGGING && ENABLE_GL_GET_ERROR_LOGGING
                GL_LOG("glGetError");
#endif

                u32 error = -1;

#if ENABLE_GL_FUNCTION_LOGGING && ENABLE_GL_GET_ERROR_LOGGING && ENABLE_GL_PARAMETER_LOGGING
                GL_LOG("\toutput data: {0}", error);
#endif

                return error;
            }

            //-------------------------------------------------------------------------
            // Framebuffers
            //-------------------------------------------------------------------------
            u32 mock_function_library::check_framebuffer_status(u32 target)
            {
                GL_LOG("glCheckFramebufferStatus");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
#endif

                u32 result = -1;

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\toutput result: {0}", result);
#endif

                return result;
            }

            //-------------------------------------------------------------------------
            void mock_function_library::delete_framebuffers(u64 count, const u32* framebuffers)
            {
                GL_LOG("glDeleteFramebuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tframebuffers: {0}", fmt::ptr(framebuffers));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::generate_framebuffers(u64 count, u32* framebuffers)
            {
                GL_LOG("glGenFramebuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tframebuffers: {0}", fmt::ptr(framebuffers));
#endif

                GL_CALL(glGenFramebuffers(gsl::narrow<GLsizei>(count), framebuffers));
            }
            //-------------------------------------------------------------------------
            void mock_function_library::bind_framebuffer(u32 target, s32 index)
            {
                GL_LOG("glBindFramebuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
                GL_LOG("\tindex: {0}", index);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::blit_framebuffer(s32 srcx, s32 srcy, s32 srcWidth, s32 srcHeight, s32 dstx, s32 dsty, s32 dstWidth, s32 dstHeight, u32 mask, u32 filter)
            {
                GL_LOG("glBlitFramebuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tsource x: {0}", srcx);
                GL_LOG("\tsource y: {0}", srcy);
                GL_LOG("\tsource width: {0}", srcWidth);
                GL_LOG("\tsource height: {0}", srcHeight);

                GL_LOG("\tdestination x: {0}", dstx);
                GL_LOG("\tdestination y: {0}", dsty);
                GL_LOG("\tdestination width: {0}", dstWidth);
                GL_LOG("\tdestination height: {0}", dstHeight);

                GL_LOG("\tmask: {0}", blit_framebuffer_mask(mask));
                GL_LOG("\tfilter: {0}", blit_framebuffer_filter(filter));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::framebuffer_texture2D(u32 target, u32 attachment, u32 textureTarget, u32 texture, s32 level)
            {
                GL_LOG("glFramebufferTexture2D");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
                GL_LOG("\tattachment: {0}", attachment);
                GL_LOG("\ttexture target: {0}", texture_target_to_string(textureTarget));
                GL_LOG("\ttexture: {0}", texture);
                GL_LOG("\tlevel: {0}", level);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::draw_buffer(u32 target)
            {
                GL_LOG("glDrawBuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", drawbuffer_target_to_string(target));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::draw_buffers(u64 count, const u32* buffers)
            {
                GL_LOG("glDrawBuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(buffers));
#endif
            }

            //-------------------------------------------------------------------------
            // Renderbuffers
            //-------------------------------------------------------------------------
            void mock_function_library::generate_renderbuffers(u64 count, u32* renderbuffers)
            {
                GL_LOG("glGenRenderbuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(renderbuffers));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::bind_renderbuffer(u32 target, s32 renderbuffer)
            {
                assert(target == GL_RENDERBUFFER && "Target must be GL_RENDERBUFFER!");

                GL_LOG("glBindRenderBuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", renderbuffer_target_to_string(target));
                GL_LOG("\trenderbuffer: {0}", renderbuffer);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::renderbuffer_storage(u32 target, u32 internal_format, u64 width, u64 height)
            {
                assert(width > 0 && width < (s32)std::numeric_limits<u16>().max() && "Invalid width given for renderbuffer size");
                assert(height > 0 && height < (s32)std::numeric_limits<u16>().max() && "Invalid height given for renderbuffer size");

                assert(target == GL_RENDERBUFFER && "Target must be GL_RENDERBUFFER!");

                GL_LOG("glRenderbufferStorage");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", renderbuffer_target_to_string(target));
                GL_LOG("\tinternal format: {0}", internal_format_to_string(internal_format));
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::framebuffer_renderbuffer(u32 target, u32 attachement, u32 renderbuffer_target, u32 renderbuffer)
            {
                assert(renderbuffer_target == GL_RENDERBUFFER && "Target must be GL_RENDERBUFFER!");

                GL_LOG("glFramebufferRenderbuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", framebuffer_target_to_string(target));
                GL_LOG("\tattachment: {0}", framebuffer_attachment_to_string(attachement));
                GL_LOG("\trenderbuffer_target: {0}", renderbuffer_target_to_string(renderbuffer_target));
                GL_LOG("\trenderbuffer: {0}", renderbuffer);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::delete_renderbuffers(u64 count, const u32* renderbuffers)
            {
                GL_LOG("glDeleteRenderbuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(renderbuffers));
#endif
            }

            //-------------------------------------------------------------------------
            void mock_function_library::delete_buffers(u64 count, const u32* buffers)
            {
                GL_LOG("glDeleteBuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(buffers));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::generate_buffers(u64 count, u32* buffers)
            {
                GL_LOG("glGenBuffers");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tbuffers: {0}", fmt::ptr(buffers));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::bind_buffer(u32 target, u32 index)
            {
                GL_LOG("glBindBuffer");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\tindex: {0}", index);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::bind_buffer_base(u32 target, u32 index, u32 buffer)
            {
                GL_LOG("glBindBufferBase");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\tindex: {0}", index);
                GL_LOG("\tbuffer: {0}", buffer);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::buffer_data(u32 target, u32 size, const void* data, u32 usage)
            {
                GL_LOG("glBufferData");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\tdata: {0}", fmt::ptr(data));
                GL_LOG("\tusage: {0}", buffer_usage_to_string(usage));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::buffer_sub_data(u32 target, s64 offset, s64 size, const void* data)
            {
                GL_LOG("glBufferSubData");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", buffer_target_to_string(target));
                GL_LOG("\toffset: {0}", offset);
                GL_LOG("\tsize: {0}", size);
                GL_LOG("\tdata: {0}", fmt::ptr(data));
#endif
            }

            //-------------------------------------------------------------------------
            // Textures
            //-------------------------------------------------------------------------
            void mock_function_library::delete_textures(u64 count, const u32* textures)
            {
                GL_LOG("glDeleteTextures");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttextures: {0}", fmt::ptr(textures));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::generate_textures(u64 count, u32* textures)
            {
                GL_LOG("glGenTextures");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttextures: {0}", fmt::ptr(textures));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::activate_texture(u32 texture)
            {
                GL_LOG("glActiveTexture");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttexture: {0}", texture);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::bind_texture(u32 target, u32 texture)
            {
                GL_LOG("glBindTexture");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\ttexture: {0}", texture);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::texture_image_2D(u32 target, s32 level, s32 internalformat, u64 width, u64 height, s32 border, u32 format, u32 type, const void* data)
            {
                GL_LOG("glTexImage2D");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tlevel: {0}", level);
                GL_LOG("\tinternal format: {0}", internal_format_to_string(internalformat));
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
                GL_LOG("\tborder: {0}", border);
                GL_LOG("\tformat: {0}", format_to_string(format));
                GL_LOG("\ttype: {0}", pixeltype_to_string(type));
                GL_LOG("\tdata: {0}", fmt::ptr(data));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::texture_sub_image_2D(u32 target, s32 level, s32 xoffset, s32 yoffset, u64 width, u64 height, u32 format, u32 type, const void* data)
            {
                GL_LOG("glTexSubImage2D");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tlevel: {0}", level);
                GL_LOG("\txoffset: {0}", xoffset);
                GL_LOG("\tyoffset: {0}", yoffset);
                GL_LOG("\twidth: {0}", width);
                GL_LOG("\theight: {0}", height);
                GL_LOG("\tformat: {0}", format_to_string(format));
                GL_LOG("\ttype: {0}", pixeltype_to_string(type));
                GL_LOG("\tdata: {0}", fmt::ptr(data));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::set_texture_integer_parameter(u32 target, u32 pname, s32 param)
            {
                GL_LOG("glTexParameteri");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tpname: {0}", pname_to_string(pname));
                GL_LOG("\tparam: {0}", param);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::set_texture_float_array_parameter(u32 target, u32 pname, f32* param)
            {
                GL_LOG("glTexParameterfv");
                #if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\ttarget: {0}", texture_target_to_string(target));
                GL_LOG("\tpname: {0}", pname_to_string(pname));
                GL_LOG("\tparam: {0}", param);
                #endif
            }

            //-------------------------------------------------------------------------
            // Shaders
            //-------------------------------------------------------------------------
            u32 mock_function_library::create_shader(u32 shaderType)
            {
                GL_LOG("glCreateShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshaderType: {0}", shadertype_to_string(shaderType));
#endif

                u32 shader_id = -1;

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader id: {0}", shader_id);
#endif

                return shader_id;
            }
            //-------------------------------------------------------------------------
            void mock_function_library::delete_shader(u32 shader)
            {
                GL_LOG("glDeleteShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::shader_source(u32 shader, u64 count, const char8** code, const s32* length)
            {
                GL_LOG("glShaderSource");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tcode: {0}", fmt::ptr(code));
                GL_LOG("\tlength: {0}", fmt::ptr(length));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::compile_shader(u32 shader)
            {
                GL_LOG("glCompileShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::attach_shader(u32 program, u32 shader)
            {
                GL_LOG("glAttachShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tshader: {0}", shader);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::detach_shader(u32 program, u32 shader)
            {
                GL_LOG("glDetachShader");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tshader: {0}", shader);
#endif
            }

            //-------------------------------------------------------------------------
            void mock_function_library::get_shader_integer_value(u32 shader, u32 pname, s32* params)
            {
                GL_LOG("glGetShaderiv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
                GL_LOG("\tpname: {0}", pname_to_string(pname));

                GL_LOG("\toutput params: {0}", *params);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::get_shader_info_log(u32 shader, u64 maxLength, s32* length, char8* infoLog)
            {
                GL_LOG("glGetShaderInfoLog");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tshader: {0}", shader);
                GL_LOG("\tmaxLength: {0}", maxLength);
                GL_LOG("\tlength: {0}", fmt::ptr(length));
                GL_LOG("\tinfoLog: {0}", fmt::ptr(infoLog));
#endif
            }

            //-------------------------------------------------------------------------
            // Program
            //-------------------------------------------------------------------------
            void mock_function_library::use_program(u32 program)
            {
                GL_LOG("glUseProgram");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::delete_program(u32 program)
            {
                GL_LOG("glDeleteProgram");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::link_program(u32 program)
            {
                GL_LOG("glLinkProgram");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
#endif
            }
            //-------------------------------------------------------------------------
            u32 mock_function_library::create_program()
            {
                GL_LOG("glCreateProgram");

                u32 program_id = -1;

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\toutput program id: {0}", program_id);
#endif
                return program_id;
            }
            //-------------------------------------------------------------------------
            void mock_function_library::get_program_integer_value(u32 program, u32 pname, s32*)
            {
                GL_LOG("glGetProgramiv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tpname: {0}", pname_to_string(pname));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::get_program_info_log(u32 program, u64 maxLength, s32* length, char8* infoLog)
            {
                GL_LOG("glGetProgramInfoLog");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tmax length: {0}", maxLength);
                GL_LOG("\tlength: {0}", fmt::ptr(length));
                GL_LOG("\tinfoLog: {0}", fmt::ptr(infoLog));
#endif
            }

            //-------------------------------------------------------------------------
            // Uniforms
            //-------------------------------------------------------------------------
            s32 mock_function_library::get_uniform_location(u32 program, const char8* name)
            {
                GL_LOG("glGetUniformLocation");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tprogram: {0}", program);
                GL_LOG("\tname: {0}", name);
#endif

                s32 location = -1;

#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\toutput location: {0}", location);
#endif
                return location;
            }

            //-------------------------------------------------------------------------
            void mock_function_library::uniform_matrix_2fv(s32 location, u64 count, bool transpose, const f32* value)
            {
                GL_LOG("glUniformMatrix2fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttranspose: {0}", transpose);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_matrix_3fv(s32 location, u64 count, bool transpose, const f32* value)
            {
                GL_LOG("glUniformMatrix3fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttranspose: {0}", transpose);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_matrix_4fv(s32 location, u64 count, bool transpose, const f32* value)
            {
                GL_LOG("glUniformMatrix4fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\ttranspose: {0}", transpose);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_1f(s32 location, f32 value)
            {
                GL_LOG("glUniform1f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tvalue: {0}", value);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_2f(s32 location, f32 x, f32 y)
            {
                GL_LOG("glUniform2f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_3f(s32 location, f32 x, f32 y, f32 z)
            {
                GL_LOG("glUniform3f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_4f(s32 location, f32 x, f32 y, f32 z, f32 w)
            {
                GL_LOG("glUniform4f");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
                GL_LOG("\tw: {0}", w);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_1fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform1fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_2fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform2fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_3fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform3fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_4fv(s32 location, u64 count, const f32* value)
            {
                GL_LOG("glUniform4fv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_1i(s32 location, s32 value)
            {
                GL_LOG("glUniform1i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tvalue: {0}", value);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_2i(s32 location, s32 x, s32 y)
            {
                GL_LOG("glUniform2i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_3i(s32 location, s32 x, s32 y, s32 z)
            {
                GL_LOG("glUniform3i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_4i(s32 location, s32 x, s32 y, s32 z, s32 w)
            {
                GL_LOG("glUniform4i");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tx: {0}", x);
                GL_LOG("\ty: {0}", y);
                GL_LOG("\tz: {0}", z);
                GL_LOG("\tw: {0}", w);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_1iv(s32 location, u64 count, const s32* value)
            {
                GL_LOG("glUniform1iv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_1ui(s32 location, u32 value)
            {
                GL_LOG("glUniform1ui");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tvalue: {0}", value);
#endif
            }
            //-------------------------------------------------------------------------
            void mock_function_library::uniform_1uiv(s32 location, u64 count, const u32* value)
            {
                GL_LOG("glUniform1uiv");
#if ENABLE_GL_PARAMETER_LOGGING && ENABLE_GL_FUNCTION_LOGGING
                GL_LOG("\tlocation: {0}", location);
                GL_LOG("\tcount: {0}", count);
                GL_LOG("\tvalue: {0}", fmt::ptr(value));
#endif
            }

#pragma endregion
        }
    }
}