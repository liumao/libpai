# Set Minimum Cmake
cmake_minimum_required(VERSION 3.10.0)

# Set Project's Name
project(pai)

# enable @rpath in the install name for any shared library being built
# note: it is planned that a future version of CMake will enable this by default
set(CMAKE_MACOSX_RPATH 1)

# ignore sh.exe error
set(CMAKE_SH CMAKE_SH-NOTFOUND)

# Link and Set Output Path
link_directories(${PROJECT_SOURCE_DIR})

# Generate execute
add_executable(${PROJECT_NAME} main.cpp)

# Link Libraries
target_link_libraries(${PROJECT_NAME} wiringPi)