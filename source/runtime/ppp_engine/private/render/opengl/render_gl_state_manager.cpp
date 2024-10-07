#include "render/opengl/render_gl_state_manager.h"

namespace ppp
{
    namespace render
    {
        gl_state_manager::gl_state_manager()
            :_blending_enabled(true)
            ,_blending_src_blend(GL_SRC_ALPHA)
            ,_blending_dst_blend(GL_ONE_MINUS_SRC_ALPHA)

            ,_culling_enabled(true)
            ,_culling_mode(GL_BACK)

            ,_depth_testing_enabled(true)
            ,_depth_testing_mode(GL_LESS)

            ,_polygon_face(GL_FRONT_AND_BACK)
            ,_polygon_mode(GL_FILL)
        {
        }

        gl_state_manager::~gl_state_manager() = default;

        gl_state_manager::gl_state_manager(const gl_state_manager& state_manager)
        {
            set_blending(state_manager._blending_enabled, state_manager._blending_src_blend, state_manager._blending_dst_blend);
            set_culling(state_manager._culling_enabled, state_manager._culling_mode);
            set_depth_testing(state_manager._depth_testing_enabled, state_manager._depth_testing_mode);
            set_polygon_mode(state_manager._polygon_face, state_manager._polygon_mode);
        }
        gl_state_manager::gl_state_manager(gl_state_manager&& state_manager) = default;

        gl_state_manager& gl_state_manager::operator=(const gl_state_manager& state_manager)
        {
            set_blending(state_manager._blending_enabled, state_manager._blending_src_blend, state_manager._blending_dst_blend);
            set_culling(state_manager._culling_enabled, state_manager._culling_mode);
            set_depth_testing(state_manager._depth_testing_enabled, state_manager._depth_testing_mode);
            set_polygon_mode(state_manager._polygon_face, state_manager._polygon_mode);

            return *this;
        }
        gl_state_manager& gl_state_manager::operator=(gl_state_manager&& state_manager) = default;

        void gl_state_manager::apply()
        {
            
        }

        void gl_state_manager::set_blending(bool enable, GLenum blending_src, GLenum blending_dst)
        {
            if (_blending_enabled != enable || _blending_src_blend != blending_src || _blending_dst_blend != blending_dst)
            {
                _blending_enabled = enable;
                _blending_src_blend = blending_src;
                _blending_dst_blend = blending_dst;
            }
        }

        void gl_state_manager::set_culling(bool enable, GLenum culling_mode)
        {
            if (_culling_enabled != enable || _culling_mode != culling_mode)
            {
                _culling_enabled = enable;
                _culling_mode = culling_mode;
            }
        }

        void gl_state_manager::set_depth_testing(bool enable, GLenum testing_func)
        {
            if (_depth_testing_enabled != enable || _depth_testing_mode != testing_func)
            {
                _depth_testing_enabled = enable;
                _depth_testing_mode = testing_func;
            }
        }

        void gl_state_manager::set_polygon_mode(GLenum polygon_face, GLenum polygon_mode)
        {
            if (_polygon_face != polygon_face || _polygon_mode != polygon_mode)
            {
                _polygon_face = polygon_face;
                _polygon_mode = polygon_mode;
            }
        }
    }
}