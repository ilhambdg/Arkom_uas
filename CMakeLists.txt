cmake_minimum_required(VERSION 3.12)
project(OpenCLdanOpenMP)

find_package(OpenMP REQUIRED)
find_package(OpenCL REQUIRED)


add_executable(opencl opencl.cpp)
if(OpenCL_FOUND)            
    target_link_libraries(opencl PUBLIC OpenCL::OpenCL)
endif()


add_executable(openmp openmp.cpp)
if(OpenMP_CXX_FOUND)
    target_link_libraries(openmp PUBLIC OpenMP::OpenMP_CXX)
endif()

## cmake -B build -DCMAKE_TOOLCHAIN_FILE="D:\vcpkg\scripts\buildsystems\vcpkg.cmake"

## cmake --build build