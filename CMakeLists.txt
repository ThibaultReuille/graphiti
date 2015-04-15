### ----- Project Configuration -----

cmake_minimum_required(VERSION 2.8)
project(OpenGraphiti)

set (OpenGraphiti_VERSION_MAJOR 1)
set (OpenGraphiti_VERSION_MINOR 0)

### ----- Packing Resources

set_source_files_properties(Pack.hh PROPERTIES GENERATED true)
add_custom_command(OUTPUT Pack.hh
    COMMAND . pack.sh 
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )

add_executable(graphiti Main.cc Pack.hh)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-missing-field-initializers -O3 -Wno-deprecated")

### -----  Includes / Libraries / Packages -----

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include_directories(${PROJECT_SOURCE_DIR}/../)

list(APPEND CMAKE_PREFIX_PATH ${PROJECT_SOURCE_DIR}/../raindance/Lib/glm-0.9.5.4)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PythonLibs)

include_directories(${OPENGL_INCLUDE_DIRS})
include_directories(${OPENCL_INCLUDE_DIRS})
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(${GLEW_INCLUDE_DIRS})
include_directories(${GLM_INCLUDE_DIRS})
include_directories(${PYTHON_INCLUDE_DIRS})

if (DEFINED OG_OCULUS_RIFT)
	add_definitions(-DOG_OCULUS_RIFT=true)
	include_directories(${PROJECT_SOURCE_DIR}/../OculusSDK/LibOVR/Include)
	include_directories(${PROJECT_SOURCE_DIR}/../OculusSDK/LibOVRKernel/Src/)

	add_library(libovr STATIC IMPORTED)
	set_target_properties(libovr PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../OculusSDK/output/libovr.a)
 endif()

### ----- Linking -----

target_link_libraries(graphiti ${OPENGL_LIBRARIES})
target_link_libraries(graphiti ${OPENCL_LIBRARIES})
target_link_libraries(graphiti ${GLFW_STATIC_LIBRARIES})
target_link_libraries(graphiti ${GLEW_LIBRARIES})
target_link_libraries(graphiti ${PYTHON_LIBRARIES})

if(DEFINED OG_OCULUS_RIFT)
	target_link_libraries(graphiti libovr)
endif()