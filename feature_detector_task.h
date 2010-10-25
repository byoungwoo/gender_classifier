#ifndef _FEATURE_DETECTOR_TASK_H_
#define _FEATURE_DETECTOR_TASK_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "face_detector.h"
#include "feature_detector.h"


struct FaceDescriptor {
  cv::Rect faceBounds;
  cv::Point leftEye;
  cv::Point rightEye;
  cv::Point mouth;
  cv::Point nose;
};

struct FeatureDetectorTask {
  explicit FeatureDetectorTask(FaceDetector *faceDetector = 0,
                            FeatureDetector *featureDetector = 0);

  FaceDescriptor operator()();

  FaceDetector *faceDetector;
  FeatureDetector *featureDetector;
  cv::Mat image;
};


#endif /* _FEATURE_DETECTOR_TASK_H_ */
