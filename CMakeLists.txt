# Red Image
# MIT License
# Copyright (c) 2020 Jacob Gelling

# Set minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Set project name, language and version
project(red-image LANGUAGES C VERSION 0.1)

# Create version header file
configure_file(${PROJECT_SOURCE_DIR}/include/version.h.in ${PROJECT_SOURCE_DIR}/include/version.h)

# Include header files
include_directories(${PROJECT_SOURCE_DIR}/include)

# Set output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Set executables to compile
add_executable(red-archive ${PROJECT_SOURCE_DIR}/src/cli.c ${PROJECT_SOURCE_DIR}/src/image.c)

# Display all warnings
set(CMAKE_C_FLAGS "-Wall -Wextra")

# Optimise release executable
set(CMAKE_C_FLAGS_RELEASE "-O3")
