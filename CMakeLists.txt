### --- Project Initialization and Configuration --- ###
cmake_minimum_required(VERSION 3.20) # Require CMake 3.20 or later
project(aresbc LANGUAGES CXX)        # Define project name and language

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

set(PROJECT_DESCRIPTION "AresByteCode is a library to interact with java bytecode and provides also utils to load/write jar files.")

# Use C++20 and enforce it
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to enable/disable testing:
option(BUILD_TESTING "Enable testing" ON)
# Option to enable/disable installation:
option(MAKE_INSTALLABLE "Make the library installable" ON)
# Option to control static vs shared library
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

### --- CMake Modules for Installation and Fetching --- ###
include(CMakePackageConfigHelpers) # Helper for creating config files for package managers
include(GNUInstallDirs)            # Define standard install directories (lib, include, etc.)
include(FetchContent)              # For downloading and using external dependencies

### --- LibZip --- ###
set(LIBZIP_DO_INSTALL OFF CACHE BOOL "Disable install for libzip")
set(BUILD_REGRESS OFF CACHE BOOL "Disable regression tests")

# Download and make LibZip available
FetchContent_Declare(
        libzip
        GIT_REPOSITORY https://github.com/nih-at/libzip.git
        GIT_TAG main
)
FetchContent_MakeAvailable(libzip)

### --- Boost --- ###
# You need to have boost installed. E.g. via sudo apt-get install libboost-all-dev
find_package(Boost CONFIG REQUIRED)

### --- Define Library Target --- ##

# Create the main library from source file
add_library(${PROJECT_NAME}
        src/attribute_info.cpp
        src/class_file.cpp
        src/class_reader.cpp
        src/class_writer.cpp
        src/constant_info.cpp
        src/field_info.cpp
        src/method_info.cpp
        src/utils.cpp
        src/vm_check.cpp)
# Create an alias for the library: your_project::your_project
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
# Link both boost and libzip to the library
target_link_libraries(${PROJECT_NAME} zip Boost::boost)

# Setup include directories for consumers and installers
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> # For build tree
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)      # For install tree

# Define public headers
set(PUBLIC_HEADERS
        include/${PROJECT_NAME}/attribute_info.h
        include/${PROJECT_NAME}/class_file.h
        include/${PROJECT_NAME}/class_reader.h
        include/${PROJECT_NAME}/class_writer.h
        include/${PROJECT_NAME}/constant_info.h
        include/${PROJECT_NAME}/field_info.h
        include/${PROJECT_NAME}/method_info.h
        include/${PROJECT_NAME}/utils.h
        include/${PROJECT_NAME}/visitor.h
        include/${PROJECT_NAME}/vm_check.h)

# Set the properties of the resulting library
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        PUBLIC_HEADER "${PUBLIC_HEADERS}")


### --- Install Rules --- ###

if(MAKE_INSTALLABLE)
    # Install the library and its headers
    install(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

    # Install the target export file (for find_package)
    install(EXPORT ${PROJECT_NAME}Targets
            FILE ${PROJECT_NAME}Targets.cmake
            NAMESPACE ${PROJECT_NAME}::
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    # Generate version file for package configuration
    write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY AnyNewerVersion)

    # Creates a config file for package configuration
    configure_package_config_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    # Install the generated config and config version file
    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif ()

### --- Unit Testing Setup --- ###

if(BUILD_TESTING)
    # Enable testing framework
    enable_testing()
    # Adds the test directory to the build
    add_subdirectory(tests)
endif()