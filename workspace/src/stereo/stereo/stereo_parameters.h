#pragma once

#define FRAME_WIDTH    (1080u)
#define FRAME_HEIGHT   (720u)
#define FRAME_CHANNELS (1u) // 1 => Grayscale
#define FRAME_SIZE     (FRAME_WIDTH * FRAME_HEIGHT * FRAME_CHANNELS)
#define MAX_FRAME_SIZE FRAME_SIZE

// Request tags
#define REQ_METADATA "metadata"
#define REQ_IMAGES   "stereoRL"
#define MAX_REQ_SIZE (16u)

struct metadata_packet
{
	// IMU values. Written by Genesys only
	unsigned int  timestamp;
	unsigned int  imu_latitude;
	unsigned int  imu_longitude;
	
	// Odometry values.  Written by Odo (HSK-EL) only
	unsigned int  odo_latitude;
	unsigned int  odo_longitude;

	// TODO: other IMU variables should be appended

	// Image features. Written by Atlatec only
	unsigned int  frame_width;
	unsigned int  frame_height;

};

struct stereo_data
{
	unsigned char metadata[128];		   // IMU + Stereo metadata
	unsigned char frame_right[FRAME_SIZE]; // Stereo Right camera data 
	unsigned char frame_left[FRAME_SIZE];  // Stereo Left camera data 
};

// TODO change to system_object, also the file name system_parameters.h
struct stereo_object 
{
	// INOUT: Metadata
	struct metadata_packet meta_pkt;

    // IN: RAW (obtained from camera are stored into)
	unsigned char *ptr_frame_right; // Stereo Right camera data 
	unsigned char *ptr_frame_left;  // Stereo Left camera data 

	// OUT: JPEG. both left + right
	unsigned char *ptr_jpeg_stream;

	// PROCESS: module supporting variables
	unsigned int  frame_size;

};
