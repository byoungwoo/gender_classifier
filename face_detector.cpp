#include <vector>

#include "face_detector.h"

FaceDetector::FaceDetector() {
  detector.load(frontalFaces);
}

cv::Rect FaceDetector::Detect(const cv::Mat &image) {
  std::vector<cv::Rect> faces;
  detector.detectMultiScale(image, faces);

  if(faces.size() > 0) {
    return faces[0];
  } else {
    return cv::Rect();
  }
  
}
