project(Ts3ScreenShare)

cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

find_package(FFmpeg REQUIRED)


include_directories(${LIBS_DIR}/ts3client-pluginsdk/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${FFMPEG_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED
        include/plugin.h src/plugin.cpp
        include/network/udp_client.h src/network/udp_client.cpp
        include/capture/screen_capture.h src/capture/screen_capture.cpp
        include/capture/avinput.h src/capture/avinput.cpp
        include/capture/avcodec.h src/capture/avcodec.cpp)

message(${FFMPEG_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES})