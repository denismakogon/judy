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
mkdir -p build
conan install . --output-folder=build --build=missing
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .

make java-sources
make jar
```

## How to use C UDP server

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


## How to use Boost::Asio server

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

## How to use these servers from Java

`src/main/java/com/boost/server/BoostServer.java`
```java
package com.boost.server;

import java.lang.foreign.*;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.nio.charset.StandardCharsets;
import java.time.Instant;


public class BoostServer implements AutoClosable {

    static final Linker linker = Linker.nativeLinker();
    MemorySession memorySession = MemorySession.openShared();
    MemorySegment handlerSegment;
    static {
        try {
            System.load(System.getProperty("boost.server.library"));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
    private static final SymbolLookup linkerLookup = linker.defaultLookup();
    private static final SymbolLookup loaderLookup = SymbolLookup.loaderLookup();
    private static final SymbolLookup lookup = name ->
            loaderLookup.lookup(name).or(() -> linkerLookup.lookup(name));

    public BoostServer() throws Exception {
        var requestHandlerMH = MethodHandles.lookup().findVirtual(
                BoostServer.class, "requestHandler",
                MethodType.methodType(
                        void.class,
                        MemoryAddress.class, long.class,
                        MemoryAddress.class, int.class
                )
        ).bindTo(this);
        handlerSegment = linker.upcallStub(
                requestHandlerMH,
                FunctionDescriptor.ofVoid(
                        ValueLayout.ADDRESS, ValueLayout.JAVA_LONG,
                        ValueLayout.ADDRESS, ValueLayout.JAVA_INT
                ),
                memorySession
        );
    }

    @Override
    public void build() throws Exception {
        var serverHandle = lookup.lookup("boostServerWithHandler").map(
                address -> linker.downcallHandle(address, FunctionDescriptor.ofVoid(
                        ValueLayout.ADDRESS, ValueLayout.JAVA_INT,
                        ValueLayout.JAVA_INT, ValueLayout.JAVA_INT
                ))
        ).orElseThrow();
        try {
            serverHandle.invoke(
                    handlerSegment.address(),
                    configuration.port,
                    configuration.bufferSize,
                    Integer.parseInt(System.getProperty("boost.threadpool.size", "1000"))
            );
        } catch (Throwable e) {
            throw new RuntimeException(e);
        }
    }

    private void requestHandler(MemoryAddress dataPtr, long ignoredBytesTransferred, MemoryAddress clientHost, int clientPort) {
        var data = MemorySegment.ofAddress(dataPtr, configuration.bufferSize, memorySession)
                .toArray(ValueLayout.JAVA_BYTE);
        var client = new String(MemorySegment.ofAddress(clientHost, 9, memorySession)
                .toArray(ValueLayout.JAVA_BYTE), StandardCharsets.UTF_8);
        System.out.println(String.format("a new packet from %s:%d of %d bytes received!",
                    client, clientPort, data.length));
    }

    @Override
    public void close() throws Exception {
        memorySession.close();
    }
}
```

`src/main/java/com/boost/server/Main.java`:
```java
public Main {
    public static void main(String[] args) {
        try(var server = new BoostServer()) {
            server.build();
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
```
