#pragma once
#include <iostream>
#include <vector>
int stereo_input_init();
int stereo_input_deinit();
int stereo_input_camera(std::vector<uchar> &buf);

