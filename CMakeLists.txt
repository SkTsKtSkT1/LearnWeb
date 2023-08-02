cmake_minimum_required(VERSION 3.0)
project(skt_learn_web)

include(cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON) #-rdynamic -O0
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS}  -rdynamic -O0 -ggdb -std=c++11  -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

#include googletest
#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)
#
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)

find_library(YAMLCPP yaml-cpp)
find_library(PTHREAD pthread)
message("****", ${YAMLCPP})

set(LIB_SRC
     skt/log/log.cpp
     skt/util/util.cpp
     skt/config/config.cpp
     skt/thread/thread.cpp
     skt/fiber/fiber.cpp
     skt/scheduler/scheduler.cpp
     skt/mutex/mutex.cpp
     skt/iomanager/iomanager.cpp
     skt/timer/timer.cpp
     skt/hook/hook.cpp
     skt/fd_manager/fd_manager.cpp
     skt/address/address.cpp
     skt/socket/socket.cpp
     skt/bytearray/bytearray.cpp
     skt/http/http.cpp
)

add_library(skt SHARED ${LIB_SRC})
redefine_file_macro(skt) #__FILE__

set(LIBS
        skt
        dl
        pthread
        yaml-cpp)

message("****", ${LIBS})

add_executable(test tests/test.cpp)
add_dependencies(test skt)
redefine_file_macro(test) #__FILE__
target_link_libraries(test ${LIBS})

add_executable(test_config tests/test_config.cpp)
add_dependencies(test_config skt)
redefine_file_macro(test_config) #__FILE__
target_link_libraries(test_config ${LIBS})

add_executable(test_thread tests/test_thread.cpp)
add_dependencies(test_thread skt)
redefine_file_macro(test_thread) #__FILE__
target_link_libraries(test_thread ${LIBS})

add_executable(test_utils tests/test_utils.cpp)
add_dependencies(test_utils skt)
redefine_file_macro(test_utils) #__FILE__
target_link_libraries(test_utils ${LIBS})

add_executable(test_fiber tests/test_fiber.cpp)
add_dependencies(test_fiber skt)
redefine_file_macro(test_fiber) #__FILE__
target_link_libraries(test_fiber ${LIBS})

add_executable(test_scheduler tests/test_scheduler.cpp)
add_dependencies(test_scheduler skt)
redefine_file_macro(test_scheduler) #__FILE__
target_link_libraries(test_scheduler ${LIBS})

add_executable(test_iomanager tests/test_iomanager.cpp)
add_dependencies(test_iomanager skt)
redefine_file_macro(test_iomanager) #__FILE__
target_link_libraries(test_iomanager ${LIBS})

add_executable(test_hook tests/test_hook.cpp)
add_dependencies(test_hook skt)
redefine_file_macro(test_hook) #__FILE__
target_link_libraries(test_hook ${LIBS})

add_executable(test_address tests/test_address.cpp)
add_dependencies(test_address skt)
redefine_file_macro(test_address) #__FILE__
target_link_libraries(test_address ${LIBS})

add_executable(test_socket tests/test_socket.cpp)
add_dependencies(test_socket skt)
redefine_file_macro(test_socket) #__FILE__
target_link_libraries(test_socket ${LIBS})

add_executable(test_bytearray tests/test_bytearray.cpp)
add_dependencies(test_bytearray skt)
redefine_file_macro(test_bytearray) #__FILE__
target_link_libraries(test_bytearray ${LIBS})

add_executable(test_http tests/test_http.cpp)
add_dependencies(test_http skt)
redefine_file_macro(test_http) #__FILE__
target_link_libraries(test_http ${LIBS})

SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)


