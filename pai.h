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

/// \brief common headers
#include <wiringPi.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <glib.h>
#include <gio/gio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <cstring>
#include <functional>
#include <vector>
#include <map>
#include <list>
#include <chrono>
#include <sstream>
#include <condition_variable>
using namespace std;

#if defined(FACE_RECO_TEST) || defined(AUDIO_RECO_TEST)
/// \brief define params map
typedef const map<string, string> ParamsMap;

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

/// \brief callback
typedef function<void(AVPacket *)> AVCallBack;

/// \brief error max size
#define ERROR_MAX_SIZE 1024
#endif

#if defined(FACE_RECO_TEST)
/// \brief device type
#define VIDEO_DEVICE_NAME "video4linux2"

/// \brief dlib header
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_io.h>
#include <dlib/image_processing.h>
#include <dlib/opencv.h>
#include <dlib/dnn.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
using namespace dlib;

/// \brief opencv
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
using namespace cv;

/// \brief face location
typedef struct tagFaceLoc {
	long m_nLeft;
	long m_nRight;
	long m_nTop;
	long m_nBottom;
}TFaceLoc, *PFaceLoc;

/// \brief callback
typedef function<void(TFaceLoc &)> LocCallBack;
#endif

#if defined(AUDIO_RECO_TEST)
/// \brief device type
#define AUDIO_DEVICE_NAME "alsa"
/// \brief pocketsphinx
#include <pocketsphinx.h>

/// \brief callback
typedef function<void(const char *)> CmdCallBack;
#endif

#if defined(BLUEZ_TEST)
/// \brief name size
#define NAME_SIZE 248
/// \brief mac size
#define MAC_SIZE 19

/// \brief node info
typedef struct tagNodeInfo {
	/// \brief device's mac
	char m_cMac[MAC_SIZE];
	
	/// \brief device's name
	char m_cName[NAME_SIZE];
}TNodeInfo, *PNodeInfo;
#endif

#if defined(DBUS_TEST)
/// \brief dbus header
#include <dbus/dbus.h>
#endif

/// \brief get current time stamp
///
/// \return current time stamp
static time_t getCurTimeStamp() {
	auto ms = chrono::time_point_cast<chrono::milliseconds>(chrono::system_clock::now());
	return ms.time_since_epoch().count();
}

/// \brief get cpu num
///
/// \return cpu num
static int getCpuNum() {
	int nCpusNum = sysconf(_SC_NPROCESSORS_CONF);
	return nCpusNum;
}

template<class T>
class SingleTon {
public:
	/// \brief constructor
	SingleTon(const SingleTon&) = delete;

	/// \brief destructor
	SingleTon& operator=(const SingleTon&) = delete;

