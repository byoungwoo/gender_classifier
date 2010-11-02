#include <iostream>
#include <fstream>

#include <list>
#include <utility>
#include <string>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

const std::string TRAIN_IMAGES = "../train_data/normalized/";
const std::string GENDERS_FILENAME = "genders.txt";
const std::string GENDERMODEL_FILENAME = "gender_model.xml";

const cv::Size THUMBNAIL_SIZE = cv::Size(13, 25);

typedef std::list<std::pair<std::string, int> > SamplesDesctiptors;

SamplesDesctiptors ParseDescriptors(const std::string &descriptorsFilename) {
  std::ifstream descriptors(descriptorsFilename.c_str());
  SamplesDesctiptors result;
  while(!descriptors.eof()) {
    std::string name;
    std::getline(descriptors, name, '\t');
    if(name == std::string()) {
      break;
    }
    descriptors.get();
    std::string gender;
    std::getline(descriptors, gender, '\n');
    result.push_back(std::make_pair(TRAIN_IMAGES + name,
                                    gender == "Male" ? 0 : 1));
  }
  descriptors.close();
  return result;
}

cv::Mat GetImageFeatures(const std::string &imageFilename) {
  cv::Mat image = cv::imread(imageFilename, 0);
  cv::Mat resizedImage;
  cv::resize(image, resizedImage, THUMBNAIL_SIZE);
  cv::Mat features(1, resizedImage.rows * resizedImage.cols,
                   CV_32FC1, image.data);
  float *feature = features.ptr<float>(0);
  size_t index = 0;
  for(size_t rowIndex = 0; rowIndex < resizedImage.rows; ++rowIndex) {
    const unsigned char *row = resizedImage.ptr<unsigned char>(rowIndex);
    for(size_t colIndex = 0; colIndex < resizedImage.cols; ++colIndex) {
      feature[index] = row[colIndex];
      ++index;
    }
  }
  return features;
}

void MakeData(const SamplesDesctiptors &descriptors,
              cv::Mat *features, cv::Mat *targets) {
  size_t rowIndex = 0;
  int *target = targets->ptr<int>(0);
  for(SamplesDesctiptors::const_iterator descriptor = descriptors.begin();
      descriptor != descriptors.end(); ++descriptor, ++rowIndex) {
    const float *imageFeature =
      GetImageFeatures(descriptor->first).ptr<float>(0);
    float *feature = features->ptr<float>(rowIndex);
    for(size_t colIndex = 0; colIndex < features->cols; ++colIndex) {
      feature[colIndex] = imageFeature[colIndex] / 255.0;
    }
    target[rowIndex] = descriptor->second;
  }
}

CvSVMParams MakeSVMParameters() {
  CvSVMParams parameters(CvSVM::C_SVC,
                         CvSVM::RBF,
                         10, // degree 
                         0.09, // gamma 
                         1, // coef0,
                         10, // c
                         0.5, // nu
                         1, // p,
                         0, // class weights,
                         cv::TermCriteria(cv::TermCriteria::MAX_ITER +
                                          cv::TermCriteria::EPS, // type??
                                          1000, // max iterations
                                          cv::TermCriteria::EPS));
  return parameters;
}

void Learn(const cv::Mat &features, const cv::Mat &targets,
           CvSVM *svm) {
  // const int *target = targets.ptr<int>(0);
  // for(size_t rowIndex = 0; rowIndex < features.rows; ++rowIndex) {
  //     const float *feature = features.ptr<float>(rowIndex);
  //     for(size_t colIndex = 0; colIndex < features.cols; ++colIndex) {
  //       std::cout << feature[colIndex] << ' ';
  //     }
  //     std::cout << std::endl;
  //   std::cout << target[rowIndex] << ' ';
  // } 
  // std::cout << std::endl;

  svm->train(features, targets, cv::Mat(), cv::Mat(), MakeSVMParameters());
  std::cout << "Suppor vectors: " << svm->get_support_vector_count() << std::endl;

}

int Classify(const CvSVM &svm, const cv::Mat &features) {
  // for(size_t vector = 0; vector < svm.get_support_vector_count(); ++vector) {
  //   std::cout << vector << std::endl;
  //   const float *support = svm.get_support_vector(vector);
  //   for(size_t index = 0; index < features.cols; ++index) {
  //     std::cout << support[index] << ' ';
  //   }
  //   std::cout << std::endl;
  // }
 float result = svm.predict(features);
 return result;
}

