


---------------------------------------------------
stereo_input.cpp

stereo_input_camera() {
    video_in_left >> cam_frame;
	cv::cvtColor(cam_frame, gray_image_left, CV_BGR2GRAY);
}

stereo_input_deinit() {
    video_in_left.release();
	Free frame memories (left, right, jpeg);
}
stereo_input_init() {
	video_in.open(iVideoCapNum);
	get rows, cols.
	Allocate memory for:
	ptr_frame_left;
	ptr_frame_right;
	ptr_jpeg_stream;
}

--------------------------------------------------
stereo.cpp
void stereo_scheduler(void *param) {
	while(1) {
	    // IMU DATA + CAMERA IMAGEs
		ret_val = stereo_input_metadata(res_meta);

		// PROCESS REQUEST, OUTPUT => e.g. METADATA
		ret_val = stereo_output_request(stereo_packet);

		// CAMERA IMAGEs (TWO Camera data)
		ret_val = stereo_input_camera(res_imgs);

		// PROCESS => TO JPEG
		ret_val = stereo_process_input(stereo_packet);

		// OUTPUT => e.g. IMAGES
		ret_val = stereo_output_request(stereo_packet);}}

stereo_execute() {
    // Storage
    ptr_stereo_packet = (struct stereo_data *) malloc(..);

	// INIT Input & Output
	stereo_input_init(ptr_stereo_packet);
	stereo_output_init(ptr_stereo_packet):

	// THREAD CREATE & START
	_beginthread(stereo_scheduler, 0, ptr_stereo_packet);}

--------------------------------------------------
main.cpp
main()
{
	// init and execute
 	stereo_execute();
	localize_execute();

	while (cv::waitKey(1) < 1);
    
	// deinit and terminate
	
}
