cmake_minimum_required(VERSION 3.10)
project(OBJParser)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set a default build type if none is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

#These packages are installed by running vcpkg install inside of the vcpkg directory

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)
find_package(HARFBUZZ REQUIRED)
find_package(Freetype REQUIRED)
# find_package(SDL2_ttf REQUIRED)
#find_package(GLM REQUIRED)

include_directories(src/include)

file(GLOB SOURCES_GL "src/cpp/*.cpp")
# Executable target
add_executable(obj_parser ${SOURCES_GL})

target_link_libraries(obj_parser 
PRIVATE 
    SDL3::SDL3 
    OpenGL::GL 
    GLEW::GLEW 
    Freetype::Freetype
    PNG::PNG
    harfbuzz::harfbuzz
    bz2
    ) 

set(CMAKE_INSTALL_PREFIX "/home/julian/Documents/Programs/finished/")
# Install your asset directories
install(DIRECTORY src/shaders/ DESTINATION OBJ_Parser/src/shaders)
install(DIRECTORY src/models/ DESTINATION OBJ_Parser/src/models)
install(DIRECTORY src/images/ DESTINATION OBJ_Parser/src/images)
install(DIRECTORY src/fonts/ DESTINATION OBJ_Parser/src/fonts)  

install(TARGETS obj_parser RUNTIME DESTINATION OBJ_Parser/build)