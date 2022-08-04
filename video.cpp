#include <video.h>

Video::Video(AVCallBack pCB)
	: m_pInput(nullptr),
	m_pCTX(nullptr),
	m_pAVCB(pCB),
	m_bRun(false),
	m_nIndex(0),
	m_pPacket(av_packet_alloc()) {
	// av packet
    memset(m_pPacket, 0, sizeof(m_pPacket));
    av_init_packet(m_pPacket);
}

Video::~Video() {
	// stop
	stop();
	
	// release
	avformat_close_input(&m_pCTX);

	// release display sws 
	if (m_pDisImgSwsCTX) {
		sws_freeContext(m_pDisImgSwsCTX);
		m_pDisImgSwsCTX = nullptr;
	}
	
	// release detect sws 
	if (m_pDetImgSwsCTX) {
		sws_freeContext(m_pDetImgSwsCTX);
		m_pDetImgSwsCTX = nullptr;
	}
	
	// release av packet
	av_packet_free(&m_pPacket);
}

bool Video::init(const string &name, const ParamsMap &params) {
	// check device name 
	if(name.empty()) {
		return false;
	}

	// open video device
	m_pInput = av_find_input_format(VIDEO_DEVICE_NAME);
	if (!m_pInput) {		
		// log
		cout << "av_find_input_format " << VIDEO_DEVICE_NAME << " error" << endl;
		return false;
	}
	
	// options
    AVDictionary *pOption = nullptr;
    for (auto &iter : params) {
        av_dict_set(&pOption, iter.first.c_str(), iter.second.c_str(), 0);
    }
	
	// open
	auto nRet = avformat_open_input(&m_pCTX, name.c_str(), m_pInput, &pOption);
	if (nRet) {
        //error message
        char errorMsg[ERROR_MAX_SIZE] = { 0 };
        av_strerror(nRet, errorMsg, ERROR_MAX_SIZE);
		
		// log
		cout << "error " << errorMsg << endl;
        return false;
    }
	
	// get index
    m_nIndex = av_find_best_stream(m_pCTX, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	// get decoder
    auto pDeCodecCTX = m_pCTX->streams[m_nIndex]->codec;
    auto pCurCodec = avcodec_find_decoder(pDeCodecCTX->codec_id);
	
	// decoder delay
    av_opt_set(pDeCodecCTX->priv_data, "preset", "slow", 0);
    av_opt_set(pDeCodecCTX->priv_data, "tune", "zerolatency", 0);
    av_opt_set(pDeCodecCTX->priv_data, "profile", "baseline", 0);

    // open decoder
    if (avcodec_open2(pDeCodecCTX, pCurCodec, nullptr) < 0) {
        return false;
    }
	
	// check pix format
	if (pDeCodecCTX->pix_fmt == AV_PIX_FMT_NONE) {		
		// set pix format
		pDeCodecCTX->pix_fmt = AV_PIX_FMT_YUV420P;
	}
	
	// return result
	return true;
}

void Video::start() {
	// set run flag
	m_bRun = true;
	
	// capture thread
	thread t([&](){
		// loop
		while (m_bRun) {
			// capture frame
			captureFrame();
		}
	});
	t.detach();
}

void Video::stop() {
	// clear run flag
	m_bRun = false;
}

AVCodecContext* Video::getDecoder() const {
	return m_pCTX->streams[m_nIndex]->codec;
}

bool Video::setDisplaySwsContext(int nWidth, int nHeight, enum AVPixelFormat dstFormat) {
	// fill display mat
	m_cvDisImage = Mat(nWidth, nHeight, CV_8UC3);
	// fill cv lines
	m_cvDisLinesizes[0] = m_cvDisImage.step1();
	
	// decoder
	auto decoder = getDecoder();
	
	// init sws context
	m_pDisImgSwsCTX = sws_getContext(decoder->width, decoder->height, decoder->pix_fmt, 
								  nWidth, nHeight, dstFormat, 
								  SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
	
	// check display sws context
	if (!m_pDisImgSwsCTX) {
		cout << "display sws_getContext fail" << endl;
		return false;
	}
	return true;
}

bool Video::setDetectSwsContext(int nWidth, int nHeight, enum AVPixelFormat dstFormat) {
	// fill detect mat
	m_cvDetImage = Mat(nWidth, nHeight, CV_8UC3);
	// fill cv lines
	m_cvDetLinesizes[0] = m_cvDetImage.step1();
	
	// decoder
	auto decoder = getDecoder();
	
	// init sws context
	m_pDetImgSwsCTX = sws_getContext(decoder->width, decoder->height, decoder->pix_fmt, 
								  nWidth, nHeight, dstFormat, 
								  SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
	
	// check detect sws context
	if (!m_pDetImgSwsCTX) {
		cout << "detect sws_getContext fail" << endl;
		return false;
	}
	return true;
}

Mat* Video::resampleDisplay(AVFrame* pFrame) {
	// check display sws context 
	if (!m_pDisImgSwsCTX) {
		cout << "sws context is not ready" << endl;
		return nullptr;
	}
	
	// decoder
	auto decoder = getDecoder();
	// convert to mat
	sws_scale(m_pDisImgSwsCTX, pFrame->data, pFrame->linesize, 0, decoder->height, &m_cvDisImage.data, m_cvDisLinesizes);
	return &m_cvDisImage;
}

Mat* Video::resampleDetect(AVFrame* pFrame) {
	// check detect sws context 
	if (!m_pDetImgSwsCTX) {
		cout << "sws context is not ready" << endl;
		return nullptr;
	}
	
	// decoder
	auto decoder = getDecoder();
	// convert to mat
	sws_scale(m_pDetImgSwsCTX, pFrame->data, pFrame->linesize, 0, decoder->height, &m_cvDetImage.data, m_cvDetLinesizes);
	return &m_cvDetImage;
}

bool Video::captureFrame() {
	// reset packet
	m_pPacket->data = nullptr;
	m_pPacket->size = 0;
	
	// read frame
	if ((av_read_frame(m_pCTX, m_pPacket)) < 0) {
		// release ref
		av_packet_unref(m_pPacket);
		return false;
	}

	// process video stream only
	if (m_pPacket->stream_index != m_nIndex) {
		// release ref
		av_packet_unref(m_pPacket);
		return false;
	}
	
	// process mat
	if (m_pAVCB) {
		m_pAVCB(m_pPacket);
	}
	
	// release ref
	av_packet_unref(m_pPacket);
	
	// return 
	return true;
}
