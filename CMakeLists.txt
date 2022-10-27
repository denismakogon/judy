cmake_minimum_required(VERSION 3.24)

project(judy LANGUAGES C CXX VERSION 0.0.1 DESCRIPTION "Multithreaded CPP UDP server consumable through the C ABI consumption")

execute_process(COMMAND date +'%Y.%m.%d' OUTPUT_VARIABLE JAR_VERSION)

add_definitions("-std=c++14")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
message(STATUS "Found boost includes at " ${Boost_INCLUDE_DIR})

AUX_SOURCE_DIRECTORY(src SOURCES)

add_executable(${PROJECT_NAME}-main ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}-main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}-main ${Boost_LIBRARIES} )

add_custom_target(
    java-sources 
    ALL
    COMMAND jextract -I /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include @${CMAKE_CURRENT_SOURCE_DIR}/jextract_dump.txt ${CMAKE_CURRENT_SOURCE_DIR}/include/c_api.h
)

add_custom_target(
    mnv-packet
    ALL
    COMMAND mvn -f ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml clean package -Dversion=${JAR_VERSION} -Dsrc.dir=build/src/main/java
)
