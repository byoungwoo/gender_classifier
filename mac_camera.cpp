#include <cstdlib>

#include "mac_camera.h"
#include "exceptions.h"

#include <iostream>

MacCamera::MacCamera() {
  capture = cvCaptureFromCAM(MAIN_CAMERA);
  if(!capture) {
    throw CameraException();
  }
}

MacCamera::~MacCamera() {
  if(capture) {
    cvReleaseCapture(&capture);
  }
  // if(frame) {
  //   cvReleaseImage(&frame);
  // }
  // // frame will be deleted be main programm
}


// const bool COPY_IMAGE = true;
void MacCamera::Retrieve(cv::Mat *image) {
  IplImage *frame = cvQueryFrame(capture);
  *image = cv::Mat(frame);
  // IplImage *image = cvCreateImage(cvGetSize(frame), 8, 3);
  // image->origin = frame->origin;
  // cvCopy(frame, image, 0);
  // cv::Mat result(image);
  // void *buffer = cvAlloc(frame->imageSize);
  // std::memcpy(buffer, frame->imageData, frame->imageSize);
  // cv::Mat result(frame->width * 3 / 4, frame->height, CV_8UC3, buffer, 2560);
  // cvFree(&buffer);
  // return result;
}
