#include <led.h>

Led::Led(int nPinNo)
	: m_nPinNo(nPinNo) {
	// set pin mode
	pinMode(m_nPinNo, OUTPUT);
}

Led::~Led() {
	// turn off led
	// no delay
	turnOff(0);
}

void Led::turnOn(int nKeepMsec) {
	// turn on led
    digitalWrite(m_nPinNo, HIGH);
    // sleep for next step
    delay(nKeepMsec);
}

void Led::turnOff(int nKeepMsec) {
	// turn off led
    digitalWrite(m_nPinNo, LOW);
    // sleep for next step
    delay(nKeepMsec);
}