#pragma once
#include <iostream>
#include <vector>

// MACROS
#define WINDOW_STEREO_INPUT "Stereo Input"
#define WINDOW_STEREO_OUTPUT "Stereo Output"
#define WINDOW_STEREO_JPEG   "Stereo JPEG"
#define WINDOW_STEREO_DEBUG   "Stereo Debug"

// uncomment the require debug
//#define DEBUG_STEREO_INPUT  (1u)
//#define DEBUG_STEREO_OUTPUT (1u)
//#define DEBUG_STEREO_DEBUG  (1u)
#define DEBUG_STEREO_JPEG   (1u)
#define STEREO_QUALITY_VALUE (95u) // Quality percentage

// Function declarations
int stereo_input_init(struct stereo_object *ptr_stereo_packet);
int stereo_input_deinit(struct stereo_object *ptr_stereo_packet);

//int stereo_input_camera(std::vector<uchar> &buf);
int stereo_input_metadata(struct stereo_object *);
int stereo_input_camera(struct stereo_object *);