double Test(const CvSVM &svm, const cv::Mat &features, const cv::Mat &targets) {
  double error = 0;
  for(size_t rowIndex = 0; rowIndex < features.rows; ++rowIndex) {
    const int *target = targets.ptr<int>(rowIndex);
    int predictedTarget = Classify(svm, features.row(rowIndex));
    if(predictedTarget != target[0]) {
      error++;
    }
    // std::cout << predictedTarget << ' ' << target[0] << std::endl;
  }
  return error / features.rows;
  // std::cout << std::endl;
}

void CopyROI(const cv::Mat &source, const cv::Range &sourceRows,
             const cv::Range &sourceCols,
             cv::Mat *target, const cv::Range &targetRows,
             const cv::Range &targetCols) {
  for(size_t sourceRow = sourceRows.start, targetRow = targetRows.start;
      sourceRow != sourceRows.end; ++sourceRow, ++targetRow) {
    const float *sourceFeature = source.ptr<float>(sourceRow);
    float *targetFeature = target->ptr<float>(targetRow);
    for(size_t sourceCol = sourceCols.start, targetCol = targetCols.start;
        sourceCol != sourceCols.end; ++sourceCol, ++targetCol) {
      targetFeature[targetCol] = sourceFeature[sourceCol];
    }
  }
}

double QFoldTest(const cv::Mat &features, const cv::Mat &targets,
             size_t folds) {
  double error = 0;
  size_t foldSize = features.rows / folds;
  for(size_t fold = 0; fold < folds; ++fold) {
    cv::Mat learnFeatures(features.rows - foldSize,
                          features.cols, CV_32FC1);
    cv::Mat learnTargets(targets.rows - foldSize, targets.cols, CV_32SC1);
    if(fold > 0) {
      CopyROI(features, cv::Range(0, fold * foldSize),
              cv::Range(0, features.cols),
              &learnFeatures, cv::Range(0, fold * foldSize),
              cv::Range(0, learnFeatures.cols));
      CopyROI(targets, cv::Range(0, fold * foldSize),
              cv::Range(0, targets.cols),
              &learnTargets, cv::Range(0, fold * foldSize),
              cv::Range(0, learnTargets.cols));

    }
    if(fold < folds) {
      CopyROI(features, cv::Range((fold + 1) * foldSize, features.rows), 
              cv::Range(0, features.cols),
              &learnFeatures, cv::Range(fold * foldSize, learnFeatures.rows), 
              cv::Range(0, learnFeatures.cols));
      CopyROI(targets, cv::Range((fold + 1) * foldSize, targets.rows),
              cv::Range(0, targets.cols),
              &learnTargets, cv::Range(fold * foldSize, learnTargets.rows),
              cv::Range(0, learnTargets.cols));
    }
    
    cv::Mat testFeatures(foldSize, features.cols, CV_32FC1);
    testFeatures = features(cv::Range(fold * foldSize, (fold + 1) * foldSize),
                            cv::Range::all());
    cv::Mat testTargets(foldSize, targets.cols, CV_32SC1);
    testTargets = targets(cv::Range(fold * foldSize, (fold + 1) * foldSize),
                          cv::Range::all());
    CvSVM svm;
    // Learn(learnFeatures, learnTargets, &svm);
    Learn(learnFeatures, learnTargets, &svm);
    error += Test(svm, testFeatures, testTargets);
  }
  return error / folds;
}

int main(int argc, char *argv[]) {
  SamplesDesctiptors descriptors =
    ParseDescriptors(TRAIN_IMAGES + GENDERS_FILENAME);

  cv::Mat features(descriptors.size(), 
                   THUMBNAIL_SIZE.width * THUMBNAIL_SIZE.height,
                   CV_32FC1);
  cv::Mat targets(descriptors.size(), 1, CV_32SC1);
  std::cout << "Parsing data" << std::endl;
  MakeData(descriptors, &features, &targets);
  CvSVM svm;
  std::cout << "Learning" << std::endl;
  Learn(features, targets, &svm);
  std::cout << "Saving model" << std::endl;
  svm.save(GENDERMODEL_FILENAME);
  // std::cout << QFoldTest(features, targets, 10) << std::endl;

  return 0;
}

