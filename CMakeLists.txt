cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(actionheadergen LANGUAGES CXX)

set(PROJECT_SOURCES
  src/main.cpp
  src/headergenerator.cpp
  src/interleafhandler.cpp
)

add_executable(actionheadergen ${PROJECT_SOURCES})

target_link_libraries(actionheadergen PRIVATE libweaver)

target_link_directories(actionheadergen PRIVATE "${CMAKE_SOURCE_DIR}/libweaver/build/lib")

target_include_directories(actionheadergen PRIVATE 
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/libweaver/lib"
)

set_target_properties(actionheadergen PROPERTIES
  CXX_STANDARD 98
  CXX_STANDARD_REQUIRED ON
)
