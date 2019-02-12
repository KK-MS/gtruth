#include "stdafx.h"

#include <opencv2/highgui.hpp> // for waitkey()


using namespace std;

int stereo_execute();
int localize_execute();
int localize_terminate();
int stereo_terminate();

int main(int argc, char **argv)
{
	localize_execute();
	stereo_execute();
	

	while (cv::waitKey(1) < 1);

	cvDestroyAllWindows();
	//localize_terminate();
	//stereo_terminate();

	return 0;
}

// NOTE: Multiple time run-stop-run, code is resulting in error: opencvxx.dll. Cannot find or open the PDB file.
// FIX: https://stackoverflow.com/questions/12954821/cannot-find-or-open-the-pdb-file-in-visual-studio-c-2010
// i.e. Tools > Options > Debugging > General > Load dll exports (Native only)
// Crash frequency is reduced but still observed.
// FIXED: stereo_input_init() was not returning anything when success. 
// Compiler did not thrown compilation error that retun is required.
// After adding code of "return 0" for success, the code seems to be stable.
// Removed "Load dll exports (Native only)" setting, and multiple execution is success. 
// Thus this a code bug.

