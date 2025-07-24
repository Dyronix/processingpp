add_library(processing_engine STATIC)

# Sources
target_sources(processing_engine PRIVATE 
    # fileio
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/fileio.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/fileio.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/vfs.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/vfs.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/file_info.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/file_info.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/system_paths.h
    # resources
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/font_atlas.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/font_atlas.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/font_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/font_pool.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/shader_pool.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/shader_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/texture_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/texture_pool.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/texture_reloader.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/texture_reloader.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/geometry_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/geometry_pool.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/material.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/material.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/material_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/material_pool.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/framebuffer_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/framebuffer_pool.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/lights_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/resources/lights_pool.cpp
    # camera
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/camera/camera_manager.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/camera/camera_manager.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/camera/camera_context.h
    # device
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/glfw/device_glfw.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/glfw/device_glfw_input.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/device.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/device/device_input.h
    # render
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_batch_renderer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_instance_renderer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_batch.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_instance.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_types.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_shader.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_shader_tags.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_shader_compiler.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_shader_library.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_shader_uniform_manager.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_item.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_storage_buffer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_vertex_buffer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_index_buffer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_instance_buffer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_features.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_framebuffer.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_framebuffer_flags.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_context.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_scissor.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pipeline.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_batch_data_table.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_instance_data_table.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_batch_render_strategy.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_instance_render_strategy.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_ui.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_unlit.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_unlit_wireframe.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_predepth.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_clear.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_shadow.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_forward_shading.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_blit.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_composite.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/render_pass_composite_factory.h
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
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_unlit.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_unlit_wireframe.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_pass_predepth.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_util.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/opengl/render_gl_util.cpp
    ## render-helpers
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_index_buffer_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_index_buffer_ops.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_vertex_buffer_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_vertex_buffer_ops.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_instance_buffer_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_instance_buffer_ops.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_storage_buffer_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_storage_buffer_ops.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_texture_registry.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_texture_registry.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_vertex_layouts.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_vertex_layouts.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_instance_layouts.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_instance_layouts.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_event_dispatcher.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/render/helpers/render_event_dispatcher.h  
    # geometry
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/geometry.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/geometry.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/geometry_helpers.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/geometry_helpers.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/geometry_bounding_box.h
    # geometry-3d
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/box.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/box.cpp   
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/plane.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/plane.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/sphere.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/sphere.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/cone.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/truncated_cone.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/cone.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/truncated_cone.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/torus.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/torus.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/octahedron.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/octahedron.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/tetrahedron.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/tetrahedron.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/cylinder.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/cylinder.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/point.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/3d/point.cpp
    # geometry-2d
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/point.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/point.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/line.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/line.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/ellipse.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/ellipse.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/polygon.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/polygon.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/triangle.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/triangle.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/rectangle.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/rectangle.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/geometry/2d/geometry_2d_helpers.h
    # model
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/model/parse_obj.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/model/parse_obj.cpp
    # string
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/string/string_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/string/string_conversions.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/string/string_id.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/string/string_id.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/string/string_pool.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/string/string_pool.cpp
    # util
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/brush.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/brush.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/transform_stack.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/transform_stack.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/hash.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/log.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/color_ops.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/color_ops.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/types.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/perlin_noise.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/perlin_noise.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/steady_clock.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/steady_clock.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/pointer_math.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/profiler.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/util/profiler.cpp
    # engine
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/camera.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/shapes.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/random.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/transform.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/color.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/environment.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/structure.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/constants.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/rendering.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/events.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/data.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/mathematics.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/material.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/trigonometry.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/typography.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/image.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/model.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/lights.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/input_output.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/time_date.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/json.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/entrypoint.cpp)

 # fileio-win32
if(WIN32)
target_sources(processing_engine PRIVATE 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private/fileio/win32/win32_system_paths.cpp)
endif()

target_sources(processing_engine PUBLIC 
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/camera.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/matrix.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/shapes.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/random.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/transform.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/color.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/environment.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/conversions.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/structure.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/constants.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/rendering.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/events.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/data.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/mathematics.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/material.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/trigonometry.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/typography.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/image.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/model.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/engine.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/lights.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/vector.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/time_date.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/input_output.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/json.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public/entrypoint.h)

# Create the project filters
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private private)
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public public)

# Public includes
target_include_directories(processing_engine PUBLIC ${SOURCE_THIRDPARTY_DIRECTORY}/nlohmann/include)
target_include_directories(processing_engine PUBLIC ${SOURCE_THIRDPARTY_DIRECTORY}/glm)

# Private includes
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/noise/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/stb/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/fmt/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/stb/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glad/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/gsl/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/freetype/include)
target_include_directories(processing_engine PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/inih/include)

target_link_libraries(processing_engine PRIVATE stb)
target_link_libraries(processing_engine PRIVATE glad)
target_link_libraries(processing_engine PRIVATE glfw)
target_link_libraries(processing_engine PRIVATE freetype)
target_link_libraries(processing_engine PRIVATE inih)

# Include self
target_include_directories(processing_engine PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private)
target_include_directories(processing_engine PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_compile_definitions(processing_engine PUBLIC -DGLM_FORCE_CTOR_INIT)
target_compile_definitions(processing_engine PUBLIC -DPPP_OPENGL)
target_compile_definitions(processing_engine PUBLIC $<$<BOOL:${CMAKE_HOST_WIN32}>:PPP_WINDOWS>)

# Target properties
set_target_properties(processing_engine PROPERTIES
                      FOLDER "modules/runtime")