#include "functions.h"

int main(int argc, char** argv) {

  if (checkArgCount(argc)==1) {
    return 1;
  }
  CvCapture* capture = cvCaptureFromFile(argv[1]);
  createFolders();
  int totalFrames = getTotalFrames(capture); 
  createVideoFrames(capture, totalFrames);
  int kernelSize = getKernelSize(argv[3]);
  applyGaussianBlur(totalFrames, kernelSize);
  createFinalVideo(capture, totalFrames, argv[2]);
  printf("Cleaning up...\n");
  cvReleaseCapture(&capture);
  rmrf("blurframes");
  rmrf("frames");

  return 0;
}
