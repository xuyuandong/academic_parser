/*
 * academic_crfpp.h
 *
 *  Created on: 2012-1-16
 *      Author: x
 */

#ifndef ACADEMIC_CRFPP_H_
#define ACADEMIC_CRFPP_H_

#include <string>

namespace CRFPP {
class Tagger;
}

namespace academic {

class AcademicCrfpp {
public:
  AcademicCrfpp(int argc, char* argv[]);
  ~AcademicCrfpp();

  void Parse(const std::string& input, std::string* output);

private:
  CRFPP::Tagger* tagger_;
};


}


#endif /* ACADEMIC_CRFPP_H_ */
