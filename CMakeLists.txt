cmake_minimum_required(VERSION 3.0)
project(skt_learn_web)

set(CMAKE_VERBOSE_MAKEFILE ON) #-rdynamic -O0
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")



set(LIB_SRC
     skt/log/log.cpp
     skt/util/util.cpp
)

add_library(skt_learn_web SHARED ${LIB_SRC})


add_executable(tests tests/test.cpp)
add_dependencies(tests skt_learn_web)
target_link_libraries(tests skt_learn_web)

SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)


