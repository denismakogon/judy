cmake_minimum_required(VERSION 3.24)

project(judy LANGUAGES C CXX VERSION 0.0.1 DESCRIPTION "Multithreaded C++ UDP server consumable through the C ABI consumption")

execute_process(COMMAND date +'%Y.%m.%d' OUTPUT_VARIABLE JAR_VERSION)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(STDLIB_INCLUDE /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include)
else()
    set(STDLIB_INCLUDE /usr/local/include)
endif()

# if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch")
#     if(CMAKE_SIZEOF_VOID_P EQUAL 8)
#         if(CMAKE_SYSTEM_NAME MATCHES "Linux")
#             include(${CMAKE_CURRENT_LIST_DIR}/../toolchains/linux-aarch64.cmake)
#         endif()
#     endif()
# endif()

add_definitions("-std=c++14")
add_compile_options(-pthread)


find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
message(STATUS "Found boost includes at " ${Boost_INCLUDE_DIR})

AUX_SOURCE_DIRECTORY(src SOURCES)

add_executable(${PROJECT_NAME}-main ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}$ENV{LIB_SUFFIX} SHARED ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}-main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME}$ENV{LIB_SUFFIX} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}-main ${Boost_LIBRARIES} )
target_link_libraries(${PROJECT_NAME}$ENV{LIB_SUFFIX} PRIVATE Threads::Threads)

add_custom_target(
    java-sources 
    COMMAND jextract -I ${STDLIB_INCLUDE} @${CMAKE_CURRENT_SOURCE_DIR}/jextract_dump.txt ${CMAKE_CURRENT_SOURCE_DIR}/include/api.h
)

add_custom_target(
    jar
    COMMAND mvn -f ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml clean package -Dversion=${JAR_VERSION} -Dsrc.dir=${CMAKE_CURRENT_SOURCE_DIR}/build/src
)
