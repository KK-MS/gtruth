#pragma once

#include "stereo_parameters.h"
#include "stereo_output.h"

int localize_input_init();
int localize_input_deinit();
int localize_input_process();

int localize_input_request_metadata(char *cmd, char *buf, int len);
int localize_input_request_images(char *cmd, char *buf, int len);