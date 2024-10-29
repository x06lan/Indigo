cmake_minimum_required(VERSION 3.16)
project(Indigo VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_POLICY_DEFAULT_CMP0072 NEW)

find_package(OpenGL REQUIRED)

add_subdirectory(lib)

set(LIB
    ${OPENGL_LIBRARY}
    glfw
    glew_220
    spdlog::spdlog
    glm::glm
    assimp
    ImGui
)

add_executable(Indigo
    src/main.cpp
    src/log.cpp
    src/exception.cpp
    src/window.cpp
    src/renderer.cpp
    src/controller.cpp
    src/program.cpp
    src/spirv.cpp
    src/camera.cpp
    src/importer.cpp
    src/vertex_array.cpp
    src/vertex_buffer.cpp
    src/index_buffer.cpp
    src/uniform_buffer.cpp
    src/frame_buffer.cpp
    src/texture.cpp
    src/transform.cpp
    src/pipeline.cpp
    src/scene.cpp
    src/scene_object.cpp
    src/model.cpp
    src/light.cpp

    include/log.hpp
    include/exception.hpp
    include/window.hpp
    include/renderer.hpp
    include/controller.hpp
    include/program.hpp
    include/spirv.hpp
    include/camera.hpp
    include/importer.hpp
    include/vertex_array.hpp
    include/vertex_buffer.hpp
    include/index_buffer.hpp
    include/uniform_buffer.hpp
    include/frame_buffer.hpp
    include/texture.hpp
    include/transform.hpp
    include/pipeline.hpp
    include/scene.hpp
    include/scene_object.hpp
    include/model.hpp
    include/light.hpp
)
target_link_libraries(Indigo ${LIB})
target_include_directories(Indigo SYSTEM PRIVATE
    lib/glew-2.2.0/include/
    lib/glfw/include/
    lib/spdlog/include/
    lib/glm/include/
    lib/stb/
    lib/assimp/include/
    ${IMGUI_INCLUDE_DIR}
)
target_include_directories(Indigo PRIVATE
    include/
)
target_precompile_headers(Indigo PRIVATE
    include/pch.hpp
)
target_compile_options(Indigo PRIVATE
    -Wall -pedantic
)