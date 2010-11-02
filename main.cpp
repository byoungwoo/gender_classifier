#include <iostream>

#include <boost/thread.hpp>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

#include "mac_camera.h"
#include "face_detector.h"
#include "face_normalizer.h"
#include "feature_detector.h"
#include "feature_detector_task.h"


const cv::Size THUMBNAIL_SIZE = cv::Size(13, 25);

cv::Mat GetImageFeatures(const cv::Mat &image) {
  cv::Mat resizedImage;
  cv::resize(image, resizedImage, THUMBNAIL_SIZE);
  cv::Mat features(1, resizedImage.rows * resizedImage.cols,
                   CV_32FC1, image.data);
  float *feature = features.ptr<float>(0);
  size_t index = 0;
  for(size_t rowIndex = 0; rowIndex < resizedImage.rows; ++rowIndex) {
    const unsigned char *row = resizedImage.ptr<unsigned char>(rowIndex);
    for(size_t colIndex = 0; colIndex < resizedImage.cols; ++colIndex) {
      feature[index] = static_cast<double>(row[colIndex]) / 255.0;
      ++index;
    }
  }
  return features;
}

const std::string MODEL_PATH = "svm_model/gender_model.xml";

int main( int argc, char** argv )
try {
  MacCamera camera;
  FaceDetector faceDetector;
  FeatureDetector featureDetector;
  FeatureDetectorTask featureDetectorTask(&faceDetector, &featureDetector);

  CvSVM classifier;
  classifier.load(MODEL_PATH.c_str());

  cv::namedWindow("video");
  cv::namedWindow("detected face");
  cv::namedWindow("transformed face");

  boost::unique_future<FaceDescriptor> future;
  while(cvWaitKey(33) != 27) {
    cv::Mat frame;
    camera.Retrieve(&frame);
    cv::Mat flippedFrame;
    const int FLIP_HOR = 1;
    cv::flip(frame, flippedFrame, FLIP_HOR);
    cv::imshow("video", flippedFrame);

    if(future.has_value()) {
      FaceDescriptor descriptor = future.get();
      cv::Scalar white(255, 255, 250);
      cv::Point delta(5, 5);
      if(descriptor.faceBounds.area()) {
        FaceDescriptor normalDescriptor;
        if(FaceNormalizer::ValidateFaceDescriptor(descriptor) ==
           FaceNormalizer::VALID) {
          cv::Point upperLeftCorner = cv::Point(descriptor.faceBounds.x,
                                                descriptor.faceBounds.y);
          normalDescriptor.leftEye = descriptor.leftEye + upperLeftCorner;
          normalDescriptor.rightEye = descriptor.rightEye + upperLeftCorner;
          normalDescriptor.mouth = descriptor.mouth + upperLeftCorner;
          cv::Mat transformedFace = FaceNormalizer::Normalize(frame, normalDescriptor);
          cv::imshow("transformed face", transformedFace);

          cv::Mat features = GetImageFeatures(transformedFace);
          std::cout << classifier.predict(features) << std::endl;

          cv::Mat grayFrame;
          cv::cvtColor(frame, grayFrame, CV_RGB2GRAY);
          cv::rectangle(grayFrame, descriptor.faceBounds, white);
          cv::rectangle(grayFrame, normalDescriptor.leftEye,
                        normalDescriptor.leftEye + delta, white);
          cv::rectangle(grayFrame, normalDescriptor.rightEye,
                        normalDescriptor.rightEye + delta, white);
          cv::rectangle(grayFrame, normalDescriptor.mouth,
                        normalDescriptor.mouth + delta, white);
          cv::imshow("detected face", grayFrame);
        }
      }
      
    }

    if(future.is_ready() || future.get_state() == boost::future_state::uninitialized) {
      featureDetectorTask.image = frame;
      boost::packaged_task<FaceDescriptor> packagedTask(featureDetectorTask);
      future = packagedTask.get_future();
      boost::thread task(boost::move(packagedTask));
    }
  }

  return 0;
 } catch(cv::Exception openCVException) {
  std::cerr << "OpenCV:\t" <<  openCVException.what() << std::endl;
 } catch(std::exception e) {
  std::cerr << "Internal:\t" << e.what() << std::endl;
 }
 
