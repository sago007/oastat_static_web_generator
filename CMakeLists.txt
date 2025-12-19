cmake_minimum_required(VERSION 3.10..4.1.2)
project (oastat_static_web_generator)

# Use Boost's native CMake config instead of FindBoost module
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 NEW)
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*/*.cpp")

find_package(Boost COMPONENTS program_options REQUIRED)
add_executable(oastat_static_web_generator src/oastat_static_web_generator.cpp ${SOURCES})
TARGET_LINK_LIBRARIES( oastat_static_web_generator ${Boost_LIBRARIES} cppdb ctemplate )
