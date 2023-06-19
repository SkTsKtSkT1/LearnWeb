cmake_minimum_required(VERSION 3.0)
project(skt_learn_web)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON) #-rdynamic -O0
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)
message("****", ${YAMLCPP})

set(LIB_SRC
     skt/log/log.cpp
     skt/util/util.cpp
     skt/config/config.cpp
)

add_library(skt SHARED ${LIB_SRC})
redefine_file_macro(skt) #__FILE__

add_executable(test tests/test.cpp)
add_dependencies(test skt)
target_link_libraries(test skt ${YAMLCPP})
redefine_file_macro(test) #__FILE__

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config skt)
target_link_libraries(test_config skt ${YAMLCPP})
redefine_file_macro(test_config) #__FILE__

SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)


