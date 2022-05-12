#include <face_reco.h>

FaceReco::FaceReco(AVInputFormat *pInFormat, const string& strSPLandMark, const LocCallBack pLocCB) 
	: m_pInFormat(pInFormat),
	m_pVideo(nullptr),
	m_pLocCB(pLocCB),
    m_pImgSwsCTX(nullptr),
	m_pFrame(av_frame_alloc()) {
	// clear cv line
	memset(m_cvLinesizes, 0, CV_LINE_SIZE);
	
	// init frontal face detector
	m_pFaceDetector = get_frontal_face_detector();
	
	// init shaped predictor
	deserialize(strSPLandMark.c_str()) >> m_pShapePredictor;
}

FaceReco::~FaceReco() {
	// stop
	stop();
	
	// release
	delete m_pVideo;
	m_pVideo = nullptr;
	
	// release sws 
	sws_freeContext(m_pImgSwsCTX);
		
	// release av frame
	av_frame_free(&m_pFrame);
}

bool FaceReco::start(const string &name, const ParamsMap &params) {
	// create video
	m_pVideo = new Video(m_pInFormat, bind(&FaceReco::processFrame, this, placeholders::_1));
	
	// init video
	if (!m_pVideo->init(name, params)) {
		return false;
	}
	
	// decoder
	auto decoder = m_pVideo->getDecoder();
	
	// init cv mat
	m_cvImage = Mat(decoder->width, decoder->height, CV_8UC3);
	m_cvLinesizes[0] = m_cvImage.step1();

	// init sws context
	m_pImgSwsCTX = sws_getContext(decoder->width, decoder->height, decoder->pix_fmt, 
								  decoder->width, decoder->height, AVPixelFormat::AV_PIX_FMT_BGR24, 
								  SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
								  
	// check sws context
	if (!m_pImgSwsCTX) {
		cout << "sws_getContext fail" << endl;
		return false;
	}
	
	// init last time stamp
	m_tLstTime = getCurTimeStamp();
	
	// start video
	m_pVideo->start();
	
	// return
	return true;
}

void FaceReco::stop() {
	// release
	if (m_pVideo) {
		// stop
		m_pVideo->stop();
	}
}

void FaceReco::processFrame(AVPacket *packet) {
	// check time stamp
	auto now = getCurTimeStamp();
	if (now - m_tLstTime < 1000) {
		return;
	}
	m_tLstTime = now;
	
	// decoder
	auto decoder = m_pVideo->getDecoder();
	
	// got
	int nGot;
	if (avcodec_decode_video2(decoder, m_pFrame, &nGot, packet) < 0 || !nGot) {
		return;
	}
	
	// convert to mat
	sws_scale(m_pImgSwsCTX, m_pFrame->data, m_pFrame->linesize, 0, decoder->height, &m_cvImage.data, m_cvLinesizes);
		
	// image 
	array2d<unsigned char> img;
	
	// gray mat
	Mat gray;
	cvtColor(m_cvImage, gray, COLOR_RGB2GRAY);
	dlib::assign_image(img, dlib::cv_image<unsigned char>(gray));
	
	// face vector
	std::vector<dlib::rectangle> dets = m_pFaceDetector(img);
	
	// check face number
	if (dets.size() != 1) {
		return;
	}
	
	// face location
	TFaceLoc tFaceLoc = {dets[0].left(), dets[0].right(), dets[0].top(), dets[0].bottom()};
	
	// notify
	if (m_pLocCB) {
		m_pLocCB(tFaceLoc);
	}
}