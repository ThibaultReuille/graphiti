cmake_minimum_required(VERSION 2.8)
project(OpenGraphiti)

set (OpenGraphiti_VERSION_MAJOR 1)
set (OpenGraphiti_VERSION_MINOR 0)

add_executable(graphiti Main.cc)

### ----- Compiler Configuration -----

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})

include_directories(${CMAKE_BINARY_DIR}/../)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PythonLibs)

#find_package(GLM REQUIRED)
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH} ../raindance/Lib/glm-0.9.5.4")

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${OPENCL_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})
 
target_link_libraries(graphiti ${OPENGL_LIBRARIES})
target_link_libraries(graphiti ${OPENCL_LIBRARIES})
target_link_libraries(graphiti ${GLFW_STATIC_LIBRARIES})
target_link_libraries(graphiti ${GLEW_LIBRARIES})
target_link_libraries(graphiti ${PYTHON_LIBRARIES})