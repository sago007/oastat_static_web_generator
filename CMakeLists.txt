cmake_minimum_required(VERSION 2.8.9)
project (oastat_static_web_generator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE} -O2")

file(GLOB SOURCES "src/*/*.cpp")

find_package(Boost COMPONENTS program_options REQUIRED)
add_executable(oastat_static_web_generator src/oastat_static_web_generator.cpp ${SOURCES})
TARGET_LINK_LIBRARIES( oastat_static_web_generator ${Boost_LIBRARIES} cppdb ctemplate )
