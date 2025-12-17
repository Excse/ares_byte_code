# AresByteCode

![State](https://img.shields.io/github/actions/workflow/status/Excse/ares_byte_code/cmake-single-platform.yml?style=flat&label=Build%2FTest)
![Last Commit](https://img.shields.io/github/last-commit/Excse/ares_byte_code?style=flat&label=Last%20Commit)
![Contributors](https://img.shields.io/github/contributors/Excse/ares_byte_code?style=flat&label=Contributors)
![Visitors](https://api.visitorbadge.io/api/visitors?path=https%3A%2F%2Fgithub.com%2FExcse%2Fares_byte_code&label=Repository%20Visits&countColor=%232ccce4&style=flat&labelStyle=none)
![License](https://img.shields.io/github/license/Excse/ares_byte_code?style=flat&label=License)
![Stars](https://img.shields.io/github/stars/Excse/ares_byte_code)

AresByteCode is a library for reading Jar files up to version 15 and is also based on
official JVM documentation.
  - Tools for parsing whole Jar archives
  - Used as a basis for other projects like AresObfuscator
  - Fully functional up to Java version 15

### Installation

Download the project or use git to clone it:
```sh
$ git clone https://github.com/Excse/ares_byte_code
```

Navigate to the directory where the project is located and execute these commands:
```sh
$ cmake -S . -B build -DBUILD_TESTING=OFF -DMAKE_INSTALLABLE=ON -DBUILD_SHARED_LIBS=OFF
$ cmake --build build
$ cmake --install build
$ sudo make install
```

When you are finished, you are ready to use the library. If you want to use it in a
cmake project, use the following code to implement it.

and in CMakeLists.txt:
```cmake
find_package(aresbc CONFIG REQUIRED)

target_link_libraries(<PROJECT_NAME> PRIVATE aresbc::aresbc)
```
