#include <iostream>

#include <boost/thread.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "mac_camera.h"
#include "face_detector.h"
#include "feature_detector.h"

struct FaceDescriptor {
  cv::Rect faceBounds;
  cv::Point leftEye;
  cv::Point rightEye;
  cv::Point mouth;
  cv::Point nose;
};

struct FaceDetectorTask {
  explicit FaceDetectorTask(FaceDetector *faceDetector = 0,
                            FeatureDetector *featureDetector = 0)
  : faceDetector(faceDetector),
    featureDetector(featureDetector), 
    image() {}

  FaceDescriptor operator()() {
    FaceDescriptor descriptor;
    if(faceDetector && featureDetector && !image.empty()) {
      descriptor.faceBounds = faceDetector->Detect(image);
      cv::Mat face = image(descriptor.faceBounds);
      descriptor.leftEye = featureDetector->LocateLeftEye(face); 
      descriptor.rightEye = featureDetector->LocateRightEye(face); 
      // descriptor.mouth = featureDetector->LocateMouth(face); 
      // descriptor.nose = featureDetector->LocateNose(face); 
    }
    return descriptor;
  }

  FaceDetector *faceDetector;
  FeatureDetector *featureDetector;
  cv::Mat image;
};

int main( int argc, char** argv )
try {
  MacCamera camera;
  FaceDetector faceDetector;
  FeatureDetector featureDetector;
  FaceDetectorTask faceDetectorTask(&faceDetector, &featureDetector);

  cv::namedWindow("video");
  cv::namedWindow("face");

  char key;
  boost::unique_future<FaceDescriptor> future;
  while( key != 'q' ) {
    cv::Mat frame;
    camera.Retrieve(frame);
    cv::imshow("video", frame);

    if(future.has_value()) {
      FaceDescriptor descriptor = future.get();
      cv::Scalar white(255, 255, 250);
      cv::Point delta(5, 5);
      if(descriptor.faceBounds.area()) {
        cv::Mat face = frame(descriptor.faceBounds);
        cv::rectangle(face, descriptor.leftEye, descriptor.leftEye + delta, white);
        cv::rectangle(face, descriptor.rightEye, descriptor.rightEye + delta, white);
        // cv::rectangle(face, descriptor.mouth, descriptor.mouth, white);
        // cv::rectangle(face, descriptor.nose, descriptor.nose, white);
        cv::imshow("face", face);
      }
    }

    if(future.is_ready() || future.get_state() == boost::future_state::uninitialized) {
      faceDetectorTask.image = frame;
      boost::packaged_task<FaceDescriptor> packagedTask(faceDetectorTask);
      future = packagedTask.get_future();
      boost::thread task(boost::move(packagedTask));
    }
    

    key = cvWaitKey( 10 );
  }

  return 0;
 } catch(cv::Exception openCVException) {
  std::cerr << "OpenCV:\t" <<  openCVException.what() << std::endl;
 } catch(std::exception e) {
  std::cerr << "Internal:\t" << e.what() << std::endl;
 }
 
