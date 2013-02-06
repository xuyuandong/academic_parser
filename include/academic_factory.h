/*
 * academic_factory.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_FACTORY_H_
#define ACADEMIC_FACTORY_H_

#include "boost/boost/shared_ptr.hpp"
#include "langrecognizer/public/LangRecognizer.h"
#include "academic_parser.h"

namespace extlibs {
  class Conf;
}

namespace academic {

class AcademicFactory {
 public:
  AcademicFactory() {}
  ~AcademicFactory() {}

  static boost::shared_ptr<AcademicParser> GetParser(langrecognizer::LangType lang, extlibs::Conf* config);
};


}


#endif /* ACADEMIC_FACTORY_H_ */
