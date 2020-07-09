cmake_minimum_required (VERSION 3.9)
project (PathTracer)

find_package(OpenMP REQUIRED)

include_directories("src/")
include_directories("src/backgrounds/")
include_directories("../external/argparse/")
include_directories("../external/CImg/")
include_directories("../external/doctest/")
include_directories("../external/json/")
include_directories("../external/sort_r/")
include_directories("../external/tinygltf/")
include_directories("src/materialmodels/")
include_directories("src/sensors/")

# set standard optimization
if(NOT DEFINED OPT)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(OPT "-O3")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(OPT "-O2")
    endif()
endif()

# set CFLAGS
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${OPT} -Wall -Wextra -Dcimg_display=0")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 ${OPT} -Wall -Dcimg_display=0")
endif()
	
# set CXXFLAGS
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${OPT} -Wall -Wextra -Dcimg_display=0")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${OPT} -Wall -Dcimg_display=0")
endif()

add_executable(pathtracer
src/assetgeometry.cpp src/assetmaterial.cpp src/assetmaterialior.cpp src/main.cpp src/mesh.cpp src/random.cpp src/referencelink.cpp src/renderer.cpp src/sensor.cpp src/utils.cpp
src/backgrounds/hdr.cpp
../external/argparse/argparse.c ../external/doctest.cpp ../external/tiny_gltf.cpp
src/materialmodels/common.cpp src/materialmodels/specular.cpp
src/sensors/camera.cpp)
target_link_libraries(pathtracer PUBLIC OpenMP::OpenMP_CXX)
