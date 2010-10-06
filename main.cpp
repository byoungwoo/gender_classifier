#include <iostream>

#include <boost/thread.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "mac_camera.h"
#include "face_detector.h"
#include "face_normalizer.h"
#include "feature_detector.h"
#include "feature_detector_task.h"


int main( int argc, char** argv )
try {
  MacCamera camera;
  FaceDetector faceDetector;
  FeatureDetector featureDetector;
  FeatureDetectorTask featureDetectorTask(&faceDetector, &featureDetector);

  cv::namedWindow("video");
  cv::namedWindow("detected face");
  cv::namedWindow("transformed face");

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
        cv::rectangle(face, descriptor.mouth, descriptor.mouth + delta, white);
        cv::imshow("detected face", face);
      }
      if(FaceNormalizer::ValidateFaceDescriptor(descriptor) == FaceNormalizer::VALID) {
        cv::Mat transformedFace = FaceNormalizer::Normalize(face, descriptor);
        cv::imshow("transformed face", transformedFace);
      }
      
    }

    if(future.is_ready() || future.get_state() == boost::future_state::uninitialized) {
      featureDetectorTask.image = frame;
      boost::packaged_task<FaceDescriptor> packagedTask(featureDetectorTask);
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
 
