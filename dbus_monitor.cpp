#include <dbus_monitor.h>

DBusMonitor::DBusMonitor()
	: m_bRun(false) {
	// init the errors
	dbus_error_init(&m_tDBusError);

	// connect to the bus
	m_pCon = dbus_bus_get(DBUS_BUS_SESSION, &m_tDBusError);

	// check
	if (dbus_error_is_set(&m_tDBusError)) {
		cout << "connect dbus " << m_tDBusError.message << endl;
	} else {
		dbus_connection_unref(m_pCon);
	}
}

DBusMonitor::~DBusMonitor() {
	// stop
	stop();
}

bool DBusMonitor::addMonitorMethod(const string& strMethod) {
	// add method
	dbus_bus_add_match(m_pCon, strMethod.c_str(), &m_tDBusError);
	
	// check
	if (dbus_error_is_set(&m_tDBusError)) {
		cout << "add method " << strMethod << " " << m_tDBusError.message << endl;
		return false;
	} 
	return true;
}

void DBusMonitor::start() {
	// set run
	m_bRun = true;
	
	// thread
	thread t([&](){		
		// loop
		while(m_bRun) {
			if (dbus_connection_read_write_dispatch(m_pCon, 10000)) {
				auto msg = dbus_connection_borrow_message(m_pCon);
				if (msg) {
					cout << dbus_message_get_sender(msg) << endl;
					cout << dbus_message_get_member(msg) << endl;
					cout << dbus_message_get_interface(msg) << endl;
					cout << dbus_message_get_path(msg) << endl;
					dbus_connection_return_message(m_pCon, msg);
				}
			}
		}
	});
	t.detach();
}

void DBusMonitor::stop() {
	// set run
	m_bRun = false;
	
	// release
	if (m_pCon) {
		// close
		dbus_connection_close(m_pCon);
		
		// free
		free(m_pCon);
		m_pCon = nullptr;
	}
}