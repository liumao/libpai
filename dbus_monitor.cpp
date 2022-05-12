#include <dbus_monitor.h>

DBusMonitor::DBusMonitor(string strRequestName)
	: m_bRun(false),
	m_strRequestName(strRequestName) {
	// init the errors
	dbus_error_init(&m_tDBusError);
}

DBusMonitor::~DBusMonitor() {
	// stop
	stop();
}

bool DBusMonitor::init() {
	// connect to the bus
	m_pCon = dbus_bus_get(DBUS_BUS_SESSION, &m_tDBusError);

	// check
	if (dbus_error_is_set(&m_tDBusError)) {
		cout << "connect dbus " << m_tDBusError.message << endl;
		
		// release dbus error
		dbus_error_free(&m_tDBusError);
		return false;
	}
		
	// request name
	auto nRet = dbus_bus_request_name(m_pCon, m_strRequestName.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING, &m_tDBusError);
	// check
	if (dbus_error_is_set(&m_tDBusError) || nRet != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
		cout << "request dbus name " << m_tDBusError.message << endl;
		
		// release dbus error
		dbus_error_free(&m_tDBusError);
		return false;
	}
	return true;
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

bool DBusMonitor::sendSignal(const char* pData, 
							 const string& strPath, 
							 const string& strInterFaceName, 
							 const string& strSignalName) {
	// create dbus message
	DBusMessage *msg = dbus_message_new_signal(strPath.c_str(), strInterFaceName.c_str(), strSignalName.c_str());
	if (!msg) {
		cout << "message is null" << endl;
		return false;
	}
	
	// update params
	DBusMessageIter arg;
	dbus_message_iter_init_append(msg, &arg);
	dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &pData);
	
	// push
	dbus_connection_send(m_pCon, msg, nullptr);
	dbus_connection_flush(m_pCon);
	
	// release dbus message
	dbus_message_unref(msg);
	return true;
}

void DBusMonitor::start() {
	// set run
	m_bRun = true;
	
	// thread
	thread t([&](){		
		// loop
		while(m_bRun) {
			if (dbus_connection_read_write(m_pCon, 1000)) {
				auto msg = dbus_connection_pop_message(m_pCon);
				if (!msg) {
					continue;
				}
				
				// log
				cout << dbus_message_get_sender(msg) << endl;
				cout << dbus_message_get_member(msg) << endl;
				cout << dbus_message_get_interface(msg) << endl;
				cout << dbus_message_get_path(msg) << endl;
				cout << "----------------------------" << endl;
				
				// free the message
				dbus_message_unref(msg);
			}
		}
	});
	t.detach();
}

void DBusMonitor::stop() {
	// set run
	m_bRun = false;
}