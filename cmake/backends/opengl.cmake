# cmake/backends/opengl.cmake  (public)
# Adds OpenGL renderer + GLFW device to 'processing_engine'

# Sources (copied from your current list)
target_sources(processing_engine PRIVATE
    # device-glfw
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/glfw/device_glfw.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/glfw/device_glfw_input.cpp
    ## render-opengl
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_storage_buffer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_vertex_buffer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_index_buffer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_instance_buffer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_batch.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_instance.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_batch_renderer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_instance_renderer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_scissor.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_shader.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_shader_compiler.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_shader_library.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_shader_uniform_manager.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_error.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_error.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_features.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_framebuffer.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_function_library.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_function_library.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_api.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_api.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pipeline.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_batch_data_table.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_instance_data_table.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_batch_render_strategy.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_instance_render_strategy.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_clear.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_shadow.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_forward_shading.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_blit.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_ui.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_font.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_unlit.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_unlit_wireframe.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_predepth.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_ui_wireframe.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_util.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_util.cpp
)

# Includes and libs only needed for GL/GLFW
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glad/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)

target_link_libraries(processing_engine PRIVATE glad)
target_link_libraries(processing_engine PRIVATE glfw)

# Backend definition
target_compile_definitions(processing_engine PUBLIC -DPPP_OPENGL)
