#include <face_reco.h>

FaceReco::FaceReco(const string& strSPLandMark) 
	: m_pInFormat(nullptr),
	m_pVideo(nullptr) {
	// ffmpeg register
	avcodec_register_all();
    avdevice_register_all();
	
	// init frontal face detector
	m_pFaceDetector = get_frontal_face_detector();
	
	// init shaped predictor
	deserialize("shape_predictor_68_face_landmarks.dat") >> m_pShapePredictor;
}

FaceReco::~FaceReco() {
	// stop
	stop();
}

bool FaceReco::start(const string &name, const ParamsMap &params) {
	// open device
	auto pInFormat = av_find_input_format(DEVICE_NAME);
	if (!pInFormat) {		
		// log
		cout << "av_find_input_format " << DEVICE_NAME << " error" << endl;
		return false;
	}
	
	// create video
	m_pVideo = new Video(m_pInFormat, bind(&FaceReco::processFrameMat, this, placeholders::_1));
	
	// init video
	if (!m_pVideo->init(name, params)) {
		cout << "video init fail" << endl;
		return false;
	}
	
	// init last time stamp
	m_tLstTime = chrono::system_clock::now().time_since_epoch();
	
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
		
		// release
		delete m_pVideo;
		m_pVideo = nullptr;
	}
}

void FaceReco::processFrameMat(Mat &mat) {
	// check time stamp
	auto now = chrono::system_clock::now().time_since_epoch();
	if (now.count() - m_tLstTime.count() < 1000000000) {
		return;
	}
	m_tLstTime = now;
	
	// image 
	array2d<unsigned char> img;
	
	// gray mat
	Mat gray;
	cvtColor(mat, gray, COLOR_RGB2GRAY);
	dlib::assign_image(img, dlib::cv_image<unsigned char>(gray));
	
	// face vector
	std::vector<dlib::rectangle> dets = m_pFaceDetector(img);
	
	// each face 68 feature point
	std::vector<dlib::full_object_detection> shapes;
	for (auto i = 0; i < dets.size(); ++i) {
		shapes.push_back(m_pShapePredictor(img, dets[i])); 
	}
	
	// log number
	cout << "Number of faces detected: " << dets.size() << ", Number of faces 68 feature point: " << shapes.size() << endl;
}