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

int stereo_input_camera(unsigned char *frame_buffer)
{
	std::vector<unsigned char> vec_frame_buffer;

	vec_frame_buffer.insert(vec_frame_buffer.end(), frame_buffer, frame_buffer + FRAME_SIZE);

	// Capature video frames
	video_in >> frameCamera;

	// Covert each frame to .jpeg format
	cv::imencode(".jpeg", frameCamera, vec_frame_buffer, params);

	// Debug code to display received frame.
	// Note: display is from camera
	// TODO display the coverted JPEG frame
	cv::resizeWindow(strWinName, frameCamera.size());
	imshow(strWinName, frameCamera);

	return 0;

}
