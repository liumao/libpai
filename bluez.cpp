#include <bluez.h>
#include <script.h>

BlueZ::BlueZ(const BulezCallBack pBluezCB)
	: m_nDeviceId(-1),
	m_pInquiryInfo(nullptr),
	m_nSock(-1),
	m_bRun(false),
	m_nServerId(0),
	m_nClientId(0),
	m_pBluezCB(pBluezCB) {	
	// init device id
	m_nDeviceId = hci_get_route(nullptr);
	
	// init device sock
	m_nSock = hci_open_dev(m_nDeviceId);
	
	// init inquiry info
	m_pInquiryInfo = new inquiry_info[sizeof(inquiry_info) * RES_MAX_SIZE];
}

BlueZ::~BlueZ() {
	// release
	if (m_pInquiryInfo) {
		delete[] m_pInquiryInfo;
		m_pInquiryInfo = nullptr;
	}
	
	// close sock
	if (m_nSock) {
		hci_close_dev(m_nSock);
		close(m_nSock);
		m_nSock = -1;
	}
	
	// stop server
	stopServer();
}

void BlueZ::scan(std::vector<TNodeInfo> &vDevices) {
	// inquiry
	auto nSize = hci_inquiry(m_nDeviceId, DEVICE_SIZE, RES_MAX_SIZE, nullptr, &m_pInquiryInfo, IREQ_CACHE_FLUSH);
	
	// check
	if (nSize < 0) {
		cout << "hci_inquiry fail " << strerror(errno) << endl;
		return;
	}
	
	// loop
	for (auto i = 0; i < nSize; i++) {
		// node info
		TNodeInfo tNodeInfo;
		
		// update node 
        ba2str(&(m_pInquiryInfo + i)->bdaddr, tNodeInfo.m_cMac);
        memset(tNodeInfo.m_cName, 0, sizeof(tNodeInfo.m_cName));
        if (hci_read_remote_name(m_nSock, &(m_pInquiryInfo + i)->bdaddr, sizeof(tNodeInfo.m_cName), tNodeInfo.m_cName, 0) < 0) {
			strcpy(tNodeInfo.m_cName, "[unknown]");
		}
		
		// add node
		vDevices.push_back(tNodeInfo);
    }
}

bool BlueZ::pair(const string& strDeviceName) {
	// connection handle
	short unsigned int nHandle = 0;
	
	// type
	unsigned int nType = HCI_DM1 | HCI_DM3 | HCI_DM5 | HCI_DH1 | HCI_DH3 | HCI_DH5;
	
	// bulez addr
	bdaddr_t bAddr;
	str2ba(strDeviceName.c_str(), &bAddr);
	
	// create connection
	if (hci_create_connection(m_nSock, &bAddr, htobs(nType), 0, 0, &nHandle, 0) < 0) {
		cout << "create connection fail " << strerror(errno) << endl;
		return false;
	}
	
	// auth link
	if (hci_authenticate_link(m_nSock, nHandle, 0) < 0) {
		cout << "authenticate fail " << strerror(errno) << endl;
		return false;
	}
	
	// encrypt link
	if (hci_encrypt_link(m_nSock, nHandle, 1, 0) < 0) {
		cout << "encrypt fail " << strerror(errno) << endl;
		return false;
	}
	
	// return
	return true;
}

bool BlueZ::startServer(int nPort) {
	// make bluetooth enable
	Script().executeCMD("bluetoothctl power on");
	Script().executeCMD("bluetoothctl discoverable on");
	
	// set run
	m_bRun = true;
	
	// socket
	m_nServerId = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	
	// non-block
	auto nFlags = fcntl(m_nServerId, F_GETFL, 0);
	if (nFlags < 0) {
		cout << "SetNonBlock failure" << endl;
		return false;
	}

	auto nNewFlag = (nFlags | O_NONBLOCK);
	auto nRet = fcntl(m_nServerId, F_SETFL, nNewFlag);
	if (nRet < 0) {
		cout << "SetNonBlock failure" << endl;
		return false;
	}
	
	// bind socket to port of the first available 
    // bluetooth adapter
	struct sockaddr_rc loc_addr = { 0 };
	loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = {{0, 0, 0, 0, 0, 0}};
    loc_addr.rc_channel = (uint8_t) nPort;
	if (bind(m_nServerId, (struct sockaddr *)&loc_addr, sizeof(loc_addr)) < 0) {
		cout << "bind fail" << endl;
		return false;
	}
	
	// put socket into listening mode
    if (listen(m_nServerId, 1) < 0) {
		cout << "listen fail" << endl;
		return false;
	}
	
	// read data from the client
	char buf[BUF_SIZE] = { 0 };
	thread t([&]() {
		while (m_bRun) {
			// accept one connection
			struct sockaddr_rc rem_addr = { 0 };
			auto opt = sizeof(rem_addr);
			m_nClientId = accept(m_nServerId, (struct sockaddr *)&rem_addr, &opt);
			// check
			if (m_nClientId == INVALID_SOCKET) {
				// sleep
				delay(1000);
				continue;
			}
			cout << "new connect " << m_nClientId << endl;
		
			while (m_bRun) {
				// reset buf
				memset(buf, 0, BUF_SIZE);
				// read data from the client
				auto nRead = read(m_nClientId, buf, sizeof(buf));
				// check read size
				if (nRead <= 0) {
					cout << m_nClientId << " disconnect" << endl;
					m_nClientId = 0;
					break;
				} else if (m_pBluezCB) {
					m_pBluezCB(buf, true);
				}
			}
		}
	});
	t.detach();
	return true;
}

void BlueZ::stopServer() {
	// make bluetooth disable
	Script().executeCMD("bluetoothctl discoverable off");
	
	// set run
	m_bRun = false;
	
	// close socket
	if (m_nServerId > 1) {
		close(m_nServerId);
		m_nServerId = -1;
	}
	if (m_nClientId > 1) {
		close(m_nClientId);
		m_nClientId = -1;
	}
}

bool BlueZ::createClient(const string& strDeviceId, int nPort) {
	// socket
	m_nClientId = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	
	// set the connection parameters (who to connect to)
	struct sockaddr_rc loc_addr = { 0 };
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_channel = (uint8_t) nPort;
    str2ba(strDeviceId.c_str(), &loc_addr.rc_bdaddr);
	
	// connect to server
    auto nRet = connect(m_nClientId, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
	
	// check ret
	if (nRet) {
		cout << "connect remote fail" << endl;
		return false;
	}
	
	// read data from the client
	char buf[1024] = { 0 };
	thread t([&]() {
		while (m_bRun) {
			// read data from the client
			auto nRead = read(m_nClientId, buf, sizeof(buf));
			if(nRead > 0 && m_pBluezCB) {
				m_pBluezCB(buf, false);
			}
		}
	});
	t.detach();
	return true;
}

void BlueZ::sendMsg(const string& strMsg) {
	auto nSent = write(m_nClientId, strMsg.c_str(), strMsg.length());
	if (nSent < 0) {
		cout << "send message fail" << endl;
	}
}

void BlueZ::deleteClient() {
	// close client
    close(m_nClientId);	
}

int BlueZ::getClientId() {
	return m_nClientId;
}