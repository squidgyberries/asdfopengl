cmake_minimum_required(VERSION 3.20)
project(asdfopengl LANGUAGES C)

cmake_policy(SET CMP0135 NEW)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(glfw URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip)
FetchContent_MakeAvailable(glfw)

set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)

FetchContent_Declare(cglm URL https://github.com/recp/cglm/archive/refs/tags/v0.8.7.tar.gz)
FetchContent_MakeAvailable(cglm)

add_executable(asdfopengl src/main.c src/shader.c src/types.c src/gl.c src/stb_image/stb_image.c src/nuklear/nuklear.c src/log.c)
target_link_libraries(asdfopengl glfw cglm)