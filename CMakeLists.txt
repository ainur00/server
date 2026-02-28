cmake_minimum_required(VERSION 3.20.0)
project(server VERSION 0.1.0 LANGUAGES C CXX)

set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_ROOT "c:/boost-build")
set(Boost_LIBRARYDIR "C:/boost-build/lib")
set(Boost_INCLUDE_DIR "c:/boost-build/include/boost-1_90/")
find_package(Boost 1.90 CONFIG REQUIRED COMPONENTS thread)

add_executable(server 
    main.cpp
    src/server.cpp
    src/talk_to_client.cpp
)

if(Boost_FOUND)
    target_include_directories(server PUBLIC ${CMAKE_SOURCE_DIR})
    target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
    target_link_libraries(${PROJECT_NAME} ws2_32)
    message(STATUS "Found BOOST ${Boost_VERSION_STRING}")
else()
    message(STATUS "WEBSOCKET BOOST Not Found")
endif()
