#include <led.h>
#include <script.h>
#include <video.h>
#include <wheel.h>

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
	
	// run flags
	bool bRun = true;

	// init led module
	Led led(LED_PIN_1);
	
	// init wheels
	Wheel leftWheel(LEFT_WHEEL_1, LEFT_WHEEL_2);	
	Wheel rightWheel(RIGHT_WHEEL_1, RIGHT_WHEEL_2);
	
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
			
			// forward
			leftWheel.forward(100, 0);	
			rightWheel.forward(100, 0);	
			delay(1000);
			
			// back
			leftWheel.back(100, 0);
			rightWheel.back(100, 0);
			delay(1000);
			
			// pause
			leftWheel.pause();
			rightWheel.pause();
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
	// dectect objetc
	auto detector = get_frontal_face_detector();
	
	// shape predictor
	shape_predictor sp;
	deserialize("shape_predictor_68_face_landmarks.dat") >> sp;
	
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
		
		// each face 68 feature point
		std::vector<dlib::full_object_detection> shapes;
		for (unsigned long i = 0; i < dets.size(); ++i) {
			shapes.push_back(sp(img, dets[i])); 
		}
		
		// log number
		cout << "Number of faces detected: " << dets.size() << ", Number of faces 68 feature point: " << shapes.size() << endl;
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
