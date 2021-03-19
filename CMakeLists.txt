project(Ts3ScreenShare)

set(CMAKE_CXX_STANDARD 20)
set(LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include_directories(${LIBS_DIR}/ts3client-pluginsdk/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(${PROJECT_NAME} main.cpp
        include/plugin.h src/plugin.cpp)