#include "stdafx.h"

#include <opencv2/highgui.hpp> // for waitkey()

#include <windows.h>

// Threads: to run streaming server
#include <process.h>

#include "stereo_input.h"
#include "stereo_output.h"
#include "stereo_parameters.h"
#include "localize_input.h"

using namespace std;

// STORAGE
struct stereo_data stereo_packet;

// Thread, scheduler
static HANDLE h_stereo_scheduler;

// Other module APIs
int localize_execute();
int localize_terminate();

int stereo_process_request(struct stereo_data *stereo_packet);
int stereo_process_input(struct stereo_data *stereo_packet);

void stereo_scheduler(void *param)
{
	unsigned char *res_meta = stereo_packet.metadata;
	unsigned char *res_imgs = stereo_packet.frame_right;
	
	int ret_val;

	while(1) {

		// IMU DATA + CAMERA IMAGEs
		ret_val = stereo_input_metadata(res_meta);
		if (ret_val) { goto err_ret; }

		// PROCESS REQUEST, OUTPUT => e.g. METADATA
		ret_val = stereo_output_request(&stereo_packet);
		if (ret_val) { goto err_ret; }

		// CAMERA IMAGEs (TWO Camera data)
		ret_val = stereo_input_camera(res_imgs);
		if (ret_val) { goto err_ret; }

		// PROCESS INPUT DATA => TO JPEG
		ret_val = stereo_process_input(&stereo_packet);
		if (ret_val) { goto err_ret; }

		// PROCESS REQUEST, OUTPUT => e.g. IMAGES
		ret_val = stereo_output_request(&stereo_packet);
		if (ret_val) { goto err_ret; }

	}

err_ret:
	printf("stereo_scheduler thread closing: %d\n", ret_val);
}

int stereo_execute()
{
	printf("In stereo_execute\n");

	if (stereo_input_init() < 0) {
		cerr << "Error in stereo input init" << endl;
		goto ret_err;
	}

	if (stereo_output_init() < 0) {
		cerr << "Couldn't open output file " << endl;
		goto ret_err;
	}

	printf("stereo: Starting stereo_scheduler\n");
	h_stereo_scheduler = (HANDLE)_beginthread(stereo_scheduler, 0, NULL);

	return 0;
ret_err:
	return -1;
}

int stereo_terminate()
{

	stereo_output_deinit();

//	stereo_input_deinit();

	// WAIT for Server loop to end
	printf("Stereo: Starting stereo_terminate\n");
	WaitForSingleObject(h_stereo_scheduler, INFINITE);
	printf("Stereo: ending stereo_terminate\n");

	return 0;
}
int main(int argc, char **argv)
{

	stereo_execute();
	localize_execute();

	while (cv::waitKey(1) < 1);
	
	localize_terminate();
	stereo_terminate();

	return 0;
}

// NOTE: Multiple time run-stop-run, code is resulting in error: opencvxx.dll. Cannot find or open the PDB file.
// FIX: https://stackoverflow.com/questions/12954821/cannot-find-or-open-the-pdb-file-in-visual-studio-c-2010
// i.e. Tools > Options > Debugging > General > Load dll exports (Native only)
// Crash frequency is reduced but still observed.
// FIXED: stereo_input_init() was not returning anything when success. 
// Compiler did not thrown compilation error that retun is required.
// After adding code of "return 0" for success, the code seems to be stable.
// Removed "Load dll exports (Native only)" setting, and multiple execution is success. 
// Thus this a code bug.

