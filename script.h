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

/// \file script.h
/// \brief script header
///
/// Execute script from file or cmd
///
/// \author liumao
/// \version v1.0
/// \date 30 Mar 2022

#ifndef __PAI_SCRIPT_H__
#define __PAI_SCRIPT_H__

#include <pai.h>

/// \brief max script return size
#define MAX_SCRIPT_RES_SIZE 512

/// \brief result of cmd
#define CMD_RES "; echo $?"

class Script {
private:
	/// \brief file handler
	FILE* m_pHandler;
	
	/// \brief cmd result buffer
	char m_cCmdRes[MAX_SCRIPT_RES_SIZE];
	
public:
	/// \brief constructor
	Script();
	
	/// \brief destructor
	virtual ~Script();
	
	/// \brief excute cmd
	///
	/// \param [in] pCmd script
	///
	/// \return script result
	const char* executeCMD(const char* pCmd);
};

#endif //__PAI_SCRIPT_H__