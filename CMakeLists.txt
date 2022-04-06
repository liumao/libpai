# Set Minimum Cmake
cmake_minimum_required(VERSION 3.10.0)

# Set Project's Name
project(pai)

# enable @rpath in the install name for any shared library being built
# note: it is planned that a future version of CMake will enable this by default
set(CMAKE_MACOSX_RPATH 1)

# ignore sh.exe error
set(CMAKE_SH CMAKE_SH-NOTFOUND)

# defined
add_definitions(-D__STDC_CONSTANT_MACROS)

# include path
include_directories(./ /usr/include /usr/local/include)

# Link and Set Output Path
link_directories(${PROJECT_SOURCE_DIR})

# Generate execute
add_executable(${PROJECT_NAME} main.cpp led.cpp script.cpp video.cpp wheel.cpp)

# Link Libraries
target_link_libraries(${PROJECT_NAME} wiringPi pthread dlib png avcodec avdevice avformat swscale avutil opencv_core opencv_imgproc)
