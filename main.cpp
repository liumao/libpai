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

#if defined(LED_TEST)
	// init led module
	Led led(LED_PIN_1);
#endif
	
#if defined(WHEEL_TEST)
	// init wheels
	Wheel leftWheel(LEFT_WHEEL_1, LEFT_WHEEL_2);	
	Wheel rightWheel(RIGHT_WHEEL_1, RIGHT_WHEEL_2);
#endif
	
#if defined(FACE_RECO_TEST)
	// init face reco
	FaceReco faceReco("shape_predictor_68_face_landmarks.dat");
	
	// init video
	if (!faceReco.start("/dev/video0", {
			{"video_size", "320x240"},
			{"framerate", "30"},
			{"b", "360000"}
		})) {
		cout << "video init fail" << endl;
	}
#endif
	
	// led thread
	thread t([&]{
		// init script module
		Script cmd;
		
		// loop
		while (bRun) {
#if defined(LED_TEST)
			// turn on led
			led.turnOn(500);
			// turn off led
			led.turnOff(500);
#endif
			
			/// read temp
			int nTemp = atoi(cmd.executeCMD("cat /sys/class/thermal/thermal_zone0/temp"));
			cout << "cpu temp: " << nTemp / 1000 << endl;
			
#if defined(WHEEL_TEST)
			// forward
			leftWheel.forward(100, 0);	
			rightWheel.back(100, 0);
			delay(3000);
			
			// back
			leftWheel.back(100, 0);
			rightWheel.forward(100, 0);	
			delay(3000);
			
			// pause
			leftWheel.pause();
			rightWheel.pause();
#endif
			// delay
			delay(1000);
		}
	});
	t.detach();
	
	// wait input
	char input;
	while(cin >> input) {
		if (input == 'q') {
			bRun = false;
			
#if defined(LED_TEST)
			// turn off led
			led.turnOff(500);
#endif
			
#if defined(WHEEL_TEST)
			// pause
			leftWheel.pause();
			rightWheel.pause();
#endif

#if defined(FACE_RECO_TEST)
			// stop face reco
			faceReco.stop();
#endif
			break;
		}
		
		// sleep
		delay(1000);
	}
	
	// sleep
	delay(1000);
	return 0;
}
