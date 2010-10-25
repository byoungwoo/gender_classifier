#include <iostream>
#include <fstream>

#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "../feature_detector_task.h"
#include "../face_normalizer.h"


const std::string RAWS_PATH = "raw/";
const std::string NORMALIZED_PATH = "normalized/";
const std::string INFO_FILENAME = RAWS_PATH + "info.txt";
const std::string GENDERS_FILENAME = NORMALIZED_PATH + "genders.txt";

int main(int argc, char *argv[]) {
  std::ifstream info(INFO_FILENAME.c_str());
  std::ofstream genders(GENDERS_FILENAME.c_str());

  while(!info.eof()) {
    // static int debug = 1;
    // if(debug++ > 100) break;

    std::string name;
    std::getline(info, name, '\t');
    FaceDescriptor descriptor;
    int x, y;
    info >> x >> y;
    descriptor.leftEye = cv::Point(x, y);
    info >> x >> y;
    descriptor.rightEye = cv::Point(x, y);
    info >> x >> y;
    descriptor.nose = cv::Point(x, y);
    info >> x >> y;
    descriptor.mouth = cv::Point(x, y);
    std::string gender;
    std::getline(info, gender, '\n');

    std::stringstream imageFilename;
    imageFilename << RAWS_PATH << name;
    cv::Mat image = cv::imread(imageFilename.str(), 0);

    cv::Mat normalizedImage =  FaceNormalizer::Normalize(image, descriptor);
    std::stringstream normalizedFilename;
    normalizedFilename << NORMALIZED_PATH << name;
    cv::imwrite(normalizedFilename.str(), normalizedImage);

    genders << name << '\t' << gender << std::endl;
  }
  info.close();
  genders.close();
  return 0;
}
