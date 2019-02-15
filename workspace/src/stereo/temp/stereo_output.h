/*
 * File: stereo_output.h
 *
 * The output interface public APIs of stereo data streaming.
 * INPUT: Timestamp, IMU, SatGPS 
 * OUTPUT: Timestamp, IMU, SatGPS, Stereo
 *   Note: SaGPS: Satellite GPS coordinates
 *
 */

int stereo_output_init();

