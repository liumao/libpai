#include <led.h>
#include <script.h>

/// \brief define pin
#define LED_PIN_1 1

int main(int argc, char* argv[]) {
	/// init wiringpi 
	wiringPiSetup();
	
	/// run flags
	bool bRun = true;

	/// init led module
	Led led(LED_PIN_1);
	
	/// led thread
	thread t([&]{
		/// init script module
		Script cmd;
		
		/// loop
		while (bRun) {
			/// turn on led
			led.turnOn(500);
			/// turn off led
			led.turnOff(500);
			
			/// read temp
			int nTemp = atoi(cmd.executeCMD("cat /sys/class/thermal/thermal_zone0/temp"));
			cout << "cpu temp: " << nTemp / 1000 << endl;
		}
	});
	t.detach();
	
	/// wait input
	char input;
	while(cin >> input) {
		if (input == 'q') {
			bRun = false;
	
			/// turn off led
			led.turnOff(500);

			break;
		}
		
		/// sleep
		delay(1000);
	}
	return 0;
}
