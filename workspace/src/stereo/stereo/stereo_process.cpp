#include "stdafx.h"

//#include <opencv2/imgproc.hpp>
//#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>      //for imshow

#include "stereo_input.h"
#include "stereo_parameters.h"

using namespace std;
using namespace cv;

// MACROS

// FUNCTIONS
int stereo_process_input(struct stereo_object *ptr_stereo_object)
{
	std::vector<uchar> jpeg_buffer;
	static std::vector<int> params = { cv::IMWRITE_JPEG_QUALITY, STEREO_QUALITY_VALUE };

	Mat gray_image_left(
		Size(ptr_stereo_object->meta_pkt.frame_width, ptr_stereo_object->meta_pkt.frame_height),
		CV_8UC1);
	
	// provide the buffer address
	gray_image_left.data = ptr_stereo_object->ptr_frame_left;

	// Covert each frame to .jpeg format
	imencode(".jpeg", gray_image_left, jpeg_buffer, params);

	memcpy(ptr_stereo_object->ptr_jpeg_stream, reinterpret_cast<char*>(jpeg_buffer.data()),
		jpeg_buffer.size());

#ifdef DEBUG_STEREO_JPEG
	//Create a Size(1, nSize) Mat object of 8 - bit, single - byte elements
	Mat decodedImage;

	Mat rawData(1, jpeg_buffer.size(), CV_8UC1, (void*)ptr_stereo_object->ptr_jpeg_stream);

	decodedImage = imdecode(rawData, false);
	if (decodedImage.data == NULL) 	{
		// Error reading raw image data
		printf("Error: Could not decode\n");
		while (cv::waitKey(1) < 1);
	} else {
		imshow(WINDOW_STEREO_JPEG, decodedImage);
		int bytes1 = rawData.total() * rawData.elemSize();
		int bytes2 = decodedImage.total() * decodedImage.elemSize();
		//printf("Decode: frame size: 0x%x, jpeg frame size: 0x%x\n", bytes1, bytes2);
	}
	// https://stackoverflow.com/questions/14727267/opencv-read-jpeg-image-from-buffer
#endif //DEBUG_STEREO_JPEG	
	return 0;
}

int stereo_process_request(struct stereo_data *stereo_packet)
{

	return 0;
}
