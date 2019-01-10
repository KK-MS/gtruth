#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

// Threads: to run streaming server
#include <process.h>

#include "localize_input.h"

using namespace std;
//using namespace cv

static HANDLE h_localize_scheduler;

static struct stereo_data stereo_packet;

void localize_scheduler(void *param)
{
	int iResult;

	printf("In localize_scheduler\n");

	char req_meta[] = REQ_METADATA;
	char *res_meta = (char *)stereo_packet.metadata;
	int len_meta = sizeof(stereo_packet.metadata);

	char req_imgs[] = REQ_IMAGES; // we need null, thus don't specify size 
	char *res_imgs = (char *)stereo_packet.frame_right;
	int len_imgs = sizeof(stereo_packet.frame_right) * 2; // Two frames L & R

	while (1) {
		// GET METADATA
		if (localize_input_request_metadata(req_meta, res_meta, len_meta) < 0) {
			goto end_client_thread;
		}

		// GET IMAGE FRAMES
		if (localize_input_request_images(req_imgs, res_imgs, len_imgs) < 0) {
			goto end_client_thread;
		}
	}

end_client_thread:
	printf("localize_scheduler thread closing\n");
	return;
}


int localize_execute()
{
	printf("In localize_execute\n");

	if (localize_input_init() < 0) {
		goto err_output;
	}

	printf("Localize: Starting localize_scheduler\n");
	h_localize_scheduler = (HANDLE)_beginthread(localize_scheduler, 0, NULL);
	
	return 0;
err_output:
	return -1;
}

int localize_terminate() 
{
	printf("In localize_terminate\n");

	localize_input_deinit();
	
	printf("In localize: WaitForSingleObject h_localize_scheduler\n");
	// WAIT for Server loop to end
	WaitForSingleObject(h_localize_scheduler, INFINITE);

	return 0;
}