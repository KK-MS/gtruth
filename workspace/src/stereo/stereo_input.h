/*
 * File: stereo_input.h
 *
 * The input interface public APIs of stereo data streaming.
 * INPUT: Timestamp, IMU, SatGPS 
 * OUTPUT: Timestamp, IMU, SatGPS, Stereo
 *   Note: SaGPS: Satellite GPS coordinates
 *
 */

int stereo_input_init();

