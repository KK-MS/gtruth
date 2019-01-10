#pragma once

#include <iostream>
#include <vector>

#include "stereo_parameters.h"

//int stereo_output_camera(std::vector<unsigned char> &buf);
int stereo_output_camera(unsigned char *frame_buffer);
int stereo_output_init();
int stereo_output_deinit();
int stereo_output_request(struct stereo_data *stereo_packet);


