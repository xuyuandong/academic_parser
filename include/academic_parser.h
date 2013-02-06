/*
 * academic_parser.h
 *
 *  Created on: 2012-1-11
 *      Author: x
 */

#ifndef ACADEMIC_PARSER_H_
#define ACADEMIC_PARSER_H_

#include <vector>
#include <string>
#include "academic_data.h"
#include "boost/boost/shared_ptr.hpp"

namespace extlibs {
  class Conf;
}

namespace academic {

class AcademicDecoder;
class AcademicPreproc;
class AcademicPostproc;

class AcademicParser {
public:
  AcademicParser(){}
  virtual ~AcademicParser(){}

  // Interface
  void Clear() {
    paper_->Clear();
  }
  const boost::shared_ptr<AcademicPaper> GetAcademicPaper() {
    return paper_;
  }
  virtual int GetHeaderPage(const std::vector<std::string>& pages) = 0;
  virtual int Parse(const std::vector<std::string>& textvec, int headpage, const std::string& title) = 0;

  // Methods
  virtual int ParseHeaderPage(const std::string& text, const std::string& title) = 0;
  virtual int ParseCitationText(const std::vector<std::string>& textvec) = 0;

  virtual int WriteHeaderPage(const std::string& text, const std::string& title, const std::string& name) = 0;
  virtual int WriteCitationText(const std::vector<std::string>& textvec, const std::string& name) = 0;

protected:
  boost::shared_ptr<AcademicDecoder> decoder_;
  boost::shared_ptr<AcademicPreproc> preproc_;
  boost::shared_ptr<AcademicPostproc> postproc_;

  boost::shared_ptr<AcademicPaper> paper_;
};

}

#endif /* ACADEMIC_PARSER_H_ */
