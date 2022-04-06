#include <led.h>
#include <script.h>
#include <video.h>

/// \brief define pin
#define LED_PIN_1 1

int main(int argc, char* argv[]) {
	// init wiringpi 
	wiringPiSetup();
	
	// run flags
	bool bRun = true;

	// init led module
	Led led(LED_PIN_1);
	
	// led thread
	thread t([&]{
		// init script module
		Script cmd;
		
		// loop
		while (bRun) {
			// turn on led
			led.turnOn(500);
			// turn off led
			led.turnOff(500);
			
			/// read temp
			int nTemp = atoi(cmd.executeCMD("cat /sys/class/thermal/thermal_zone0/temp"));
			cout << "cpu temp: " << nTemp / 1000 << endl;
		}
	});
	t.detach();
	
	// ffmpeg register
	avcodec_register_all();
    avdevice_register_all();
	
	// open device
	auto pInFormat = av_find_input_format(DEVICE_NAME);
	if (!pInFormat) {		
		// log
		cout << "av_find_input_format " << DEVICE_NAME << " error" << endl;
		return 1;
	}
	
	// init dlib
	auto detector = get_frontal_face_detector();
	
	// timestamp
	auto lastTime = chrono::system_clock::now().time_since_epoch();
	
	// video 
	Video video(pInFormat, [&](Mat &mat) {
		// check time stamp
		auto now = chrono::system_clock::now().time_since_epoch();
		if (now.count() - lastTime.count() < 1000000000) {
			return;
		}
		lastTime = now;
		
		// image 
		array2d<unsigned char> img;
		
		// gray mat
		Mat gray;
		cvtColor(mat, gray, COLOR_RGB2GRAY);
		dlib::assign_image(img, dlib::cv_image<unsigned char>(gray));
		
		// face vector
		std::vector<dlib::rectangle> dets = detector(img);
		
		// log
		cout << "Number of faces detected: " << dets.size() << endl;
	});

	// init video
	if (!video.init("/dev/video0", {
			{"video_size", "320x240"},
			{"framerate", "30"},
			{"b", "360000"}
		})) {
		cout << "video init fail" << endl;
	} else {
		video.start();
	}
	
	// wait input
	char input;
	while(cin >> input) {
		if (input == 'q') {
			bRun = false;
			
			// turn off led
			led.turnOff(500);
			
			// stop video
			video.stop();
			break;
		}
		
		// sleep
		delay(1000);
	}
	
	// sleep
	delay(1000);
	return 0;
}
