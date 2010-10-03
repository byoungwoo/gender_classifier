#ifndef _FEATURE_DETECTOR_H_
#define _FEATURE_DETECTOR_H_

#include <string>

#include <opencv/cv.h>

const std::string leftEyeFile = "haarcascades/haarcascade_mcs_lefteye.xml";
const std::string mouthFile = "haarcascades/haarcascade_mcs_mouth.xml";
const std::string noseFile = "haarcascades/haarcascade_mcs_nose.xml";
const std::string rightEyeFile = "haarcascades/haarcascade_mcs_righteye.xml";

class FeatureDetector {
public:
  FeatureDetector();

  cv::Point LocateLeftEye(const cv::Mat &face);
  cv::Point LocateRightEye(const cv::Mat &face);
  cv::Point LocateMouth(const cv::Mat &face);
  cv::Point LocateNose(const cv::Mat &face);

private:
  cv::CascadeClassifier leftEyeDetector;
  cv::CascadeClassifier rightEyeDetector;
  cv::CascadeClassifier mouthDetector;
  cv::CascadeClassifier noseDetector;

  cv::Point leftEye;
  cv::Point rightEye;
  cv::Point mouth;
  cv::Point nose;
};

#endif /* _FEATURE_DETECTOR_H_ */
