cmake_minimum_required(VERSION 3.22)
project(webgpu_minimal CXX)
set(CMAKE_CXX_STANDARD 20)

if (NOT EMSCRIPTEN)
    # Add the 'glfw' directory, which contains the definition of a 'glfw' target
    add_subdirectory(glfw)
else()
    # Create a mock 'glfw' target that just sets the `-sUSE_GLFW=3` link option:
    add_library(glfw INTERFACE)
    target_link_options(glfw INTERFACE -sUSE_GLFW=3)
endif()
# In both cases, we can now link to the 'glfw' target

add_subdirectory(WebGPU/WebGPU-distribution-wgpu-v0.19.4.1)
add_subdirectory(glfw3webgpu)

add_executable(webgpu_minimal main.cpp app.h app.cpp shader.h)

# Add the 'webgpu' target as a dependency of our App
target_link_libraries(webgpu_minimal PRIVATE glfw webgpu glfw3webgpu)
target_copy_webgpu_binaries(webgpu_minimal)

message("Copying shaders from ${PROJECT_SOURCE_DIR}/shaders to ${CMAKE_CURRENT_BINARY_DIR}/shaders")
file(COPY ${PROJECT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})