# AresByteCode

AresByteCode is a library for reading Jar files up to version 15 and is also based on
official JVM documentation.
  - Tools for parsing whole Jar archives
  - Used as a basis for other projects like AresObfuscator
  - Fully functional up to Java version 15

### Installation

Download the project or use git to clone it:
```sh
$ git clone https://github.com/Excse/AresByteCode.git
```

Navigate to the directory where the project is located and execute these commands:
```sh
$ cmake -S . -B build -DBUILD_TESTING=OFF
$ cmake --build build
$ cmake --install build
$ sudo make install
```

When you are finished, you are ready to use the library. If you want to use it in a
cmake project, use the following code to implement it.

and in CMakeLists.txt:
```cmake
find_package(aresbc CONFIG REQUIRED)

target_link_libraries(!!PROJECT_NAME!! PRIVATE ${LIBARESBC_LIBRARY})
```