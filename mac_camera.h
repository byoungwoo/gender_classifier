#ifndef _MAC_CAMERA_H_
#define _MAC_CAMERA_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>


const int MAIN_CAMERA = 0;
class MacCamera {
public:
  MacCamera();
  ~MacCamera();

  void Retrieve(cv::Mat *image);

private:
  CvCapture *capture;
};
 

#endif /* _MAC_CAMERA_H_ */
