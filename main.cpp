#if defined(LED_TEST)
#include <led.h>
#endif
#include <script.h>
#if defined(WHEEL_TEST)
#include <wheel.h>
#endif
#if defined(FACE_RECO_TEST)
#include <face_reco.h>
#endif
#if defined(AUDIO_RECO_TEST)
#include <audio_reco.h>
#endif
#if defined(BLUEZ_TEST)
#include <bluez.h>
#endif
#if defined(DBUS_TEST)
#include <dbus_monitor.h>
#endif

/// \brief define pin
#define LED_PIN_1 29

/// \brief define left wheel pin
#define LEFT_WHEEL_1 0
#define LEFT_WHEEL_2 1

/// \brief define right wheel pin
#define RIGHT_WHEEL_1 2
#define RIGHT_WHEEL_2 4

int main(int argc, char* argv[]) {
	// init wiringpi 
	wiringPiSetup();
	
	// run
	bool bRun = true;
	
	// check measure temp
	thread t([&]() {
		while (bRun) {
			cout << Script().executeCMD("vcgencmd measure_temp");
			
			// sleep
			delay(10000);
		}
	});
	t.detach();

#if defined(LED_TEST)
	// init led module
	Led led(LED_PIN_1);
#endif
	
#if defined(WHEEL_TEST)
	// init wheels
	Wheel wheel(LEFT_WHEEL_1, LEFT_WHEEL_2, RIGHT_WHEEL_1, RIGHT_WHEEL_2);	
#endif

#if defined(FACE_RECO_TEST) || defined(AUDIO_RECO_TEST)
	// ffmpeg register
	avcodec_register_all();
    avdevice_register_all();
#endif
	
#if defined(FACE_RECO_TEST)
	// open video device
	auto pVidInFormat = av_find_input_format(VIDEO_DEVICE_NAME);
	if (!pVidInFormat) {		
		// log
		cout << "av_find_input_format " << VIDEO_DEVICE_NAME << " error" << endl;
		return 1;
	}
#endif

#if defined(AUDIO_RECO_TEST)
	// open audio device
	auto pAudInFormat = av_find_input_format(AUDIO_DEVICE_NAME);
	if (!pAudInFormat) {		
		// log
		cout << "av_find_input_format " << AUDIO_DEVICE_NAME << " error" << endl;
		return 1;
	}
#endif
	
#if defined(FACE_RECO_TEST)
	// init face reco
	FaceReco faceReco(pVidInFormat, "shape_predictor_68_face_landmarks.dat", [&](TFaceLoc &tFaceLoc) {
		// calc distance
		auto distance = 25 * 2.8 * 400 / (tFaceLoc.m_nBottom - tFaceLoc.m_nTop);
		cout << "distance " << distance << ", left " << tFaceLoc.m_nLeft << ", right " << tFaceLoc.m_nRight << endl;
		
		// turn left or right
#if defined(WHEEL_TEST)
		if (tFaceLoc.m_nRight + tFaceLoc.m_nLeft < 480) {
			wheel.right(0, 100);
		} else if (tFaceLoc.m_nRight + tFaceLoc.m_nLeft > 800) {
			wheel.left(0, 100);
		} else if (distance > 150) { // check distance
			wheel.forward(0, 1000);
		} else if (distance < 100) {
			wheel.back(0, 1000);
		}
#endif
	});
	
	// init video
	if (!faceReco.start("/dev/video0", {
			{"video_size", "640x480"},
			{"framerate", "30"},
			{"b", "360000"}
		})) {
		cout << "video init fail" << endl;
	}
#endif

#if defined(AUDIO_RECO_TEST)
	// init audio reco
	AudioReco audioReco(pAudInFormat, "./zh_cn.cd_cont_5000", "./zh_cn.lm.bin", "./zh_cn.dic", [&](const char *cmd) {
		cout << "=========cmd " << cmd << endl;
		
		// check cmd
#if defined(WHEEL_TEST)
		if (!strcmp(cmd, "前进")) {
			wheel.forward(0, 1000);
		} else if (!strcmp(cmd, "后退")) {
			wheel.back(0, 1000);
		} else if (!strcmp(cmd, "左转")) {
			wheel.left(0, 500);
		} else if (!strcmp(cmd, "右转")) {
			wheel.right(0, 500);
		}
#endif
	});
	
	// init audio
	if (!audioReco.start("hw:1", {
			{"sample_rate", "8000"},
			{"b", "16000"},
			{"channels", "1"}
		})) {
		cout << "audio init fail" << endl;
	}
#endif

#if defined(BLUEZ_TEST)
#if defined(BLUEZ_SERVER)
	// server bluez
	BlueZ bSvr([&](const char *pData, bool bServer) {
		// log
		cout << "bluez server " << pData << endl;
		// check data
#if defined(WHEEL_TEST)
		if (!strcmp(pData, "前进")) {
			wheel.forward(0, 0);
		} else if (!strcmp(pData, "后退")) {
			wheel.back(0, 0);
		} else if (!strcmp(pData, "左转")) {
			wheel.left(0, 0);
		} else if (!strcmp(pData, "右转")) {
			wheel.right(0, 0);
		} else if (!strcmp(pData, "暂停")) {
			wheel.pause();
		}
#endif
	});
	// start bluez service
	bSvr.startServer(1);
	
	// check bluetooth status
	thread tCheck([&]() {
		while (bRun) {
			// sleep
			delay(BLUETOOTH_DISCOVERABLE_TIMEOUT * 1000);
			
			// check status
			if (bSvr.getClientId() <= 0) {
				// log
				cout << "restart bluetooth" << endl;

				// stop bluez
				bSvr.stopServer();
				
				// sleep
				delay(2000);
				
				// start bluez service
				bSvr.startServer(1);
			}
		}
	});
	tCheck.detach();
#endif
	
#if defined(BLUEZ_CLIENT)
	// client bluez
	BlueZ bCli([&](const char *pData, bool bServer) {
		cout << "received " << pData << endl;
	});
	if (bCli.createClient("DC:A6:32:30:A1:3B", 1)) {
		thread t1([&]() {
			while (bRun) {
				// send message
				bCli.sendMsg("hello world!");
				// sleep
				delay(10000);
			}
		});
		t1.detach();
	}
#endif
#endif

#if defined(DBUS_TEST)
	// init dbus monitor
	DBusMonitor helloSDBus("hello.world.service");
	helloSDBus.init();
	helloSDBus.addMonitorMethod("type='signal',interface='hello.world.client'");
	helloSDBus.start();
	
	DBusMonitor clientCDBus("hello.world.client");
	clientCDBus.init();
	clientCDBus.sendSignal("hello world", "/hello/world/client", "hello.world.client", "hello");
#endif
	
	// wait input
	char input;
	while(cin >> input) {
		if (input == 'q') {	
			// set run
			bRun = false;
			
#if defined(LED_TEST)
			// turn off led
			led.turnOff(0);
#endif
			
#if defined(WHEEL_TEST)
			// pause
			wheel.pause();
#endif

#if defined(FACE_RECO_TEST)
			// stop face reco
			faceReco.stop();
#endif

#if defined(BLUEZ_TEST)
#if defined(BLUEZ_SERVER)			
			// stop bluez
			bSvr.stopServer();
#endif
#endif

#if defined(AUDIO_RECO_TEST)
			// stop audio reco
			audioReco.stop();
#endif
			break;
		} else if (input == 'f') {
#if defined(WHEEL_TEST)
			wheel.forward(0, 3000);
#endif
		} else if (input == 'b') {
#if defined(WHEEL_TEST)
			wheel.back(0, 3000);
#endif
		} else if (input == 'l') {
#if defined(WHEEL_TEST)
			wheel.left(0, 500);
#endif
		} else if (input == 'r') {
#if defined(WHEEL_TEST)
			wheel.right(0, 500);
#endif
		}
		
		// sleep
		delay(1000);
	}
	cout << "finishing..." << endl;
	
	// sleep
	delay(3000);
	return 0;
}
