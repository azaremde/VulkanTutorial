cmake_minimum_required(VERSION 3.18)

include(CMake/ExternalSettings.cmake)

set(projectName         LearningVulkan)
set(targetName          ${projectName})
set(projectVersion      0.0.1)
set(sourceDirectory     Source)
set(externalDirectory   External)
set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} /std:c++17")

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

target_precompile_headers(${targetName} PUBLIC ${sourceDirectory}/Pch.hpp)

target_include_directories(
    ${targetName} 
    PUBLIC 
    Source
    ${Vulkan_INCLUDE_DIR}
    ${externalDirectory}/GLM
    ${externalDirectory}/GLFW/include
)

target_link_libraries(
    ${targetName} 
    glfw
    ${Vulkan_LIBRARIES}
)

file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Debug)
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR}/Release)
file(COPY Shaders DESTINATION ${CMAKE_BINARY_DIR})