[![](https://jitpack.io/v/denismakogon/judy.svg)](https://jitpack.io/#denismakogon/judy)

# Multithreaded C++ UDP server for Java

## Prerequisites

Make you have the following tools installed:
- [conan2](https://conan.io/)
- [jextract](https://jdk.java.net/jextract/)

### Conan profile

For macOS M1 you need the following profile (`~/.conan2/profiles/default`):
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

void printRequest(struct udp_request* request) {
    printf("%s [INFO] packet is %ld bytes long\n", currentDateTime().c_str(), request->bytesReceived);
    std::flush(std::cout);
}


int main(int argc, char const *argv[]) {
    return startNativeServer(printRequest, 20777, 2048, 1000);
}
```

see [main.cc](src/main.cc) for more details.


## How to use Boost::Asio server

```cpp
#include <iostream>
#include <thread>

#include "server.h"

void printRequest(struct udp_request* request) {
    printf("%s [INFO] packet is %ld bytes long\n", currentDateTime().c_str(), request->bytesReceived);
    std::flush(std::cout);
}

int main() {
    startBoostServer(&printRequest, 20777, 2048, std::thread::hardware_concurrency());
}
```

## How to use these servers from Java

`src/main/java/com/boost/server/BoostServer.java`
```java
package com.boost.server;

import java.lang.foreign.Arena;
import java.lang.invoke.MethodHandles;
import java.lang.invoke.MethodType;
import java.nio.charset.StandardCharsets;
import java.time.Instant;
import java.util.Objects;
import java.util.logging.Logger;

import static judy.server.c_api_h.boostServerWithHandler;

public class BoostServer implements AutoClosable {

    static {
        try {
            System.load(System.getProperty("judy.server.lib"));
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    static final Linker linker = Linker.nativeLinker();
    static final Arena sharedArena = Arena.ofAuto();
    static MemorySegment handlerSegment = null;

    public BoostServer() throws Exception {
        Objects.requireNonNull(configuration);
        this.configuration = configuration;
        var requestHandlerMH = MethodHandles.lookup().findVirtual(
                BoostServer.class, "requestHandler",
                MethodType.methodType(
                        void.class,
                        MemorySegment.class, long.class,
                        MemorySegment.class, int.class
                )
        ).bindTo(this);
        handlerSegment = linker.upcallStub(
                requestHandlerMH,
                FunctionDescriptor.ofVoid(
                        ValueLayout.ADDRESS, ValueLayout.JAVA_LONG,
                        ValueLayout.ADDRESS, ValueLayout.JAVA_INT
                ),
                sharedArena
        );
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            try {
                close();
            } catch (Exception _) {
            }
        }));
    }

    void requestHandler(MemorySegment dataPtr, long bytesTransferred,
                        MemorySegment clientHost, int clientPort) {
        logger.info(String.format("[%s] a new request", getClass().getName()));
        var data = dataPtr.reinterpret(2048, sharedArena, null)
                .toArray(ValueLayout.JAVA_BYTE);
        var client = new String(clientHost.reinterpret(9, sharedArena, null)
                .toArray(ValueLayout.JAVA_BYTE), StandardCharsets.UTF_8);
        System.out.printf("a new packet from %s:%d of %d bytes received!\n",
                client, clientPort, bytesTransferred);

    }

    public void build() {
        boostServerWithHandler(handlerSegment, 20777, 2048, 100);
    }

    @Override
    public void close() {
        sharedArena.close();
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
