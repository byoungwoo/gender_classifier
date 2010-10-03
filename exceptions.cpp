#include "exceptions.h"

CameraException::CameraException() {

}

const char* CameraException::what() const throw() {
  return "Camera was enable to start";
}
