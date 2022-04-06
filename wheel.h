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

/// \file wheel.h
/// \brief wheel header
///
/// Control wheel
///
/// \author liumao
/// \version v1.0
/// \date 30 Mar 2022
 
#ifndef __PAI_WHEEL_H__
#define __PAI_WHEEL_H__

#include <pai.h>

class Wheel {
private:
	/// \brief pin 0
	int m_nPin0;
	/// \brief pin 1
	int m_nPin1;
	
public:
	/// \brief constructor
	Wheel(int nPin0, int nPin1);
	
	/// \brief destructor
	virtual ~Wheel();
	
	/// \brief forward
	/// 
	/// \param [in] nSpeed speed
	/// \param [in] nKeepMsec for times
	///
	/// \return 
	void forward(int nSpeed, int nKeepMsec);
	
	/// \brief back
	///
	/// \param [in] nSpeed speed
	/// \param [in] nKeepMsec for times
	///
	/// \return 
	void back(int nSpeed, int nKeepMsec);
	
	/// \brief pause
	///
	/// \return
	void pause();

};

#endif //__PAI_WHEEL_H__

