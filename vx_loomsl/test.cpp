#include "loom_shell_util.h"

// int main(){
//     // create context, configure, and initialize
//     ls_context context;
//     context = lsCreateContext();
//     lsSetOutputConfig(context, VX_DF_IMAGE_RGB, 3840, 1920);
//     lsSetCameraConfig(context, 6, 1, VX_DF_IMAGE_RGB, 1920, 1080 * 6);
//     lsImportConfiguration(context, "pts", "\panotool\Panorama.pts");
//     lsSetCameraModule(context, "vx_loomio_bm", "com.amd.loomio_bm.capture", "30,0,0,16");
//     lsSetOutputModule(context, "vx_loomio_bm", "com.amd.loomio_bm.display", "30,0,0");
//     lsInitialize(context);
//     // process live from camera until aborted by input capture plug-in
//     run(context, 0);
//     // release the context
//     lsReleaseContext(&context);
//     return 0;
// }

// Step 1: Calibrate cameras
//Save test input images from all cameras into BMP files: \"CAM00.bmp\", \"CAM01.bmp\", \"CAM02.bmp\", ..., and \"CAM15.bmp\". 
//Align these test input images using PTGui Pro and save the project into \"myrig.pts\" (it should be in ASCII text format).

// Step 2: Use the below script to generate stitched 4K output
//create context, configure, and initialize
#include <stdio.h>
int main(){
	ls_context context;
	context = lsCreateContext();
	lsSetOutputConfig(context, VX_DF_IMAGE_RGB, 3840, 1920);
	lsSetCameraConfig(context, 6, 1, VX_DF_IMAGE_RGB, 1920, 1080 * 6);
	lsImportConfiguration(context, "pts", "Panorama.pts");
	lsExportConfiguration(context, "loom_shell", "loom_shell.txt");
	showConfiguration(context, "loom_shell");
	lsInitialize(context);

	//create buffers for input and output
	cl_context opencl_context;
	cl_mem buf[2];
	lsGetOpenCLContext(context, &opencl_context);
	createBuffer(opencl_context, 3 * 1920 * 1080 * 6, &buf[0]);
	createBuffer(opencl_context, 3 * 3840 * 1920, &buf[1]);

	//load input images into buf[0]
	loadBufferFromMultipleImages(buf[0], "%d.bmp", 6, 1, VX_DF_IMAGE_RGB, 1920, 1080 * 6);

	//process camera inputs from buf[0] into stitched output in buf[1]
	lsSetCameraBuffer(context, &buf[0]);
	lsSetOutputBuffer(context, &buf[1]);
	run(context, 1);
	lsExportConfiguration(context, "data", "data");
	//save the output
	saveBufferToImage(buf[1], "output.bmp", VX_DF_IMAGE_RGB, 3840, 1920);

	//release all resources
	releaseBuffer(&buf[0]);
	releaseBuffer(&buf[1]);
	lsReleaseContext(&context);
	getchar();
}
