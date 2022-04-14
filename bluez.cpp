#include <bluez.h>

BlueZ::BlueZ()
	: m_nDeviceId(-1),
	m_pInquiryInfo(nullptr),
	m_nSock(-1) {
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
		delete m_pInquiryInfo;
		m_pInquiryInfo = nullptr;
	}
	
	// close sock
	if (m_nSock) {
		hci_close_dev(m_nSock);
		close(m_nSock);
		m_nSock = -1;
	}
}

void BlueZ::scan(vector<TNodeInfo> &vDevices) {
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