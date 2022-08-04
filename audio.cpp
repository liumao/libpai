#include <audio.h>

Audio::Audio(const AVCallBack pCB)
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

Audio::~Audio() {
	// stop
	stop();
	
	// release
	avformat_close_input(&m_pCTX);
	
	// release av packet
	av_packet_free(&m_pPacket);
}

bool Audio::init(const string &name, const ParamsMap &params) {
	// check device name 
	if(name.empty()) {
		return false;
	}
	
	// open audio device
	m_pInput = av_find_input_format(AUDIO_DEVICE_NAME);
	if (!m_pInput) {		
		// log
		cout << "av_find_input_format " << AUDIO_DEVICE_NAME << " error" << endl;
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
    m_nIndex = av_find_best_stream(m_pCTX, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	// get decoder
    auto pDeCodecCTX = m_pCTX->streams[m_nIndex]->codec;
    auto pCurCodec = avcodec_find_decoder(pDeCodecCTX->codec_id);

    // open decoder
    if (avcodec_open2(pDeCodecCTX, pCurCodec, nullptr) < 0) {
        return false;
    }
	
	// return result
	return true;
}

void Audio::start() {
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

void Audio::stop() {
	// clear run flag
	m_bRun = false;
}

AVCodecContext* Audio::getDecoder() const {
	return m_pCTX->streams[m_nIndex]->codec;
}

bool Audio::captureFrame() {
	// reset packet
	m_pPacket->data = nullptr;
	m_pPacket->size = 0;
	
	// read frame
	if ((av_read_frame(m_pCTX, m_pPacket)) < 0) {
		// release ref
		av_packet_unref(m_pPacket);
		return false;
	}

	// process audio stream only
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