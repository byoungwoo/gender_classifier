#ifndef _FACE_NORMALIZER_H_
#define _FACE_NORMALIZER_H_

#include <iostream>

#include <opencv/cv.h>

#include "feature_detector_task.h"

namespace FaceNormalizer {

enum DescriptorState {
  VALID,
  INVALID
};

DescriptorState ValidateFaceDescriptor(const FaceDescriptor &descriptor);
cv::Mat Normalize(const cv::Mat &image, const FaceDescriptor &descriptor);

}

#endif /* _FACE_NORMALIZER_H_ */
