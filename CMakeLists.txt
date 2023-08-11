cmake_minimum_required(VERSION 3.0)
project(skt_learn_web)

include(cmake/utils.cmake)
include(cmake/ragel.cmake)

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
#message("****", ${YAMLCPP})

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
     skt/http/http_parser.cpp
     skt/tcp_server/tcp_server.cpp
     skt/stream/stream.cpp
     skt/streams/socket_stream.cpp
     skt/http/http_session.cpp
     skt/http/http_server.cpp
     skt/http/servlet.cpp
     skt/http/http_connection.cpp
     skt/daemon/daemon.cpp
     skt/env/env.cpp
     skt/application/application.cpp
)

ragelmaker(skt/http/http11_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/skt/http)
ragelmaker(skt/http/httpclient_parser.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/skt/http)
ragelmaker(skt/uri/uri.rl LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/skt/uri)

#message(STATUS ${LIB_SRC})

add_library(skt SHARED ${LIB_SRC})
redefine_file_macro(skt) #__FILE__

set(LIBS
        skt
        dl
        pthread
        yaml-cpp)

#message("****", ${LIBS})

skt_add_executable(test tests/test.cpp skt "${LIBS}")
skt_add_executable(test_config tests/test_config.cpp skt "${LIBS}")
skt_add_executable(test_thread tests/test_thread.cpp skt "${LIBS}")
skt_add_executable(test_utils tests/test_utils.cpp skt "${LIBS}")
skt_add_executable(test_fiber tests/test_fiber.cpp skt "${LIBS}")
skt_add_executable(test_scheduler tests/test_scheduler.cpp skt "${LIBS}")
skt_add_executable(test_iomanager tests/test_iomanager.cpp skt "${LIBS}")
skt_add_executable(test_hook tests/test_hook.cpp skt "${LIBS}")
skt_add_executable(test_address tests/test_address.cpp skt "${LIBS}")
skt_add_executable(test_socket tests/test_socket.cpp skt "${LIBS}")
skt_add_executable(test_bytearray tests/test_bytearray.cpp skt "${LIBS}")
skt_add_executable(test_http tests/test_http.cpp skt "${LIBS}")
skt_add_executable(test_http_parser tests/test_http_parser.cpp skt "${LIBS}")
skt_add_executable(test_tcp_server tests/test_tcp_server.cpp skt "${LIBS}")
skt_add_executable(echo_server example/echo_server.cpp skt "${LIBS}")
skt_add_executable(test_http_server tests/test_http_server.cpp skt "${LIBS}")
skt_add_executable(test_http_connection tests/test_http_connection.cpp skt "${LIBS}")
skt_add_executable(test_uri tests/test_uri.cpp skt "${LIBS}")
skt_add_executable(test_daemon tests/test_daemon.cpp skt "${LIBS}")
skt_add_executable(test_env tests/test_env.cpp skt "${LIBS}")
skt_add_executable(test_application tests/test_application.cpp skt "${LIBS}")

SET(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/lib)


