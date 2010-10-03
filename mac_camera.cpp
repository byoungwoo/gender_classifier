#include "mac_camera.h"
#include "exceptions.h"

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
  // frame will be deleted be main programm
}

void MacCamera::Retrieve(cv::Mat &matrix) {
  frame = cvQueryFrame(capture);
  matrix = cv::Mat(frame);
}
