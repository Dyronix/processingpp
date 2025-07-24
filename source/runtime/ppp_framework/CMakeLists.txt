add_library(processing_framework STATIC)

target_sources(processing_framework PRIVATE 
    #imgui
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_demo.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_draw.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_internal.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_stdlib.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_tables.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_widgets.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_impl_glfw.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_impl_glfw.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_impl_opengl3.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_impl_opengl3.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_impl_opengl3_loader.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/imgui_icons_font_awesome.h
    #imgui-implot
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/implot/implot.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/implot/implot.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/implot/implot_internal.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/implot/implot_items.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/implot/implot_demo.cpp
    #imgui-inspector
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/inspector/inspector.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/inspector/inspector.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/inspector/inspector_profiler.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/imgui/inspector/inspector_profiler.h
    #device
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/device/device_glfw_imgui.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/device/device_glfw_input_framework.cpp
    #render
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/render/render_gl_imgui.cpp

    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/framework.cpp
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private/sketch.cpp)

target_sources(processing_framework PUBLIC 
    #imgui
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui/imconfig.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui/imgui.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui/imgui_stdlib.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui/imstb_rectpack.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui/imstb_textedit.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui/imstb_truetype.h
    #device
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/device/device_imgui.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/device/device_input_framework.h
    #render
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/render/render_imgui.h

    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/framework.h
    ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/sketch.h)

# Create the project filters
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private private)
GROUPSOURCES(${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public public)

# Public includes
target_include_directories(processing_framework PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/public)

target_link_libraries(processing_framework PUBLIC processing_engine)

# Private includes
target_include_directories(processing_framework PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_engine/private)
target_include_directories(processing_framework PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/fmt/include)
target_include_directories(processing_framework PRIVATE ${SOURCE_THIRDPARTY_DIRECTORY}/glfw/include)

target_link_libraries(processing_framework PRIVATE processing_engine)
target_link_libraries(processing_engine PRIVATE glfw)

# Include self
target_include_directories(processing_framework PRIVATE ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/private)
target_include_directories(processing_framework PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public)
target_include_directories(processing_framework PUBLIC ${SOURCE_RUNTIME_DIRECTORY}/ppp_framework/public/imgui)

# Target properties
set_target_properties(processing_framework PROPERTIES
                      FOLDER "modules/runtime")