cmake_minimum_required(VERSION 3.24)

project(ShaderCourseStarter
    VERSION 0.1.0
    DESCRIPTION "Minimal OpenGL/GLSL starter for 3D and Shader Programming"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Keep third-party projects small and quiet.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)

# Pinned versions make the course setup reproducible.
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

add_library(glad STATIC
    external/glad/src/gl.c
)

target_include_directories(glad
    PUBLIC
        external/glad/include
)

add_executable(shader_course
    src/main.cpp
)

target_link_libraries(shader_course
    PRIVATE
        glfw
        glad
        glm::glm-header-only
)

if(MSVC)
    target_compile_options(shader_course PRIVATE /W4 /permissive-)
else()
    target_compile_options(shader_course PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Put the executable in a predictable location for single- and multi-config generators.
set_target_properties(shader_course PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)
    set_target_properties(shader_course PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/bin"
    )
endforeach()

# Keep runtime shader files synchronized whenever shader_course is built.
add_custom_target(copy_shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_SOURCE_DIR}/shaders"
            "${CMAKE_BINARY_DIR}/bin/shaders"
    COMMENT "Copying shaders next to executable"
    VERBATIM
)

add_dependencies(shader_course copy_shaders)

# When launching from a generated Visual Studio project, run from the
# executable directory so relative paths such as shaders/basic.vert work.
set_target_properties(shader_course PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Helpful when opening the generated solution in Visual Studio.
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT shader_course)
