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

/// \file audio_reco.h
/// \brief audio reco header
///
/// Capture audio reco from device.
///
/// \author liumao
/// \version v1.0
/// \date 3 May 2022

#ifndef __PAI_AUDIO_RECO_H__
#define __PAI_AUDIO_RECO_H__

#include <pai.h>
#include <audio.h>

/// \brief recognition buf max size
#define RECO_BUF_MAX_SIZE 51200
/// \brief recognition size
#define RECO_SIZE 512

class AudioReco {
private:
	/// \brief av input format
    AVInputFormat *m_pInFormat;
	
	/// \brief audio
	Audio *m_pAudio;
	
	/// \brief audio buffer size
	int m_nAudSize;
	
	/// \brief pocket sphinx decoder
	ps_decoder_t *m_pPsDecoder; 
	
	/// \brief audio buffer
	char m_cAudBuffer[RECO_BUF_MAX_SIZE];
	
	/// \brief run
	bool m_bRun;
	
	/// \brief audio cmd callback
	CmdCallBack m_pCmdCB;
	
public:
	/// \brief constructor
	///
	/// \param [in] pInFormat av input format
	/// \param [in] strSPModel pocket sphinx model
	/// \param [in] strSPBin pocket sphinx model bin
	/// \param [in] strSPDict pocket sphinx model dict
	/// \param [in] pCmdCB audio cmd callback
	AudioReco(AVInputFormat *pInFormat, const string& strSPModel, const string& strSPBin, const string& strSPDict, const CmdCallBack pCmdCB);
	
	/// \brief destructor
	virtual ~AudioReco();
	
	/// \brief start audio recognition
	///
	/// \param [in] name audio name
	/// \param [in] params params
	///
	/// \return true/false
	bool start(const string &name, const ParamsMap &params);
	
	/// \brief stop face recognition
	void stop();
	
private:
	/// \brief process audio packet
	///
	/// \param [in] packet audio packet
	void processFrame(AVPacket *packet);
	
	/// \brief audio recognition
	void audioReco();
	
};

#endif //__PAI_AUDIO_RECO_H__