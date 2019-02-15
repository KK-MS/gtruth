/*
 * File: stereo_main.c
 *
 * The main program of stereo data streaming.
 * INPUT: Timestamp, IMU, SatGPS 
 * OUTPUT: Timestamp, IMU, SatGPS, Stereo
 *   Note: SaGPS: Satellite GPS coordinates
 *
 */

// Includes
#include <stdio.h>
#include "stereo_input.h"
#include "stereo_output.h"

int main()
{
    // To be implemented

    // Initialize the input interfaces
    stereo_input_init();

    // Initialize the output interface.
    // Output is a stream of IMU data + Stereo camera frames
    stereo_output_init();

    return 0;
}

