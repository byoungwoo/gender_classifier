#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>

class CameraException : public std::exception {
public:
  CameraException();

  virtual const char* what() const throw();
};

#endif /* _EXCEPTIONS_H_ */
