#include <wheel.h>

Wheel::Wheel(int nPin0, int nPin1, int nPin2, int nPin3)
	: m_nPin0(nPin0),
	m_nPin1(nPin1),
	m_nPin2(nPin2),
	m_nPin3(nPin3),
	m_nTaskType(TaskType::None),
	m_nKeepMsec(0),
	m_nSpeed(0) {
	// set pin mode
	pinMode(m_nPin0, OUTPUT);
	pinMode(m_nPin1, OUTPUT);
	pinMode(m_nPin2, OUTPUT);
	pinMode(m_nPin3, OUTPUT);
}

Wheel::~Wheel() {
	// pause
	pause();
}

void Wheel::forward(int nSpeed, int nKeepMsec) {
	// update task type
	m_nTaskType = TaskType::Forward;
	
	// update speed and keep times
	m_nKeepMsec = nKeepMsec;
	m_nSpeed = nSpeed;
	
	// dispatch task
	SingleTon<TPPool<TTask*>>::instance()->dispatchTask(this);
}

void Wheel::back(int nSpeed, int nKeepMsec) {
	// update task type
	m_nTaskType = TaskType::Back;
	
	// update speed and keep times
	m_nKeepMsec = nKeepMsec;
	m_nSpeed = nSpeed;
	
	// dispatch task
	SingleTon<TPPool<TTask*>>::instance()->dispatchTask(this);
}

void Wheel::left(int nSpeed, int nKeepMsec) {
	// update task type
	m_nTaskType = TaskType::Left;
	
	// update speed and keep times
	m_nKeepMsec = nKeepMsec;
	m_nSpeed = nSpeed;
	
	// dispatch task
	SingleTon<TPPool<TTask*>>::instance()->dispatchTask(this);
}

void Wheel::right(int nSpeed, int nKeepMsec) {
	// update task type
	m_nTaskType = TaskType::Right;
	
	// update speed and keep times
	m_nKeepMsec = nKeepMsec;
	m_nSpeed = nSpeed;
	
	// dispatch task
	SingleTon<TPPool<TTask*>>::instance()->dispatchTask(this);
}

void Wheel::pause() {
	// set pin0 low	
    digitalWrite(m_nPin0, LOW);
	// set pin1 low
    digitalWrite(m_nPin1, LOW);
	// set pin2 low	
    digitalWrite(m_nPin2, LOW);
	// set pin3 low
    digitalWrite(m_nPin3, LOW);
	// task type
	m_nTaskType = TaskType::None;
	// keep times
	m_nKeepMsec = 0;
	// speed
	m_nSpeed = 0;
}

void Wheel::process() {
	switch (m_nTaskType) {
	case TaskType::Forward: {
		// log
		cout << "forward" << endl;
		// set pin0 high	
		digitalWrite(m_nPin0, HIGH);
		// set pin1 low
		digitalWrite(m_nPin1, LOW);
		// set pin0 high	
		digitalWrite(m_nPin2, HIGH);
		// set pin1 low
		digitalWrite(m_nPin3, LOW);
		// sleep for next step
		delay(m_nKeepMsec);
		// stop
		pause();
	}
		break;
	case TaskType::Back: {
		// log
		cout << "back" << endl;
		// set pin0 low	
		digitalWrite(m_nPin0, LOW);	
		// set pin1 high
		digitalWrite(m_nPin1, HIGH);
		// set pin2 low	
		digitalWrite(m_nPin2, LOW);	
		// set pin3 high
		digitalWrite(m_nPin3, HIGH);
		// sleep for next step
		delay(m_nKeepMsec);
		// stop
		pause();
	}
		break;
	case TaskType::Left: {
		// log
		cout << "left" << endl;
		// set pin0 low	
		digitalWrite(m_nPin0, LOW);	
		// set pin1 high
		digitalWrite(m_nPin1, HIGH);
		// set pin2 low	
		digitalWrite(m_nPin2, HIGH);	
		// set pin3 high
		digitalWrite(m_nPin3, LOW);
		// sleep for next step
		delay(m_nKeepMsec);
		// stop
		pause();
	}
		break;
	case TaskType::Right: {
		// log
		cout << "right" << endl;
		// set pin0 low	
		digitalWrite(m_nPin0, HIGH);	
		// set pin1 high
		digitalWrite(m_nPin1, LOW);
		// set pin2 low	
		digitalWrite(m_nPin2, LOW);	
		// set pin3 high
		digitalWrite(m_nPin3, HIGH);
		// sleep for next step
		delay(m_nKeepMsec);
		// stop
		pause();
	}
		break;
	default:
		break;
	}
}