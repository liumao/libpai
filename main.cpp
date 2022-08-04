#if defined(LED_TEST)
#include <led.h>
#endif
#include <script.h>
#if defined(WHEEL_TEST)
#include <wheel.h>
#endif
#if defined(FACE_RECO_TEST)
#include <video.h>
#endif
#if defined(AUDIO_RECO_TEST)
#include <audio.h>
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

/// \brief line size
#define CV_LINE_SIZE 1

int main(int argc, char* argv[]) {
	// init wiringpi 
	wiringPiSetup();
	
	// run
	bool bRun = true;

#if defined(LED_TEST)
	// init led module
	Led led(LED_PIN_1);
#endif
	
#if defined(WHEEL_TEST)
	// init wheels
	auto wheel = make_shared<Wheel>(LEFT_WHEEL_1, LEFT_WHEEL_2, RIGHT_WHEEL_1, RIGHT_WHEEL_2);	
#endif

#if defined(FACE_RECO_TEST) || defined(AUDIO_RECO_TEST)
	// ffmpeg register
	avcodec_register_all();
    avdevice_register_all();
#endif
	
#if defined(FACE_RECO_TEST)
	// name
	string strName = "";
	
	// detector
	auto pFaceDetector = get_frontal_face_detector();
	// init shaped predictor
	shape_predictor pShapePredictor;
	deserialize("shape_predictor_68_face_landmarks.dat") >> pShapePredictor;
	// init anet type
	ANET_TYPE pFaceRecoDNN;
	deserialize("dlib_face_recognition_resnet_model_v1.dat") >> pFaceRecoDNN;
	
	// show win
	dlib::image_window win;
	
	// face frame count
	long nFaceFrame = 0;
	
	// face position
	std::vector<dlib::rectangle> dets;
	
	// create video
	Video video([&](AVPacket *packet) -> void {
			// decoder
			auto decoder = video.getDecoder();
			
			// frmae
			AVFrame* pFrame = av_frame_alloc();
			
			// got
			int nGot;
			if (avcodec_decode_video2(decoder, pFrame, &nGot, packet) < 0 || !nGot) {
				// release av frame
				av_frame_free(&pFrame);
				return;
			}

			// resample display
			auto matDis = video.resampleDisplay(pFrame);

			// check num
			if (nFaceFrame++ % 5 == 0) {
				// resample detect
				auto matDet = video.resampleDetect(pFrame);
	
				// gray mat
				Mat gray;
				cvtColor(*matDet, gray, COLOR_RGB2GRAY);
				
				// image 
				array2d<unsigned char> img;
				dlib::assign_image(img, dlib::cv_image<unsigned char>(gray));
				// face vector
				dets = pFaceDetector(img);
			}
			
			// display face position
			for (auto det = dets.begin(); det != dets.end(); det++) {
				cv::Rect rect((*det).left() * 2, (*det).top() * 2, (*det).width() * 2, (*det).height() * 2);
				cv::rectangle(*matDis, rect, cv::Scalar(0, 0, 255), 1, 1, 0);
				cv::Point origin = {(*det).left() * 2, (*det).top() * 2};
				cv::putText(*matDis, "", origin, cv::FONT_HERSHEY_COMPLEX, 1, cv::Scalar(255, 0, 0), 2, 2, 0);
				
				// take 68 pointer feature
				//full_object_detection shape = pShapePredictor(img, *det);
				// 128 face vector
				//dlib::matrix<dlib::rgb_pixel> faceChip;
				//dlib::extract_image_chip(img, dlib::get_face_chip_details(shape, 150, 0.25), faceChip);
			
				// get 128 face vector
				//dlib::matrix<float, 0, 1> face = pFaceRecoDNN(faceChip);
			}
			
			// dlib show image
			dlib::cv_image<rgb_pixel> imgD(*matDis);
			win.clear_overlay();
			win.set_image(imgD);
			
			// release av frame
			av_frame_free(&pFrame);
		});
	
	// init video
	if (!video.init("/dev/video0", {
			{"video_size", "640x480"},
			{"framerate", "30"},
			{"b", "360000"}
		})) {
		cout << "video init fail" << endl;
	}

	// set display sws context
	if (!video.setDisplaySwsContext(640, 480, AVPixelFormat::AV_PIX_FMT_BGR24)) {
		cout << "set display sws context fail" << endl;
	}
	// set detect sws context
	if (!video.setDetectSwsContext(320, 240, AVPixelFormat::AV_PIX_FMT_BGR24)) {
		cout << "set detect sws context fail" << endl;
	}
	video.start();
#endif

#if defined(AUDIO_RECO_TEST)
	// create audio
	Audio audio([&](AVPacket *packet) -> void {
		
	});
	
	// init audio
	if (!audio.init("hw:2", {
			{"sample_rate", "8000"},
			{"b", "16000"},
			{"channels", "1"}
		})) {
		cout << "audio init fail" << endl;
	}
	audio.start();
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
			wheel->forward(0, 0);
		} else if (!strcmp(pData, "后退")) {
			wheel->back(0, 0);
		} else if (!strcmp(pData, "左转")) {
			wheel->left(0, 0);
		} else if (!strcmp(pData, "右转")) {
			wheel->right(0, 0);
		} else if (!strcmp(pData, "暂停")) {
			wheel->pause();
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
			wheel->pause();
#endif

#if defined(FACE_RECO_TEST)
			// stop video
			video.stop();
#endif

#if defined(BLUEZ_TEST)
#if defined(BLUEZ_SERVER)			
			// stop bluez
			bSvr.stopServer();
#endif
#endif

#if defined(AUDIO_RECO_TEST)
			// stop audio
			audio.stop();
#endif
			break;
		} else if (input == 'f') {
#if defined(WHEEL_TEST)
			wheel->forward(0, 3000);
#endif
		} else if (input == 'b') {
#if defined(WHEEL_TEST)
			wheel->back(0, 3000);
#endif
		} else if (input == 'l') {
#if defined(WHEEL_TEST)
			wheel->left(0, 500);
#endif
		} else if (input == 'r') {
#if defined(WHEEL_TEST)
			wheel->right(0, 500);
#endif
		} else if (input == 'i') {
#if defined(FACE_RECO_TEST)
#endif
		} else if (input == 'n') {
#if defined(FACE_RECO_TEST)
			cout << "input name: ";
			cin >> strName;
#endif
		} else if (input == 'c') {
#if defined(FACE_RECO_TEST)
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
