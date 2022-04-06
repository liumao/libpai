/*
Copyright (C) 2004  loveyou12300liumao@163.com
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/// \file pai.h
/// \brief pai header
///
/// \author liumao
/// \version v1.0
/// \date 30 Mar 2022
 
#ifndef __PAI_H__
#define __PAI_H__

/// \brief ffmpeg header
extern "C" {
#include <libavdevice/avdevice.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
}

/// \brief common headers
#include <wiringPi.h>
#include <iostream>
#include <thread>
#include <cstring>
#include <functional>
#include <vector>
#include <map>
#include <chrono>
using namespace std;

/// \brief device type
#if defined (WIN32)
#define DEVICE_NAME "dshow"
#elif defined(__linux__) || defined(__arm__)
#define DEVICE_NAME "video4linux2"
#elif defined(__APPLE__) && defined(__GNUC__)
#define DEVICE_NAME "avfoundation"
#endif

/// \brief define params map
typedef const map<string, string> ParamsMap;

/// \brief dlib header
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
using namespace dlib;

/// \brief opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
using namespace cv;

/// \brief callback
typedef function<void(Mat &)> AVCallBack;

#endif //__PAI_H__
