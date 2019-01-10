#include "stdafx.h"

#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>      //for imshow
#include <vector>
#include <iostream>
#include <iomanip>

#include "stereo_parameters.h"

using namespace std;
using namespace cv;

static VideoCapture video_in;
static String strWinName = "Stereo_Input";
static std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, 95 };

static Mat frameCamera;
static Mat frameJpeg;


int stereo_input_init()
{
	Mat frame;
	int iVideoCapNum = 0;
	printf("In stereo_input_init\n");

	namedWindow(strWinName, WINDOW_NORMAL);
	
	video_in.open(iVideoCapNum);
	if (!video_in.isOpened()) {
		cerr << "Couldn't open video " << iVideoCapNum << endl;
		return 1;
	}

	return 0;
}

int stereo_input_deinit() {
	video_in.release();
	return 0;
}

int stereo_input_metadata(unsigned char *res_meta)
{

	return 0;
}

int stereo_input_camera(unsigned char *buf_frames)
{
	unsigned char *buf_frame1 = buf_frames;
	unsigned char *buf_frame2 = buf_frames + FRAME_SIZE;

	std::vector<unsigned char> vec_right_frame;
	std::vector<unsigned char> vec_left_frame;
	
	vec_right_frame.insert(vec_right_frame.end(), buf_frame1, buf_frame1 + FRAME_SIZE);
	vec_right_frame.insert(vec_right_frame.end(), buf_frame2, buf_frame2 + FRAME_SIZE);

	// Capature video frames
	video_in >> frameCamera;

	// Covert each frame to .jpeg format
//	cv::imencode(".jpeg", frameCamera, vec_frame_buffer, params);

	// Debug code to display received frame.
	// Note: display is from camera
	// TODO display the coverted JPEG frame
//	cv::resizeWindow(strWinName, frameCamera.size());

	imshow(strWinName, frameCamera);

	return 0;

}
