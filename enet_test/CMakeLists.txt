cmake_minimum_required(VERSION 2.8)

# project name
set(project_name enet_test)
project(${project_name})

# bin dir
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

# Configure OS versions with some variables
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH})

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH}) 

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message("WARNING: ${PROJECT_NAME} requires g++ 4.7 or greater in order to use c++11. Instead c++0x will be used.")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX} -std=c++0x")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11.")
endif ()

# set some options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")


set(SRC ${PROJECT_SOURCE_DIR}/src)
include_directories(${SRC})

# modules
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# find files
file(GLOB_RECURSE source_files ${SRC}/*)

# Testing executable
set(EXEC ${project_name})
add_executable(
    ${project_name}
    ${source_files}
)

# Finding libs
find_package(ENet REQUIRED)
find_package(Threads REQUIRED)

#includes
include_directories(
    ${ENet_INCLUDE_DIRS}
)

# libs
target_link_libraries(
    ${EXEC}
    ${ENet_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_custom_target(run
    COMMAND ${EXECUTABLE_OUTPUT_PATH}/${EXEC}
    DEPENDS ${EXEC}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)


