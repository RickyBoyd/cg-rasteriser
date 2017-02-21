cmake_minimum_required(VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Ofast")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

find_package(SDL REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(SDL_INCLUDE_DIR)
include_directories("/usr/include/SDL")
include_directories(GLM_INCLUDE_DIR)

add_executable(main_raytracer Source/raytracer.cpp)

target_link_libraries(main_raytracer ${SDL_LIBRARY})