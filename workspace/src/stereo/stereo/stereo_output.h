#pragma once
#include <iostream>
#include <vector>
//int stereo_output_camera(std::vector<unsigned char> &buf);
int stereo_output_camera(unsigned char *frame_buffer);
int stereo_output_init();
int stereo_output_deinit();

