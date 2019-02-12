#include "stdafx.h"

#include <opencv2/highgui.hpp> // for waitkey()

#include <windows.h>

// Threads: to run streaming server
#include <process.h>

#include "stereo_input.h"
#include "stereo_process.h"
#include "stereo_output.h"
#include "stereo_parameters.h"
#include "localize_input.h"

// MACROS


// Namespace
using namespace std;
using namespace cv;

// Thread, scheduler
static HANDLE h_stereo_scheduler;

void stereo_open_display_windows()
{
#ifdef DEBUG_STEREO_INPUT
	namedWindow(WINDOW_STEREO_INPUT, WINDOW_NORMAL);
#endif // DEBUG_STEREO_INPUT

#ifdef DEBUG_STEREO_OUTPUT
	namedWindow(WINDOW_STEREO_OUTPUT, WINDOW_NORMAL);
#endif // DEBUG_STEREO_OUTPUT

#ifdef DEBUG_STEREO_JPEG
	namedWindow(WINDOW_STEREO_JPEG, WINDOW_NORMAL);
#endif // DEBUG_STEREO_JPEG

#ifdef DEBUG_STEREO_DEBUG
	namedWindow(WINDOW_STEREO_DEBUG, WINDOW_NORMAL);
#endif // DEBUG_STEREO_DEBUG
}

void stereo_scheduler(void *param)
{
	int ret_val;
	
	struct stereo_object * ptr_stereo_object =
		(struct stereo_object *) param;

	// for debug: open the required windows
	stereo_open_display_windows();

	// enter into scheduling
	while (1) {

		// IMU DATA + CAMERA IMAGEs
		//ret_val = stereo_input_metadata(res_meta);
		//if (ret_val) { goto err_ret; }

		// OUTPUT => e.g. METADATA
		//ret_val = stereo_output_request(stereo_packet);
		//if (ret_val) { goto err_ret; }

		// INPUT: CAMERA IMAGEs (TWO Camera data)
		ret_val = stereo_input_camera(ptr_stereo_object);
		if (ret_val) { goto err_ret; }

		// PROCESS: TO JPEG
		ret_val = stereo_process_input(ptr_stereo_object);
		if (ret_val) { goto err_ret; }

		// OUTPUT:e.g. IMAGES
		ret_val = stereo_output_request(ptr_stereo_object);
		if (ret_val) { goto err_ret; }

		//  wait until ESC key
		if (cv::waitKey(10) == 27) { // delay: Tune it.
			break;
		}
		
	}

err_ret:
	destroyWindow(WINDOW_STEREO_INPUT);
	destroyWindow(WINDOW_STEREO_OUTPUT);
	destroyWindow(WINDOW_STEREO_JPEG);
	destroyWindow(WINDOW_STEREO_DEBUG);
		
	printf("stereo_scheduler thread closing: %d\n", ret_val);
}

int stereo_terminate()
{
	// All memory resource should be deallocated in the 
	// module thread before exiting.
	// Terminate will only wait for the thread to exit and
	// exit the module gracefully.
	
	// WAIT for Server loop to end
	printf("Stereo: Starting stereo_terminate\n");
	WaitForSingleObject(h_stereo_scheduler, INFINITE);
	printf("Stereo: ending stereo_terminate\n");

	return 0;
}

int stereo_execute()
{
	printf("In stereo_execute\n");

	// create a memory space for stereo object
	struct stereo_object * ptr_stereo_object = 
		(struct stereo_object *) malloc(sizeof(struct stereo_object));

	if (ptr_stereo_object == NULL) {
		cerr << "Error in stereo alloc" << endl; goto ret_err; }
	
	// init the output
	if (stereo_output_init(ptr_stereo_object) < 0) {
		cerr << "Couldn't open output file " << endl; goto ret_err;
	}

	// init the input
	if (stereo_input_init(ptr_stereo_object) < 0) {
		cerr << "Error in stereo input init" << endl; goto ret_err; }


	// start stereo module scheduler
	printf("stereo: Starting stereo_scheduler\n");
	h_stereo_scheduler = (HANDLE)_beginthread(stereo_scheduler, 0,
		(void *)ptr_stereo_object);

	return 0;
ret_err:
	return -1;
}