	/// \brief instance
	///
	/// \return instance of object
	static T* instance() {
		if (m_pInstance == nullptr) {
			m_pInstance = new T;
			atexit(destory);
		}
		return m_pInstance;
	}

private:
	/// \brief destory instance
	///
	/// \return 
	static void destory() {
		if (m_pInstance) {
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	/// \brief T's instance
	static T* m_pInstance;

};
template<class T> T* SingleTon<T>::m_pInstance = nullptr;

/// \brief task processor priority
typedef enum class TPPriority : int {
	High = 0,
	Low
}TTPPriority;

/// \brief task processor timeout
#define TASK_PROCESSOR_TIMEOUT 200

template<class T>
class TaskProcessor {
public:
	/// \brief constructor
	TaskProcessor(int nId = -1, TPPriority nTPPriority = TPPriority::Low)
		: m_nId(nId),
		m_nTPPriority(nTPPriority),
		m_nLastTime(getCurTimeStamp()),
		m_bRun(false) {
	}

	/// \brief destructor
	virtual ~TaskProcessor() {
		// stop the worker thread
		stop();
		// clear task deque
		m_vecTask.clear();
	}

	/// \brief start
	/// 
	/// start processor
	///
	/// \return 
	void start() {
		// update processor run flag
		m_bRun = true;

		// init processor
		m_pWorker = new thread([this] { 
			process();
		});
		m_pWorker->detach();
	}

	/// \brief stop
	/// 
	/// stop processor
	///
	/// \return 
	void stop() {
		// set flag
		m_bRun = false;
        // sleep for safe
        this_thread::sleep_for(chrono::milliseconds(50));
		// release thread
		if (m_pWorker) {
			delete m_pWorker;
			m_pWorker = nullptr;
		}
	}

	/// \brief push task
	/// 
	/// push task into queue
	///
	/// \return 
	void pushTask(T pTask) {
		unique_lock<std::mutex> lck(m_mutex);
		m_vecTask.push_back(pTask);
		m_cv.notify_one();
	}

	/// \brief pop task
	/// 
	/// pop task from queue
	///
	/// \return 
	T popTask() {
		// T can be base pointer or shared pointer
		// for example: A*, shared_ptr<A>
		T t = m_vecTask.front();
		m_vecTask.pop_front();
		return t;
	}

	/// \brief get id
	/// 
	/// get task processor id
	///
	/// \return task processor's id
	long getId() const {
		return m_nId;
	}

	/// \brief get priority
	/// 
	/// get task processor priority
	///
	/// \return task processor's priority
	TPPriority getPriority() const {
		return m_nTPPriority;
	}

	/// \brief get last process time
	/// 
	/// get task processor last process time
	///
	/// \return task processor's last process time
	time_t getLastProcessTime() const {
		return m_nLastTime;
	}

	/// \brief get task num
	/// 
	/// get task remain num
	///
	/// \return remain num
	size_t getTaskNum() const {
		return m_vecTask.size();
	}

protected:
	/// \brief execute task
	///
	/// inherited class must implements this method
	/// task processor provide an entry
	///
	/// for example:
	/// 
	/// class A {};
	/// 
	/// class MyTaskProcessor : public TaskProcessor<A *> {
	/// public:
	///    void ExecTask(A *a) {}
	/// };
	virtual void execTask(T task) = 0;

private:
	/// \brief process
	void process() {
		unique_lock<std::mutex> lck(m_mutex);
		while (m_bRun) {
			// task size 0, wait again
			// if not, pop task and exec task
			while (m_bRun && m_vecTask.empty()) {
				m_cv.wait_for(lck, chrono::milliseconds(TASK_PROCESSOR_TIMEOUT));
			}

			// worker be canceled when be wakeup
			if (!m_bRun) {
				break;
			}

			// pop the first task
			auto t = popTask();
			// unlock mutex
			// we can put more task into list
			lck.unlock();
			// process data
			execTask(t);
			// unlock
			lck.lock();
		}
	}

private:
	/// \brief task processor's id
	long m_nId;

	/// \brief task processor's priority
	TPPriority m_nTPPriority;

	/// \brief last process task time stamp
	time_t m_nLastTime;

	/// \brief thread is running or not
	bool m_bRun;
	/// \brief task processor thread
	thread *m_pWorker;

	/// \brief mutex
	std::mutex m_mutex;
	/// \brief condition of thread
	condition_variable m_cv;

	/// \brief deque storage task to deal with
	list<T> m_vecTask;

};

template <class T>
class TPPool {
public:
	/// \brief constructor
	TPPool() {
		for (auto i = 0; i < 2; i++) {
			auto tp = new ETProcessor();
			m_lstETProcessor.push_back(tp);
			tp->start();
		}
	}

	/// \brief destructor
	virtual ~TPPool() {
		for (auto& iter : m_lstETProcessor) {
			iter->stop();
		}
		m_lstETProcessor.clear();
	}

	/// \brief dispatch task
	/// 
	/// \param [in] task new task
	///
	/// \return 
	void dispatchTask(T task) {
		// find the best task processor
		ETProcessor *pETP = nullptr;
		{
			// lock
			unique_lock<std::mutex> lck(m_mutex);
			for (auto& iter : m_lstETProcessor) {
				if (!pETP || pETP->getTaskNum() > iter->getTaskNum()) {
					pETP = iter;
				}
			}
		}

		// process task
		if (pETP) {
			pETP->pushTask(task);
		}
	}

	class ETProcessor : public TaskProcessor<T> {
	public:
		ETProcessor()
			: TaskProcessor<T>(-1, TPPriority::High) {
		}

		virtual ~ETProcessor() = default;

		void execTask(T task) {
			task->process();
		}
	};

private:
	/// \brief mutex
	std::mutex m_mutex;
	/// \brief event task processor
	list<ETProcessor*> m_lstETProcessor;

};

/// \brief task type
typedef enum class TaskType : int {
	None = -1,
	Forward,
	Back,
	Left,
	Right
}TTaskType;

/// \brief task
typedef struct tagTask {
	// process
	virtual void process() = 0;
}TTask, *PTask;

#endif //__PAI_H__
