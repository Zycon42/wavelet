#
# CMakeLists.txt
# author: Jan Dušek <xdusek17@stud.fit.vutbr.cz>

project(zpo13)
cmake_minimum_required(VERSION 2.6)

# we use c++11 so set proper gcc flags
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x")
endif()

# set bin directory for runtime files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(OpenCV REQUIRED)

add_subdirectory(src)
add_subdirectory(test)

# install required system runtime libraries
include(InstallRequiredSystemLibraries)