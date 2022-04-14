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
		
		// log
		cout << "distance " << distance << endl;
		
		// turn left or right
		if (tFaceLoc.m_nRight + tFaceLoc.m_nLeft < 480) {
#if defined(WHEEL_TEST)
			wheel.right(0, 100);
#endif
			// log
			cout << "=========right" << endl;
		} else if (tFaceLoc.m_nRight + tFaceLoc.m_nLeft > 800) {
#if defined(WHEEL_TEST)
			wheel.left(0, 100);
#endif
			// log
			cout << "=========left" << endl;
		} else {
			// check distance
			if (distance > 150) {
#if defined(WHEEL_TEST)
				wheel.forward(0, 1000);
#endif
				// log
				cout << "=========forward" << endl;
			} else if (distance < 100) {
#if defined(WHEEL_TEST)
				wheel.back(0, 1000);
#endif
				// log
				cout << "=========back" << endl;
			}
		}
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
	AudioReco audioReco(pAudInFormat, [&](const char *cmd) {
		cout << "=========cmd " << cmd << endl;
		
		// check cmd
		if (!strcmp(cmd, "forward")) {
#if defined(WHEEL_TEST)
			wheel.forward(0, 1000);
#endif
		} else if (!strcmp(cmd, "back")) {
#if defined(WHEEL_TEST)
			wheel.back(0, 1000);
#endif			
		} else if (!strcmp(cmd, "left")) {
#if defined(WHEEL_TEST)
			wheel.left(0, 1000);
#endif			
		} else if (!strcmp(cmd, "right")) {
#if defined(WHEEL_TEST)
			wheel.right(0, 1000);
#endif			
		}
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
	// init bluez
	BlueZ blueZ;
#endif
	
	// wait input
	char input;
	while(cin >> input) {
		if (input == 'q') {			
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

#if defined(AUDIO_RECO_TEST)
			// stop audio reco
			audioReco.stop();
#endif
			break;
		} else if(input == 's') {
#if defined(BLUEZ_TEST)
			// scan
			vector<TNodeInfo> vNodes;
			blueZ.scan(vNodes);
			
			// log
			for (auto i = 0; i < vNodes.size(); i++) {
				cout << "index " << i << ", mac " << vNodes[i].m_cMac << ", name " << vNodes[i].m_cName << endl;
			}
#endif
		} else if (input == 'p') {
#if defined(BLUEZ_TEST)
			// pair
			if (!blueZ.pair("D0:05:E4:0C:DB:54")) {
			}
#endif
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
	
	// sleep
	delay(3000);
	return 0;
}
