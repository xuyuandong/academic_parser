/*
 * academic_factory.cc
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#include "base/logging.h"
#include "boost/boost/shared_ptr.hpp"
#include "academic_factory.h"
#include "academic_parser_english.h"
#include "academic_parser_chinese.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;
using namespace langrecognizer;

namespace academic {

boost::shared_ptr<AcademicParser> AcademicFactory::GetParser(LangType lang, Conf* config) {
  boost::shared_ptr<AcademicParser> parser_;

  switch (lang) {
  case LANG_ENGLISH:
    parser_.reset(new(nothrow) AcademicParserEnglish(config));
    break;
  case LANG_CHINESE:
    parser_.reset(new(nothrow) AcademicParserChinese(config));
    break;
  default:
    break;
  }

  CHECK(parser_.get() != NULL) << "Failed to create academic parser";

  return parser_;
}


}
