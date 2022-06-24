/*
Copyright (C) 2004  loveyou12300liumao@163.com
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/// \file bluez.h
/// \brief bluez header
///
/// Control bluetooth
///
/// \author liumao
/// \version v1.0
/// \date 8 May 2022
 
#ifndef __PAI_BLUEZ_H__
#define __PAI_BLUEZ_H__

#include <pai.h>

/// \brief device size
#define DEVICE_SIZE 8
/// \brief max response
#define RES_MAX_SIZE 255

class BlueZ {
private:
	/// \brief device id
	int m_nDeviceId;
	
	/// \brief inquiry info
	inquiry_info *m_pInquiryInfo;
	
	/// \brief sock
	int m_nSock;
	
	/// \brief run flag
	bool m_bRun;
	
	/// \brief server id
	int m_nServerId;
	/// \brief client id
	int m_nClientId;
	
	/// \brief bluez callback
	BulezCallBack m_pBluezCB;
	
public:
	/// \brief constructor
	///
	/// \param [in] pBluezCB bluez callback
	BlueZ(const BulezCallBack pBluezCB);
	
	/// \brief destructor
	virtual ~BlueZ();
	
	/// \brief scan
	///
	/// \param [in] vDevices available devices
	///
	/// \return 
	void scan(std::vector<TNodeInfo> &vDevices);
	
	/// \brief pair
	///
	/// \param [in] device_name device name
	///
	/// \return true/false
	bool pair(const string& strDeviceName);
	
	/// \brief start server
	///
	/// \param [in] nPort port
	///
	/// \return true/false
	bool startServer(int nPort);
	
	/// \brief stop server
	///
	/// \return 
	void stopServer();
	
	/// \brief create client
	///
	/// \param [in] strDeviceId blue tooth id
	/// \param [in] nPort port
	///
	/// return true/false
	bool createClient(const string& strDeviceId, int nPort);
	
	/// \brief send message
	///
	/// \param [in] strMsg
	void sendMsg(const string& strMsg);
	
	/// \brief delete client
	///
	/// \return 
	void deleteClient();
	
	/// \brief get client id
	///
	/// \return client id
	int getClientId();

};

#endif //__PAI_BLUEZ_H__