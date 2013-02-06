/*
 * academic_data.h
 *
 *  Created on: 2012-1-13
 *      Author: x
 */

#ifndef ACADEMIC_DATA_H_
#define ACADEMIC_DATA_H_

#include <vector>
#include <string>

namespace academic {

class Author {
public:
  std::string name_;
  std::string affiliate_;
  std::string email_;
};

class Citation {
public:
  std::string title_;
  std::string date_;
  std::string publication_;
  std::vector<std::string> names_;
};

class AcademicPaper {
public:
  virtual ~AcademicPaper(){}

  void Clear();
  void PrintDebug();

  virtual void Normalize() = 0;

  int quality_;

  std::string title_;
  std::string abstract_;
  std::vector<std::string> keywords_;

  std::vector<Author> authors_;
  std::vector<Citation> citations_;

  // internal module data, should not be accessed by external call
  std::string crf_str_;
  std::vector<std::string> crf_vec_;
};

class AcademicPaperEnglish : public AcademicPaper {
public:
  virtual void Normalize();
};

class AcademicPaperChinese : public AcademicPaper {
public:
  virtual void Normalize();
};

}


#endif /* ACADEMIC_DATA_H_ */
