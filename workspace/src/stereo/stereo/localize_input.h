#pragma once

#include "stereo_parameters.h"


// Structure
struct localize_object
{
	// INOUT: Metadata
	struct metadata_packet meta_pkt;
	
	// IN: JPEG (obtained stereo data stored into )
	unsigned char *ptr_jpeg_stream; // both left + right

	// PROCESS: RAW (decompressed jpeg stored in to )
	unsigned char *ptr_frame_right; 
	unsigned char *ptr_frame_left; 

	// PROCESS: module supporting variables
	unsigned int  frame_size;

};

// Function declaration
int localize_input_init(struct stereo_object *ptr_system_object);
int localize_input_deinit(struct stereo_object *ptr_system_object);
int localize_input_process(struct stereo_object *ptr_system_object);

int localize_input_request_metadata(char *cmd, char *buf, int len);
int localize_input_request_images(char *cmd, char *buf, int len);