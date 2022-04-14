#include <audio_reco.h>
#include <script.h>

AudioReco::AudioReco(AVInputFormat *pInFormat, const CmdCallBack pCmdCB) 
	: m_pInFormat(pInFormat),
	m_pAudio(nullptr),
	m_nAudSize(0),
	m_bRun(false),
	m_pCmdCB(pCmdCB) {
	// meset audio buffer
	memset(m_cAudBuffer, 0, RECO_BUF_MAX_SIZE);
}

AudioReco::~AudioReco() {	
	// stop
	stop();
	
	// release
	delete m_pAudio;
	m_pAudio = nullptr;
}

bool AudioReco::start(const string &name, const ParamsMap &params) {
	// create audio
	m_pAudio = new Audio(m_pInFormat, bind(&AudioReco::processFrame, this, placeholders::_1));
	
	// init audio
	if (!m_pAudio->init(name, params)) {
		return false;
	}

	// start audio
	m_pAudio->start();
	
	// set run 
	m_bRun = true;
	
	// start audio recognition tread
	thread t([&](){
		// loop
		while (m_bRun) {
			// recognition
			audioReco();
			
			// sleep
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	});
	t.detach();
	
	// return
	return true;
}

void AudioReco::stop() {
	// release
	if (m_pAudio) {
		// stop
		m_pAudio->stop();
	}
	
	// set run
	m_bRun = false;
}

void AudioReco::processFrame(AVPacket *packet) {
	// buffer full ignore
	if (m_nAudSize >= RECO_BUF_MAX_SIZE) {
		return;
	}
	
	// write
	memcpy(m_cAudBuffer + m_nAudSize, packet->data, packet->size);
	
	// update size
	m_nAudSize += packet->size;
}

void AudioReco::audioReco() {
	// check size
	if (m_nAudSize < RECO_BUF_MAX_SIZE) {
		return;
	}
	
	// init config 
	auto config = cmd_ln_init(nullptr, ps_args(), true,  
                 "-hmm", "/usr/share/pocketsphinx/model/en-us/en-us",  
                 "-lm", "/usr/share/pocketsphinx/model/en-us/en-us.lm.bin",  
                 "-dict", "/usr/share/pocketsphinx/model/en-us/cmudict-en-us.dict",
				 "-logfn", "/dev/null", 
				 nullptr);
	if (!config) {
		return;
	}
	
	// init pocketsphinx decoder
	auto pPsDecoder = ps_init(config);
	if (!pPsDecoder) {
		return;
	}
	
	// start
	ps_start_utt(pPsDecoder);
	// process
	ps_process_raw(pPsDecoder, (const int16*)m_cAudBuffer, RECO_BUF_MAX_SIZE / 2, false, false);
	// end
	ps_end_utt(pPsDecoder);
	
	// get result
	int32 score;
	auto pRet = ps_get_hyp(pPsDecoder, &score);
	
	// callback
	if (m_pCmdCB) {
		m_pCmdCB(pRet);
	}
	
	// reset audio size
	m_nAudSize = 0;
	
	// release pocketsphinx decoder
	ps_free(pPsDecoder);
}