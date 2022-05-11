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

/// \file dbus_monitor.h
/// \brief dbus monitor header
///
/// Monitor dbus message
///
/// \author liumao
/// \version v1.0
/// \date 15 May 2022

#ifndef __PAI_DBUS_MONITOR_H__
#define __PAI_DBUS_MONITOR_H__

#include <pai.h>

class DBusMonitor {
private:
	/// \brief dbus error
	DBusError m_tDBusError;
	
	/// \brief con
	DBusConnection *m_pCon;

	/// \brief run flag
	bool m_bRun;
	
public:
	/// \brief constructor
	DBusMonitor();
	
	/// \brief destructor
	virtual ~DBusMonitor();
	
	/// \brief add monitor method
	///
	/// \param [in] strMethod method
	///
	/// \return true/false
	bool addMonitorMethod(const string& strMethod);
	
	/// \brief start monitor
	///
	/// \return
	void start();
	
	/// \brief stop monitor
	///
	/// \return
	void stop();
	
};

#endif  //__PAI_DBUS_MONITOR_H__