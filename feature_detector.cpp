#include "feature_detector.h"

FeatureDetector::FeatureDetector() {
  leftEyeDetector.load(leftEyeFile);
  rightEyeDetector.load(rightEyeFile);
  mouthDetector.load(mouthFile);
  noseDetector.load(noseFile);
}

inline cv::Rect Locate(cv::CascadeClassifier &detector, const cv::Mat& face) {
  std::vector<cv::Rect> results;
  detector.detectMultiScale(face, results);

  if(results.size() > 0) {
    return results[0];
  } else {
    return cv::Rect();
  }
}

inline cv::Point RectCenter(const cv::Rect rect) {
  return cv::Point(rect.x + rect.width / 2,
                   rect.y + rect.height / 2);
}

cv::Point FeatureDetector::LocateLeftEye(const cv::Mat &face) {
  
  return RectCenter(Locate(leftEyeDetector, face));
}

cv::Point FeatureDetector::LocateRightEye(const cv::Mat &face) {
  return RectCenter(Locate(rightEyeDetector, face));
}

cv::Point FeatureDetector::LocateMouth(const cv::Mat &face) {
  return RectCenter(Locate(mouthDetector, face));
}

cv::Point FeatureDetector::LocateNose(const cv::Mat &face) {
  return RectCenter(Locate(noseDetector, face));
}
