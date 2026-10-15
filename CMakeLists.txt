cmake_minimum_required(VERSION 3.15)
project(renderer2d CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(renderer2d
  src/core/main.cpp
  src/core/window.cpp
  src/engine/app.cpp
  src/engine/render/renderer.cpp
  src/resources/vertex.hlsl
  src/resources/pixel.hlsl
  src/resources/common.hlsli
)

set_source_files_properties(
    src/resources/vertex.hlsl
    src/resources/pixel.hlsl
    src/resources/common.hlsli
    PROPERTIES HEADER_FILE_ONLY TRUE
)

add_custom_target(copy_resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resources
    ${CMAKE_CURRENT_BINARY_DIR}/resources)

add_dependencies(renderer2d copy_resources)

target_include_directories(renderer2d PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(renderer2d PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/glfw3.lib
    d3d11
    d3dcompiler
    dxgi
)
