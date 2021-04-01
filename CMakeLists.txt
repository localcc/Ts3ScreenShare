project(Ts3ScreenShare C CXX)

cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

find_package(FFmpeg REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(${LIBS_DIR}/ts3client-pluginsdk/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${FFMPEG_INCLUDE_DIRS})
include_directories(${SDL_INCLUDE_DIRS})
include_directories(${LIBS_DIR}/enet/include)

add_subdirectory(${LIBS_DIR})

file(GLOB packets_src include/network/packets/*.h)


add_library(${PROJECT_NAME} SHARED
        include/plugin.h src/plugin.cpp
        include/network/udp_client.h src/network/udp_client.cpp
        include/capture/screen_capture.h src/capture/screen_capture.cpp
        include/capture/avinput.h src/capture/avinput.cpp
        include/capture/avcodec.h src/capture/avcodec.cpp
        include/network/NetworkHelper.h
        include/window/video_window.h src/window/video_window.cpp
        include/window/sdl_window.h src/window/sdl_window.cpp
        include/window/sdl_renderer.h src/window/sdl_renderer.cpp
        include/window/sdl_texture.h src/window/sdl_texture.cpp
        ${packets_src}
        include/capture/swscontext.h src/capture/swscontext.cpp )
    message(${SWSCALE_VERSION})
target_link_libraries(${PROJECT_NAME} ${FFMPEG_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES} ${AVDEVICE_LIBRARIES}  ${SDL_LIBRARIES})
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32 winmm)
endif()
