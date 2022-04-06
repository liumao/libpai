#include <wheel.h>

Wheel::Wheel(int nPin0, int nPin1)
	: m_nPin0(nPin0),
	m_nPin1(nPin1) {
	// set pin mode
	pinMode(m_nPin0, OUTPUT);
	pinMode(m_nPin1, OUTPUT);
}

Wheel::~Wheel() {
	// pause
	pause();
}

void Wheel::forward(int nSpeed, int nKeepMsec) {
	// set pin0 high	
    digitalWrite(m_nPin0, HIGH);
	// set pin1 low
    digitalWrite(m_nPin1, LOW);
    // sleep for next step
    delay(nKeepMsec);
}

void Wheel::back(int nSpeed, int nKeepMsec) {
	// set pin0 low	
    digitalWrite(m_nPin0, LOW);	
	// set pin1 high
    digitalWrite(m_nPin1, HIGH);
    // sleep for next step
    delay(nKeepMsec);
}

void Wheel::pause() {
	// set pin0 low	
    digitalWrite(m_nPin0, LOW);
	// set pin1 low
    digitalWrite(m_nPin1, LOW);
}