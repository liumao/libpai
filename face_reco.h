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

class FaceReco {
private:
	/// \brief last time stamp
	chrono::system_clock::duration m_tLstTime;
	
	/// \brief av input format
    AVInputFormat *m_pInFormat;
	
	/// \brief video
	Video *m_pVideo;
	
	/// \brief frontal face detector
	frontal_face_detector m_pFaceDetector;
	
	/// \brief shape predictor
	shape_predictor m_pShapePredictor;
	
public:
	/// \brief constructor
	///
	/// \param [in] strSPLandMark shape predictor land mark
	FaceReco(const string& strSPLandMark);
	
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
	/// \brief deal with frame mat
	///
	/// \param [in] mat frame mat
	void processFrameMat(Mat &mat);

};

#endif //__PAI_FACE_RECO_H__