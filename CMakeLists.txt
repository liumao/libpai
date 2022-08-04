# Set Minimum Cmake
cmake_minimum_required(VERSION 3.10.0)

# Set Project's Name
project(pai)

# enable @rpath in the install name for any shared library being built
# note: it is planned that a future version of CMake will enable this by default
set(CMAKE_MACOSX_RPATH 1)

# ignore sh.exe error
set(CMAKE_SH CMAKE_SH-NOTFOUND)

# include path
include_directories(./ 
					/usr/include 
					/usr/local/include
					/usr/local/include/pocketsphinx
					/usr/local/include/sphinxbase)

# Link and Set Output Path
link_directories(${PROJECT_SOURCE_DIR})

# Source Files
set(src_files main.cpp script.cpp)

# Lib Files
set(lib_files wiringPi pthread)

# check led
option(LED_TEST "This is a default option for LED_TEST" OFF)
if (${LED_TEST} STREQUAL "ON")
	# message
	message(STATUS "led enable")
	
	# add defined
	add_definitions(-DLED_TEST)
	
	# add src file
	set(src_files ${src_files} led.cpp)
endif()

# check wheel
option(WHEEL_TEST "This is a default option for WHEEL_TEST" OFF)
if (${WHEEL_TEST} STREQUAL "ON")
	# message
	message(STATUS "wheel enable")
	
	# add defined
	add_definitions(-DWHEEL_TEST)
	
	# add src file
	set(src_files ${src_files} wheel.cpp)
endif()

# check face reco
option(FACE_RECO_TEST "This is a default option for FACE_RECO_TEST" OFF)
if (${FACE_RECO_TEST} STREQUAL "ON")
	# message
	message(STATUS "face reco enable")
	
	# add defined
	add_definitions(-DFACE_RECO_TEST)
	# ffmpeg defined
	add_definitions(-D__STDC_CONSTANT_MACROS)
	
	# add src file
	set(src_files ${src_files} video.cpp)
	
	# add lib file
	set(lib_files ${lib_files} dlib png avcodec avdevice avformat swscale avutil opencv_core opencv_imgproc opencv_highgui X11 opencv_imgcodecs)
endif()

# check audio reco
option(AUDIO_RECO_TEST "This is a default option for AUDIO_RECO_TEST" OFF)
if (${AUDIO_RECO_TEST} STREQUAL "ON")
	# message
	message(STATUS "audio reco enable")
	
	# add defined
	add_definitions(-DAUDIO_RECO_TEST)
	# ffmpeg defined
	add_definitions(-D__STDC_CONSTANT_MACROS)
	
	# add src file
	set(src_files ${src_files} audio.cpp)
	
	# add lib file
	set(lib_files ${lib_files} avcodec avdevice avformat avutil pocketsphinx sphinxbase sphinxad)
endif()

# check bluez
option(BLUEZ_TEST "This is a default option for BLUEZ_TEST" OFF)
if (${BLUEZ_TEST} STREQUAL "ON")
	# message
	message(STATUS "bluez enable")
	
	# add defined
	add_definitions(-DBLUEZ_TEST)
	
	# add src file
	set(src_files ${src_files} bluez.cpp)
	
	# add lib file
	set(lib_files ${lib_files} bluetooth)
endif()

# check bluez server
option(BLUEZ_SERVER "This is a default option for BLUEZ_SERVER" OFF)
if (${BLUEZ_SERVER} STREQUAL "ON")
	# message
	message(STATUS "bluez server enable")
	
	# add defined
	add_definitions(-DBLUEZ_SERVER)
endif()

# check bluez client
option(BLUEZ_CLIENT "This is a default option for BLUEZ_CLIENT" OFF)
if (${BLUEZ_CLIENT} STREQUAL "ON")
	# message
	message(STATUS "bluez client enable")
	
	# add defined
	add_definitions(-DBLUEZ_CLIENT)
endif()

# Generate execute
add_executable(${PROJECT_NAME} ${src_files})

# Link Libraries
target_link_libraries(${PROJECT_NAME} ${lib_files})