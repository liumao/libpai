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

/// \file video.h
/// \brief video header
///
/// Capture video from device.
///
/// \author liumao
/// \version v1.0
/// \date 1 Apr 2022

#ifndef __PAI_VIDEO_H__
#define __PAI_VIDEO_H__

#include <pai.h>

/// \brief error max size
#define ERROR_MAX_SIZE 1024
/// \brief line size
#define CV_LINE_SIZE 1

class Video {
private:
	/// \brief av input format
	AVInputFormat *m_pInput;
	
	/// \brief context
    AVFormatContext *m_pCTX;
	
	/// \brief callback
    AVCallBack m_pAVCB;
	
	/// \brief start flag
    bool m_bRun;
	
    /// \brief index
    int m_nIndex;
	
	/// \brief av packet
	AVPacket *m_pPacket;
	
	/// \brief cv line size
	int m_cvLinesizes[CV_LINE_SIZE];
	
	/// \brief opencv mat
	Mat m_cvImage;
	
	/// \brief av frame
	AVFrame *m_pFrame;
	
	/// \brief image context
    struct SwsContext *m_pImgSwsCTX;
	
public:
	/// \brief constructor
	Video(AVInputFormat *pAVInput, AVCallBack pCB);
	
	/// \brief destructor
	virtual ~Video();
	
	/// \brief init
	///
	/// \param [in] name device name
	/// \param [in] params params
	///
	/// \return true/false
	bool init(const string &name, const ParamsMap &params);
	
	/// \brief start
	///
	/// \return 
	void start();
	
	/// \brief stop
	///
	/// \return 
	void stop();
	
};

#endif //__PAI_VIDEO_H__