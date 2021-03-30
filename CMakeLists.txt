cmake_minimum_required(VERSION 3.18)

# GLFW settings
set(GLFW_BUILD_DOCS                 OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS                OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES             OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS               OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL    OFF CACHE BOOL "" FORCE)
# #

set(projectName         LearningVulkan)
set(targetName          ${projectName})
set(projectVersion      0.0.1)
set(sourceDirectory     Source)
set(externalDirectory   External)

project(${projectName} VERSION ${projectVersion})

add_subdirectory(${externalDirectory}/GLFW)
add_subdirectory(${externalDirectory}/GLM)
find_package(Vulkan REQUIRED FATAL_ERROR)

file(GLOB projectSrc
    Source/*.cxx
    Source/**/*.cxx
    Source/**/**/*.cxx
    Source/**/**/**/*.cxx
    Source/**/**/**/**/*.cxx
)

add_executable(${targetName} ${projectSrc})

target_include_directories(
    ${targetName} 
    PUBLIC 
    Source
    ${Vulkan_INCLUDE_DIR}
    ${externalDirectory}/GLM
    ${externalDirectory}/GLFW/include
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

target_precompile_headers(${targetName} PUBLIC ${sourceDirectory}/Pch.hpp)

target_link_libraries(
    ${targetName} 
    glfw
    ${Vulkan_LIBRARIES}
)

file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Debug)
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Release)
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR})