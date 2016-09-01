#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ftw.h>
#include <stdlib.h>

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

using namespace cv;

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
  int rv = remove(fpath);

  if (rv) { perror(fpath); }

  return rv;
}

int rmrf(char const*path) {
  return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

void createFolders() {
  struct stat st = {0};
  
  printf("Creating folder structure if not exists...\n");

  if (stat("blurframes",&st) == -1) {
    mkdir("blurframes",0755);
  }
  if (stat("frames",&st) == -1) {
    mkdir("frames",0755);
  }
}

int getTotalFrames(CvCapture* capture) {

  printf("Counting frames...\n");

  double tFrames = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_COUNT);
  
  int totalFrames = (int) tFrames;
  printf("Total frames: %i\n", totalFrames);

  return totalFrames;

}

void createVideoFrames(CvCapture* capture, int totalFrames) {

  IplImage* frame = NULL;
  int currentFrameCount = 0;

  char buf [25];

  printf("Splitting video into frames...\n");

  while (currentFrameCount < totalFrames) {
    frame = cvQueryFrame(capture);
    sprintf(buf, "frames/frame%d.jpg", currentFrameCount);
    cvSaveImage(buf,frame);
    currentFrameCount++;
  }
}

void applyGaussianBlur(int totalFrames, int kernelSize) {

  Mat src; Mat dst;
  char buf [25];
  char bbuf [30];
  int acurrentFrameCount = 0;

  printf("Applying Gaussian Blur to frames...\n");

  while (acurrentFrameCount < totalFrames) {
    sprintf(buf, "frames/frame%d.jpg", acurrentFrameCount);
    sprintf(bbuf, "blurframes/frame%d.jpg", acurrentFrameCount);
    src = imread(buf, IMREAD_COLOR);
    GaussianBlur(src, dst, Size(kernelSize, kernelSize), 0, 0);
    imwrite(bbuf, dst);
    acurrentFrameCount++;
  }
  
}

void createFinalVideo(CvCapture* capture, int totalFrames, const char* video) { 

  IplImage* img = cvLoadImage("blurframes/frame0.jpg");
  
  CvSize size;
  size.width = img->width;
  size.height = img->height;

  double tfps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
  int fps = (int) tfps;

  CvVideoWriter* writer = cvCreateVideoWriter(
      video,
      CV_FOURCC('X','V','I','D'),
      fps,
      size);

  char bbuf [30];
  int bcurrentFrameCount = 0;

  printf("Creating final video...\n");

  while (bcurrentFrameCount < totalFrames) {
    sprintf(bbuf, "blurframes/frame%d.jpg", bcurrentFrameCount);
    img = cvLoadImage(bbuf);
    cvWriteFrame(writer, img);

    bcurrentFrameCount++;
  }

  cvReleaseVideoWriter(&writer);
  cvReleaseImage(&img);

}

int getKernelSize(const char* kernelSizeChar) {

  int kernelSize;

  char* p;

  int errno = 0;
  long conv = strtol(kernelSizeChar, &p, 10);

  printf("Validating blur level...\n");

  if (errno != 0 || *p != '\0' || conv > INT_MAX) {
    printf("Invalid number\n");
    return 1;
  } else {
    kernelSize = conv;
  }

  if (kernelSize % 2 == 0 || kernelSize < 0) {
    printf("Blur Level Must be a Odd Positive Number\n");
    return 1;
  } 

    return kernelSize;
}

int checkArgCount (int argc) {
  if (argc != 4) {
    printf("This program requires 3 arguments ->" 
       " <String Input Video> <String Output Video.avi>" 
       " <Integer Blur Level (Odd Positive Number)>\n");

    return 1;
  } else {
    return 0;
  }
}
#endif
