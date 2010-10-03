#ifndef _FACE_DETECTOR_H_
#define _FACE_DETECTOR_H_

#include <string>

#include <opencv/cv.h>

const std::string frontalFaces = "haarcascades/haarcascade_frontalface_alt.xml";

class FaceDetector {
public:
  FaceDetector();

  cv::Rect Detect(const cv::Mat &image);
private:
  cv::CascadeClassifier detector;
};

#endif /* _FACE_DETECTOR_H_ */
