/*
 * academic_parser_english.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_PARSER_ENGLISH_H_
#define ACADEMIC_PARSER_ENGLISH_H_

#include "academic_parser.h"

namespace extlibs {
  class Conf;
}

namespace academic {

class AcademicParserEnglish : public AcademicParser {
public:
  explicit AcademicParserEnglish(extlibs::Conf* config);
  virtual ~AcademicParserEnglish();

  // Interface
  virtual int GetHeaderPage(const std::vector<std::string>& pages);
  virtual int Parse(const std::vector<std::string>& textvec, int headpage, const std::string& title);

  // Methods
  virtual int ParseHeaderPage(const std::string& text, const std::string& title);
  virtual int ParseCitationText(const std::vector<std::string>& textvec);

  virtual int WriteHeaderPage(const std::string& text, const std::string& title, const std::string& name);
  virtual int WriteCitationText(const std::vector<std::string>& textvec, const std::string& name);
};

}


#endif /* ACADEMIC_PARSER_ENGLISH_H_ */
