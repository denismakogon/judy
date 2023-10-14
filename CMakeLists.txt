cmake_minimum_required(VERSION 3.24)

project(judy LANGUAGES C CXX VERSION 0.0.1 DESCRIPTION "Multithreaded C++ UDP server consumable through the C ABI consumption")

execute_process(COMMAND date +'%Y.%m.%d' OUTPUT_VARIABLE JAR_VERSION)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(STDLIB_INCLUDE /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include)
else()
    set(STDLIB_INCLUDE /usr/local/include)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX TRUE)
elseif(WIN32)
elseif(APPLE)
else()
    message(FATAL_ERROR "Platform is not supported")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm" OR CMAKE_SYSTEM_PROCESSOR MATCHES "aarch")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "aarch64")
    else()
        set(ARCH "arm")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^mips.*")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "mips64el")
    else()
        set(ARCH "mipsel")
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^ppc.*")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "ppc64le")
    else()
        message(FATAL_ERROR "Architecture is not supported")
    endif()
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(ARCH "x86_64")
    else()
        set(ARCH "x86")
    endif()
endif()

if(CMAKE_OSX_ARCHITECTURES)
    if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
        set(ARCH "aarch64")
    elseif(CMAKE_OSX_ARCHITECTURES MATCHES "x86_64")
        set(ARCH "x86_64")
    elseif(CMAKE_OSX_ARCHITECTURES MATCHES "i386")
        set(ARCH "x86")
    else()
        message(FATAL_ERROR "Architecture ${CMAKE_OSX_ARCHITECTURES} is not "
                            "supported. Only one architecture (arm64, x86_64 "
                            "or i386) could be specified at build time.")
    endif()
endif()
set(CMAKE_MACOSX_RPATH TRUE)


macro(set_cpp_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_CXX_FLAGS_${ARGV1} "${CMAKE_CXX_FLAGS_${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
    endif()
endmacro()
macro(set_linker_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_EXE_LINKER_FLAGS_${ARGV1} "${CMAKE_EXE_LINKER_FLAGS_${ARGV1}} ${FLAG}")
        set(CMAKE_SHARED_LINKER_FLAGS_${ARGV1} "${CMAKE_EXE_LINKER_FLAGS_${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAG}")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${FLAG}")
    endif()
endmacro()

if(ARCH STREQUAL "x86")
    set_cpp_flag("-m32")
    set_cpp_flag("-msse2")
    set_cpp_flag("-mfpmath=sse")
    set_cpp_flag("-march=pentium4")
    set_cpp_flag("-mtune=generic")
endif()
if(ARCH STREQUAL "x86_64")
    set_cpp_flag("-m64")
    set_cpp_flag("-fPIC")
    set_cpp_flag("-march=x86-64")
    set_cpp_flag("-mtune=generic")
endif()



add_definitions("-std=c++14")
add_compile_options(-pthread)


find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIR})
message(STATUS "Found boost includes at " ${Boost_INCLUDE_DIR})

AUX_SOURCE_DIRECTORY(src SOURCES)

add_executable(${PROJECT_NAME}-main ${SOURCES} ${HEADERS})
add_library(${PROJECT_NAME}-${ARCH} SHARED ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME}-main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME}-${ARCH} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME}-main ${Boost_LIBRARIES} )
target_link_libraries(${PROJECT_NAME}-${ARCH} PRIVATE Threads::Threads)

add_custom_target(
    java-sources 
    COMMAND jextract -I ${STDLIB_INCLUDE} @${CMAKE_CURRENT_SOURCE_DIR}/jextract_dump.txt ${CMAKE_CURRENT_SOURCE_DIR}/include/api.h
)

add_custom_target(
    jar
    COMMAND mvn -f ${CMAKE_CURRENT_SOURCE_DIR}/pom.xml clean package -Dversion=${JAR_VERSION} -Dsrc.dir=${CMAKE_CURRENT_SOURCE_DIR}/build/src
)
