#include <cmath>

#include "face_normalizer.h"

namespace FaceNormalizer {

const int MAX_EYES_YDELTA = 15;
DescriptorState ValidateFaceDescriptor(const FaceDescriptor &descriptor) {
  // std::cout << "Left eye: " << descriptor.leftEye.y << " Right eye: " << descriptor.rightEye.y << " Mouth: " << descriptor.mouth.x << ' ' << descriptor.mouth.y << std::endl; 
  bool eyesYDeltaCriteria = (abs(descriptor.leftEye.y - 
                                 descriptor.rightEye.y) < MAX_EYES_YDELTA);
  bool nullCoordinateCriteria = (descriptor.leftEye.y != 0) &&
    (descriptor.rightEye.y != 0) &&
    (descriptor.mouth.x != 0);

  bool mouthCenteredCriteria = (descriptor.mouth.x <= descriptor.faceBounds.width * 0.666) && (descriptor.mouth.x >= descriptor.faceBounds.width * 0.333);
  if(eyesYDeltaCriteria && nullCoordinateCriteria && mouthCenteredCriteria) {
    return VALID;
  } else {
    return INVALID;
  }
} 

cv::Point GetCenter(const cv::Point &a, const cv::Point &b,
                    const cv::Point &c) {
  return cv::Point((a.x + b.x + c.x) / 3, (a.y + b.y + c.y) / 3);
}

int GetArea(const cv::Point &a, const cv::Point &b,
                    const cv::Point &c) {
  return ((a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y)) / 2;
}

const cv::Size PRINT_SIZE(240, 320);
const bool LOAD_GRAY = 0;
const double PI = 3.14159265;

cv::Mat Normalize(const cv::Mat &image, const FaceDescriptor &descriptor) {
  // // Performing Homografy transformation
  // cv::Rect faceRect = descriptor.faceBounds;
  // cv::Point upperLeftPoint = cv::Point(faceRect.x, faceRect.y);
  // std::vector<cv::Point2f> sourcePoints(4);
  // sourcePoints[0] = descriptor.leftEye;
  // sourcePoints[1] = descriptor.rightEye;
  // sourcePoints[2] = descriptor.mouth;
  // sourcePoints[3] = (sourcePoints[0] + sourcePoints[1] + sourcePoints[2]) * 0.3333;

  // std::vector<cv::Point2f> destinationPoints(4);
  // destinationPoints[0] = cv::Point2f(faceRect.width * 0.25, faceRect.height * 0.25);
  // destinationPoints[1] = cv::Point2f(faceRect.width * 0.75, faceRect.height * 0.25);
  // destinationPoints[2] = cv::Point2f(faceRect.width * 0.50, faceRect.height * 0.75);
  // destinationPoints[3] = (destinationPoints[0] + destinationPoints[1] + 
  //                         destinationPoints[2]) * 0.3333;

  // cv::Mat homographyTransformation = cv::findHomography(cv::Mat(sourcePoints), 
  //                                               cv::Mat(destinationPoints));
  // cv::Mat transformedImage;
  // cv::warpPerspective(image, transformedImage, homographyTransformation, cv::Size(image.cols, image.rows));


  // See ideal_face.jpg
  int unit = PRINT_SIZE.width / 6;
  FaceDescriptor ideal;
  ideal.leftEye = cv::Point(1.75 * unit, 4 * unit);
  ideal.rightEye = cv::Point(4.25 * unit, 4 * unit);
  ideal.mouth = cv::Point(3 * unit, 6.75 * unit);
  cv::Point idealCenter = GetCenter(ideal.leftEye, ideal.rightEye,
                                    ideal.mouth);
  // int idealMera = GetArea(ideal.leftEye, ideal.rightEye,
  //                         ideal.mouth);
  int idealMera = std::abs(ideal.leftEye.x - ideal.rightEye.x);
  // std::cout << "Ideal " << ideal.leftEye.x << ' ' << ideal.leftEye.y << '\t'
  //           << ideal.rightEye.x << ' ' << ideal.rightEye.y << '\t' 
  //           << ideal.mouth.x << ' ' << ideal.mouth.y << std::endl;

  const FaceDescriptor &real = descriptor;
  cv::Point realCenter = GetCenter(real.leftEye, real.rightEye,
                                   real.mouth);
  // int realMera = GetArea(real.leftEye, real.rightEye,
  //                        real.mouth);
  int realMera = std::abs(real.leftEye.x - real.rightEye.x);
  // std::cout << "Real " << real.leftEye.x << ' ' << real.leftEye.y << '\t'
  //           << real.rightEye.x << ' ' << real.rightEye.y << '\t' 
  //           << real.mouth.x << ' ' << real.mouth.y << std::endl;

  double rotateAngle = std::atan(static_cast<double>(real.rightEye.y - real.leftEye.y) /
                                 (real.rightEye.x - real.leftEye.x)) * 180 / PI;
  // std::cout << "Rotation angle " << rotateAngle << std::endl;

  double scale = static_cast<double>(idealMera) / realMera;
  // std::cout << "Scale " << scale << std::endl;

  cv::Mat rotationMatrix = cv::getRotationMatrix2D(realCenter, rotateAngle,
                                                   scale);
  cv::Mat grayImage;
  cv::cvtColor(image, grayImage, CV_RGB2GRAY);
  cv::Mat transformedImage;
  cv::warpAffine(grayImage, transformedImage, rotationMatrix,
                 image.size());
  cv::Rect clipper(realCenter.x - idealCenter.x,
                   realCenter.y - idealCenter.y,
                   PRINT_SIZE.width, PRINT_SIZE.height);
  cv::Rect imageRect = cv::Rect(cv::Point(0, 0), image.size());
  clipper = imageRect & clipper;
  cv::Mat clippedImage = transformedImage(clipper);
  cv::Mat equalizedImage;
  cv::equalizeHist(clippedImage, equalizedImage);

  cv::Mat normalizedImage;
  cv::Mat mask = cv::imread("resources/face_mask2.bmp", LOAD_GRAY);
  cv::Mat scaledMask;
  cv::resize(mask, scaledMask, clipper.size());

  cv::subtract(clippedImage, scaledMask, normalizedImage);

  return normalizedImage;
}

}
