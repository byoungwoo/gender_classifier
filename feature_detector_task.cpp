#include "feature_detector_task.h"

FeatureDetectorTask::FeatureDetectorTask(FaceDetector *faceDetector,
                                   FeatureDetector *featureDetector)
  : faceDetector(faceDetector),
    featureDetector(featureDetector), 
    image() {}

FaceDescriptor FeatureDetectorTask::operator()() {
  FaceDescriptor descriptor;
  if(faceDetector && featureDetector && !image.empty()) {
    descriptor.faceBounds = faceDetector->Detect(image);
    cv::Rect faceRegion = descriptor.faceBounds;

    // Left eye
    cv::Rect leftEyeRegion(faceRegion.x, faceRegion.y,
                           faceRegion.width / 2, faceRegion.height * 2 / 3);
    if(leftEyeRegion.area()) {
      descriptor.leftEye = featureDetector->LocateLeftEye(image(leftEyeRegion));
    }
    
    //Right eye
    cv::Rect rightEyeRegion(faceRegion.x + faceRegion.width / 2, faceRegion.y,
                            faceRegion.width / 2, faceRegion.height * 2 / 3);
    if(rightEyeRegion.area()) {
      descriptor.rightEye = featureDetector->LocateLeftEye(image(rightEyeRegion)) +
        cv::Point(faceRegion.width / 2, 0);
    }
      
    cv::Rect mouthRegion(faceRegion.x, faceRegion.y + faceRegion.height / 2,
                       faceRegion.width, faceRegion.height / 2);
    if(mouthRegion.area()) {
      descriptor.mouth = featureDetector->LocateMouth(image(mouthRegion)) +
        cv::Point(0, faceRegion.height / 2); 
    }
    // descriptor.nose = featureDetector->LocateNose(face); 
  }
  return descriptor;
}

