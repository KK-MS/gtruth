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

// Macros
#define TAG_LOC "LOC: "

static HANDLE h_localize_scheduler;

static struct stereo_data stereo_packet;

/**
 * localize_scheduler
 * 
 * This is the localize process scheduler.
 * First it request the metadata, and process it.
 * Secondly request for the image data as per metadata.
 * The stereo data is stored locally in the struct stereo_data type.
 */
void localize_scheduler(void *param)
{
	int iResult;

	printf(TAG_LOC "In localize_scheduler\n");

	char req_meta[] = REQ_METADATA;
	char *res_meta = (char *)stereo_packet.metadata;
	int len_meta = sizeof(stereo_packet.metadata);

	char req_imgs[] = REQ_IMAGES;
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

		// PROCESS IMAGE FRAME

	}

end_client_thread:
	printf(TAG_LOC "localize_scheduler thread closing\n");
	return;
}


/**
 * localize_execute
 * 
 * Localize module main function
 * 
 * does localize_input_init(), e.g. localize_input_network_init()
 * and starts a localize process thread localize_scheduler().
 * The main thread will wait infinitly until thread is exited.
 * This looping is in stereo.cpp file.
 */
int localize_execute()
{
	printf(TAG_LOC "In localize_execute\n");
	
	// create a memory space for system object
	struct stereo_object * ptr_system_object =
		(struct stereo_object *) malloc(sizeof(struct stereo_object));

	if (localize_input_init(ptr_system_object) < 0) {
		goto err_output;
	}

	printf(TAG_LOC "Starting localize_scheduler\n");
	h_localize_scheduler = (HANDLE)_beginthread(localize_scheduler, 0, NULL);
	
	return 0;
err_output:
	return -1;
}

int localize_terminate() 
{
	printf(TAG_LOC "In localize_terminate\n");

	//localize_input_deinit();
	
	printf(TAG_LOC "In localize: WaitForSingleObject h_localize_scheduler\n");
	// WAIT for Server loop to end
	WaitForSingleObject(h_localize_scheduler, INFINITE);

	return 0;
}