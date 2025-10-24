cmake_minimum_required(VERSION 3.10)
project(Computergrafik)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)
find_package(HARFBUZZ REQUIRED)
# find_package(SDL2_ttf REQUIRED)
#find_package(GLM REQUIRED)

include_directories(src/include ${OPENGL_INCLUDE_DIR} usr/include src/include/SDL3_ttf src/include/freetype usr/include/harfbuzz )

file(GLOB SOURCES_GL "src/cpp_opengl/*.cpp")
# Executable target
add_executable(cg_opengl ${SOURCES_GL})

target_link_libraries(cg_opengl 
PRIVATE 
    SDL3::SDL3 
    OpenGL::GL 
    GLEW::GLEW 
    ${PROJECT_SOURCE_DIR}/lib/libSDL3_ttf.so 
    ${PROJECT_SOURCE_DIR}/lib/libfreetype.a 
    PNG::PNG
    /usr/lib64/libharfbuzz.so
    /usr/lib64/libz.so 
    bz2) 