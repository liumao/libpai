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

/// \file audio.h
/// \brief audio header
///
/// Capture audio from device.
///
/// \author liumao
/// \version v1.0
/// \date 3 May 2022

#ifndef __PAI_AUDIO_H__
#define __PAI_AUDIO_H__

#include <pai.h>

class Audio {
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
	
public:
	/// \brief constructor
	Audio(AVInputFormat *pAVInput, AVCallBack pCB);
	
	/// \brief destructor
	virtual ~Audio();
	
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
	
	/// \brief get decoder
	///
	/// \return codec
	AVCodecContext* getDecoder() const;
	
private:
	/// \brief capture frame
	///
	/// \return true/false
	bool captureFrame();
	
};

#endif  //__PAI_AUDIO_H__