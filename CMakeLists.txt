cmake_minimum_required(VERSION 3.18)

set(projectName     AstrumVulkan)
set(projectVersion  0.1)

project(${projectName} VERSION ${projectVersion})

file(GLOB src
	${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/Source/**/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/Source/**/**/*.cxx
	${CMAKE_CURRENT_SOURCE_DIR}/Source/**/**/**/*.cxx
)

add_executable(${projectName} ${src})

target_precompile_headers(${projectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source/Pch.hpp)

target_include_directories(${projectName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Source)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

# 3rd party settings
set(GLFW_BUILD_DOCS                 OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS                OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES             OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS               OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL    OFF CACHE BOOL "" FORCE)
# #

add_subdirectory(External/GLFW)
add_subdirectory(External/GLM)

find_package(Vulkan REQUIRED FATAL_ERROR)

target_include_directories(${projectName} PUBLIC External/GLFW/include ${Vulkan_INCLUDE_DIRS})
target_include_directories(${projectName} PUBLIC External/GLM)

target_link_libraries(${projectName} glfw ${Vulkan_LIBRARY})

if(WIN32)
	add_custom_command(
		TARGET ${projectName}
		POST_BUILD
		COMMAND cmd ${CMAKE_CURRENT_SOURCE_DIR}Compile.bat
	)
endif(WIN32)

file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Debug)