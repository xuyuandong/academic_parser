/*
 * academic_wrapper.h
 *
 *  Created on: 2012-2-13
 *      Author: x
 */

#ifndef ACADEMIC_WRAPPER_H_
#define ACADEMIC_WRAPPER_H_

#include <map>
#include <string>
#include <vector>
#include "third_party/boost/boost/shared_ptr.hpp"
#include "page_analysis/langrecognizer/public/LangRecognizer.h"

namespace indexing {
  class MergedDoc;
}

namespace pdfconvert {
  class PdfParser;
}

namespace academic {

class AcademicParser;

class AcademicWrapper {
public:
  explicit AcademicWrapper(const char* conf);
  ~AcademicWrapper();

  bool ParseMergeddoc(indexing::MergedDoc* doc, langrecognizer::LangType lang);

private:
  int GetPageText(const std::string& src_doc, std::vector<std::string>* textvec);
  int GetTitle(const std::string& src_doc, int idx, std::string* title);
  void FormatHtml(const std::string& title, const std::string& body, std::string* html);

  boost::shared_ptr<pdfconvert::PdfParser> pdfreader_;
  boost::shared_ptr<pdfconvert::PdfParser> rawreader_;
  std::map<langrecognizer::LangType, boost::shared_ptr<AcademicParser> > parsermap_;
};


}


#endif /* ACADEMIC_WRAPPER_H_ */
