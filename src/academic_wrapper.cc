/*
 * academic_wrapper.cc
 *
 *  Created on: 2012-2-13
 *      Author: x
 */


#include <string>
#include "base/logging.h"
#include "base/thrift.h"
#include "boost/boost/shared_ptr.hpp"
#include "pdfconvert/public/pdf_parser.h"
#include "proto/gen-cpp/mergeddoc_types.h"
#include "proto/gen-cpp/scholar_mergeddoc_types.h"
#include "academic_wrapper.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_regex.h"
#include "academic_parser.h"
#include "academic_factory.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace base;
using namespace extlibs;
using namespace scholar;
using namespace indexing;
using namespace pdfconvert;
using namespace langrecognizer;

namespace academic {

AcademicWrapper::AcademicWrapper(const char* conf) {
  Conf config;
  CHECK(config.ReadConf(conf) > 0) << "Failed to read config file";

  AcademicRegex::Init();
  AcademicDict::Init(&config);

  parsermap_[LANG_ENGLISH] = AcademicFactory::GetParser(LANG_ENGLISH, &config);
  parsermap_[LANG_CHINESE] = AcademicFactory::GetParser(LANG_CHINESE, &config);

  rawreader_.reset(new(nothrow) PdfParser(true));
  CHECK(rawreader_.get() != NULL) << "Failed to create pdf raw reader";

  pdfreader_.reset(new(nothrow) PdfParser(false));
  CHECK(pdfreader_.get() != NULL) << "Failed to create pdf fragment reader";
}


AcademicWrapper::~AcademicWrapper() {

}

bool AcademicWrapper::ParseMergeddoc(MergedDoc* doc, LangType lang) {
  string title, content;
  vector<string> textvec;

  boost::shared_ptr<AcademicParser> acparser = parsermap_[lang];

  GetPageText(doc->content, &textvec);
  for (int i = 0; i < textvec.size(); ++i)
    content.append(textvec[i]);
  if (content.empty())
    return false;

  // location header
  int idx = acparser->GetHeaderPage(textvec);
  GetTitle(doc->content, idx + 1, &title);

  // parse header and citation
  acparser->Parse(textvec, idx, title);

  // get result
  const boost::shared_ptr<AcademicPaper> paper = acparser->GetAcademicPaper();

  scholar::PaperMergedDoc paper_doc;

  // basic info
  paper_doc.paper_language = lang;

  paper_doc.content = content;
  paper_doc.title = paper->title_;
  paper_doc.summary = paper->abstract_;
  paper_doc.keywords = paper->keywords_;

  // author info
  scholar::Author author;
  for (int i = 0; i < paper->authors_.size(); ++i) {
    author.name = paper->authors_[i].name_;
    author.organization = paper->authors_[i].affiliate_; // TODO:
    author.email = paper->authors_[i].email_; // TODO:
    paper_doc.authors.push_back(author);
  }

  // citation info
  scholar::Reference reference;
  for (int i = 0; i < paper->citations_.size(); ++i) {
    reference.title = paper->citations_[i].title_;
    reference.authors = paper->citations_[i].names_;
    reference.pub_year = paper->citations_[i].date_;
    reference.publication.name = paper->citations_[i].publication_;
    paper_doc.references.push_back(reference);
  }

  // pdf meta info // TODO:

  // merged doc info
  FormatHtml(paper_doc.title, paper_doc.content, &doc->content);

  // serialization
  ThriftObjWriter thrift_writer;
  string str_paper_doc;
  thrift_writer.FromThriftToString(paper_doc, &str_paper_doc);
  doc->news_property = str_paper_doc;
  doc->__isset.news_property = true;

  return true;
}

int AcademicWrapper::GetPageText(const string& src_doc, vector<string>* textvec) {
  int ret = rawreader_->LoadDoc(src_doc.data(), src_doc.size());
  if (ret < 0) {
    LOG(INFO) << "FAILED to parse pdf doc";
    return -1;
  }

  int num = rawreader_->GetPageNum();
  for (int i = 1; i <= num; ++i) {
    rawreader_->CleanPage();
    rawreader_->GetPageInfo(i, NULL, false);

    string text;
    rawreader_->GetText(&text);
    textvec->push_back(text);
  }

  rawreader_->CleanDoc();
  return 0;
}

int AcademicWrapper::GetTitle(const string& src_doc, int idx, string* title) {
  int ret = pdfreader_->LoadDoc(src_doc.data(), src_doc.size());
  if (ret < 0) {
    LOG(INFO) << "FAILED to parse pdf doc";
    return -1;
  }

  PDFPageInfo info;
  pdfreader_->GetPageInfo(idx, &info, true);
  info.GetTitle(title);

  pdfreader_->CleanDoc();
  return 0;
}

void AcademicWrapper::FormatHtml(const string& title, const string& body, string* html) {
  html->clear();

  html->append("<html>\n<head>\n<title>");

  html->append(title);

  html->append("</title>\n</head>\n<body>\n");

  html->append(body);

  html->append("\n</body>\n</html>");
}

}

