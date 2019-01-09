#pragma once

#define FRAME_WIDTH    (1080u)
#define FRAME_HEIGHT   (720u)
#define FRAME_CHANNELS (1u) // 1 => Grayscale
#define FRAME_SIZE     (FRAME_WIDTH * FRAME_HEIGHT * FRAME_CHANNELS)

struct stereo_data
{
	unsigned char metadata[64];			   // IMU + Stereo metadata
	unsigned char frame_right[FRAME_SIZE]; // Stereo Right camera data 
	unsigned char frame_left[FRAME_SIZE];  // Stereo Left camera data 
};
