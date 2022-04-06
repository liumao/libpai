#include <video.h>

Video::Video(AVInputFormat *pAVInput, AVCallBack pCB)
	: m_pInput(pAVInput),
	m_pCTX(nullptr),
	m_pAVCB(pCB),
	m_bRun(false),
	m_nIndex(0),
	m_pPacket(av_packet_alloc()),
	m_pFrame(av_frame_alloc()),
    m_pImgSwsCTX(nullptr) {
	// av packet
    memset(m_pPacket, 0, sizeof(m_pPacket));
    av_init_packet(m_pPacket);
	
	// clear cv line
	memset(m_cvLinesizes, 0, CV_LINE_SIZE);
}

Video::~Video() {
	// stop
	stop();
	
	// release      
	avformat_close_input(&m_pCTX);
	
	// release av packet
	av_packet_free(&m_pPacket); 	
	
	// release av frame
	av_frame_free(&m_pFrame);
	
	// release sws 
	sws_freeContext(m_pImgSwsCTX);
}

bool Video::init(const string &name, const ParamsMap &params) {
	// check device name 
	if(name.empty()) {
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
        return false;
    }	
	
	// get index
    m_nIndex = av_find_best_stream(m_pCTX, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
	//get decoder
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
	
	// init cv mat
	m_cvImage = Mat(pDeCodecCTX->width, pDeCodecCTX->height, CV_8UC3);
	m_cvLinesizes[0] = m_cvImage.step1();
	
	// init sws context
	m_pImgSwsCTX = sws_getContext(pDeCodecCTX->width, pDeCodecCTX->height, pDeCodecCTX->pix_fmt, 
								  pDeCodecCTX->width, pDeCodecCTX->height, AVPixelFormat::AV_PIX_FMT_BGR24, 
								  SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);
								  
	// check sws context
	if (!m_pImgSwsCTX) {
		cout << "sws_getContext fail" << endl;
		return false;
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
			// reset packet
			m_pPacket->data = nullptr;
			m_pPacket->size = 0;
			
			// read frame
			if ((av_read_frame(m_pCTX, m_pPacket)) < 0) {
				// release ref
				av_packet_unref(m_pPacket);
				continue;
			}

			// process video stream only
			if (m_pPacket->stream_index != m_nIndex) {
				// release ref
				av_packet_unref(m_pPacket);
				continue;
			}
			
			// got
			int nGot;

			// decode
			auto pCodecCTX = m_pCTX->streams[m_nIndex]->codec;
			if (avcodec_decode_video2(pCodecCTX, m_pFrame, &nGot, m_pPacket) < 0 || !nGot) {
				continue;
			}
			
			// convert to mat
			sws_scale(m_pImgSwsCTX, m_pFrame->data, m_pFrame->linesize, 0, pCodecCTX->height, &m_cvImage.data, m_cvLinesizes);
			
			// process mat
			if (m_pAVCB) {
				m_pAVCB(m_cvImage);
			}
			
			// release ref
			av_packet_unref(m_pPacket);
		}
	});
	t.detach();
}

void Video::stop() {
	// clear run flag
	m_bRun = false;
}
