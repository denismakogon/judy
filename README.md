[![](https://jitpack.io/v/denismakogon/judy.svg)](https://jitpack.io/#denismakogon/judy)

# Multithreaded CPP UDP server consumable through the C ABI consumption

## Prerequisites

Make you have the following tools installed:
- [conan](https://conan.io/)
- [jextract](https://jdk.java.net/jextract/)

### Conan profile

For macOS M1 you need the following profile (`~/.conan/profiles/default`):
```text
[settings]
os=Macos
os_build=Macos
arch=armv8
arch_build=armv8
compiler=clang
compiler.version=14
compiler.libcxx=libc++
build_type=Release
compiler.cppstd=14

[options]

[build_requires]

[env]
CC=/usr/bin/clang
CXX=/usr/bin/clang++
```

If you're on something else like `linux` or `x86_64` arch, please following [these instructions](https://docs.conan.io/en/latest/reference/profiles.html).


## How to build

```shell
mkdir -p build && cd build
conan install --build=missing ..
cmake .. -DCMAKE_BUILD_TYPE=release
cmake --build .
make java-sources
make jar
```

## How to use it native server

```cpp
#include "server.h"
#include "structs.h"

void printRequest(char*, long bytesTransferred, char* host, int port) {
    printf("a new packet from %s:%d of %ld bytes received!\n", host, port, bytesTransferred);
    std::flush(std::cout);
}

int main(int argc, char const *argv[]) {
    return startNativeServerWithHandler(printRequest, 20777, 2048, 1000);
}
```

see [main.cc](src/main.cc) for more details.


## How to use it Boost::Asio server

```cpp
#include <iostream>
#include <thread>

#include "server.h"

void printRequest(char*, long bytesTransferred, char* host, int port) {
    printf("a new packet from %s:%d of %ld bytes received!\n", host, port, bytesTransferred);
    std::flush(std::cout);
}

int main() {
    startBoostServerWithHandler(&printRequest, 20777, 2048, std::thread::hardware_concurrency());
}
```
