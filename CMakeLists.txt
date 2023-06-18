cmake_minimum_required(VERSION 3.0)
project(skt_learn_web)

set(CMAKE_VERBOSE_MAKEFILE ON) #-rdynamic -O0
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -rdynamic -o0 -g -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

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

add_library(skt_learn_web SHARED ${LIB_SRC})


add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config skt_learn_web)
target_link_libraries(test_config skt_learn_web ${YAMLCPP})

SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)


