#include "stdafx.h"

#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>      //for imshow
#include <vector>
#include <iostream>
#include <iomanip>

#include "stereo_input.h"
#include "stereo_parameters.h"

using namespace std;
using namespace cv;

static VideoCapture video_in_left;
static VideoCapture video_in_right;

// Function defination
void debug_print_stereo_object(struct stereo_object *ptr_stereo_object)
{
	printf("Stereo: H: %d\n", ptr_stereo_object->meta_pkt.frame_height);
	printf("Stereo: W: %d\n", ptr_stereo_object->meta_pkt.frame_width);
	printf("Stereo: ptrL: 0x%x\n", ptr_stereo_object->ptr_frame_left);

}

void debug_mat(Mat &mat_obj, const char *mat_name)
{
	printf("Mat %s: H %d\n", mat_name, mat_obj.rows);
	printf("Mat %s: W %d\n", mat_name, mat_obj.cols);
	printf("Mat %s: T %d\n", mat_name, mat_obj.type());
	printf("Mat %s: D 0x%x\n", mat_name, mat_obj.data);
}


int stereo_input_metadata(struct stereo_object *ptr_stereo_object)
{

	return 0;
}

int stereo_input_camera(struct stereo_object *ptr_stereo_object)
{
	Mat cam_frame;

	Mat gray_image_left(
		Size(ptr_stereo_object->meta_pkt.frame_width, ptr_stereo_object->meta_pkt.frame_height),
		CV_8UC1);

	gray_image_left.data = ptr_stereo_object->ptr_frame_left;

	// Capature video frames
	video_in_left >> cam_frame;

	// Covert to gray scale
	cv::cvtColor(cam_frame, gray_image_left, CV_BGR2GRAY);

#ifdef DEBUG_STEREO_INPUT
	// Debug: Show gray scale
	imshow(WINDOW_STEREO_INPUT, gray_image_left);
	
	// Debug: Print the gray scale matrix details
	debug_mat(gray_image_left, "Gray_input");
#endif // DEBUG_STEREO_INPUT

	return 0;

}

int stereo_input_deinit(struct stereo_object *ptr_stereo_object) 
{
	printf("In stereo_input_deinit\n");

	video_in_left.release();
	
	free(ptr_stereo_object->ptr_frame_left);
	free(ptr_stereo_object->ptr_frame_right);
	free(ptr_stereo_object->ptr_jpeg_stream);
	
	return 0;
}

int stereo_input_init(struct stereo_object *ptr_stereo_object)
{
	Mat frame;
	int frame_size;

	int iVideoCapNum = 0;
	printf("In stereo_input_init\n");

	// Open the left side video input
	video_in_left.open(iVideoCapNum);
	if (!video_in_left.isOpened()) {
		cerr << "Couldn't open video " << iVideoCapNum << endl;
		return 1;
	}

	video_in_left >> frame;

	// both camera has same height, width
	ptr_stereo_object->meta_pkt.frame_height = frame.rows;
	ptr_stereo_object->meta_pkt.frame_width = frame.cols;
	ptr_stereo_object->frame_size = frame.cols * frame.rows;

	printf("In stereo_input_init H:%d W:%d\n",
		ptr_stereo_object->meta_pkt.frame_height,
	    ptr_stereo_object->meta_pkt.frame_width
		);

	frame_size = frame.rows * frame.cols;

	// allocate memory to store the image raw and jpeg bytes
	ptr_stereo_object->ptr_frame_left =
		(unsigned char *) malloc(frame_size);

	ptr_stereo_object->ptr_frame_right =
		(unsigned char *)malloc(frame_size);

	ptr_stereo_object->ptr_jpeg_stream =
		(unsigned char *)malloc(frame_size * 2);

	return 0;
}
