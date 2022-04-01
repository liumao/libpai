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

/// \file led.h
/// \brief led header
///
/// Control led
///
/// \author liumao
/// \version v1.0
/// \date 30 Mar 2022
 
#ifndef __PAI_LED_H__
#define __PAI_LED_H__

#include <pai.h>

class Led {
private:
	/// \brief pin no
	int m_nPinNo;

public:
	/// \brief constructor
	Led(int nPinNo);
	
	/// \brief destructor
	virtual ~Led();
	
	/// \brief turn on led
	/// 
	/// \param [in] nKeepMsec turn on led for times
	///
	/// \return 
	void turnOn(int nKeepMsec);
	
	/// \brief turn off led
	/// 
	/// \param [in] nKeepMsec turn on led for times
	///
	/// \return 
	void turnOff(int nKeepMsec);
};

#endif //__PAI_LED_H__