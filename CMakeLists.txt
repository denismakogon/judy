cmake_minimum_required(VERSION 3.24)

project(judy LANGUAGES C CXX VERSION 0.0.1 DESCRIPTION "Multithreaded CPP UDP server consumable through the C ABI consumption")

add_definitions("-std=c++14")

AUX_SOURCE_DIRECTORY(src SOURCES)

add_executable(${PROJECT_NAME}-main ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}-main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_custom_target(
    java-sources 
    ALL
    COMMAND jextract -I /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include @${CMAKE_CURRENT_SOURCE_DIR}/jextract_dump.txt ${CMAKE_CURRENT_SOURCE_DIR}/include/c_api.h
)
