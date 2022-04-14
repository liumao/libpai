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

/// \file face_reco.h
/// \brief face recognition header
///
/// Face recognition
///
/// \author liumao
/// \version v1.0
/// \date 7 May 2022
 
#ifndef __PAI_FACE_RECO_H__
#define __PAI_FACE_RECO_H__

#include <pai.h>
#include <video.h>

/// \brief line size
#define CV_LINE_SIZE 1

class FaceReco {
private:
	/// \brief last time stamp
	time_t m_tLstTime;
	
	/// \brief av input format
    AVInputFormat *m_pInFormat;
	
	/// \brief video
	Video *m_pVideo;
	
	/// \brief frontal face detector
	frontal_face_detector m_pFaceDetector;
	
	/// \brief shape predictor
	shape_predictor m_pShapePredictor;
	
	/// \brief shape location callback
	LocCallBack m_pLocCB;
	
	/// \brief cv line size
	int m_cvLinesizes[CV_LINE_SIZE];
	
	/// \brief opencv mat
	Mat m_cvImage;
	
	/// \brief image context
    struct SwsContext *m_pImgSwsCTX;
	
	/// \brief av frame
	AVFrame *m_pFrame;
	
public:
	/// \brief constructor
	///
	/// \param [in] pInFormat av input format
	/// \param [in] strSPLandMark shape predictor land mark
	/// \param [in] pLocCB shape location callback
	FaceReco(AVInputFormat *pInFormat, const string& strSPLandMark, const LocCallBack pLocCB);
	
	/// \brief destructor
	virtual ~FaceReco();
	
	/// \brief start face recognition
	///
	/// \param [in] name video name
	/// \param [in] params params
	///
	/// \return true/false
	bool start(const string &name, const ParamsMap &params);
	
	/// \brief stop face recognition
	void stop();
	
private:
	/// \brief process video packet
	///
	/// \param [in] packet video packet
	void processFrame(AVPacket *packet);

};

#endif //__PAI_FACE_RECO_H__