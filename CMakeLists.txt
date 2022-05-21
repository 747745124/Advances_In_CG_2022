cmake_minimum_required(VERSION 3.19)
project(CGFinal)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

include_directories( ./include ./basic ./gui ./landscape ./model)
link_directories ( ./lib)

IF (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	link_libraries(libglfw.3.dylib libassimp.dylib)
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	link_libraries(glfw3.lib assimp-vc142-mt.lib)
ENDIF ()

add_definitions(-DDEFERRED_SHADING)

file(GLOB_RECURSE imgui_cpp ${PROJECT_SOURCE_DIR}/include/imgui/*.cpp)
file(GLOB basic_cpp ${PROJECT_SOURCE_DIR}/basic/*.cpp)
file(GLOB landscape_cpp ${PROJECT_SOURCE_DIR}/landscape/*.cpp)
add_executable(CGFinal main.cpp glad.c ${imgui_cpp} ${basic_cpp} ${landscape_cpp})

IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	set_target_properties(CGFinal PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$(ProjectDir)..\\lib;")
ENDIF()

