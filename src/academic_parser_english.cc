/*
 * academic_parser_english.cc
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
#include "file/file.h"
#include "base/logging.h"
#include "academic_utils.h"
#include "academic_parser_english.h"
#include "academic_decoder_english.h"
#include "academic_preproc_english.h"
#include "academic_postproc_english.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;

namespace academic {

AcademicParserEnglish::AcademicParserEnglish(Conf* config) {
  paper_.reset(new(nothrow) AcademicPaperEnglish());
  CHECK(paper_.get() != NULL) << "failed to create paper";

  decoder_.reset(new(nothrow) AcademicDecoderEnglish(paper_, config));
  CHECK(decoder_.get() != NULL) << "failed to create decoder";

  preproc_.reset(new(nothrow) AcademicPreprocEnglish(paper_));
  CHECK(preproc_.get() != NULL) << "failed to create pre-processor";

  postproc_.reset(new(nothrow) AcademicPostprocEnglish(paper_));
  CHECK(postproc_.get() != NULL) << "failed to create post-processor";
}

AcademicParserEnglish::~AcademicParserEnglish() {

}

int AcademicParserEnglish::GetHeaderPage(const vector<string>& pages) {
  int idx = preproc_->GetHeaderPage(pages);
  return (idx < 0)? 0 : idx;
}

int AcademicParserEnglish::Parse(const vector<string>& textvec, int headpage, const string& title) {
  paper_->Clear();

  ParseHeaderPage(textvec[headpage], title);
  ParseCitationText(textvec);

  // recall abstract, keywords ...
  if (paper_->abstract_.empty())
    preproc_->RecallSummary(textvec, headpage);

  paper_->Normalize();
  paper_->PrintDebug();

  return 0;
}

int AcademicParserEnglish::ParseHeaderPage(const string& text, const string& title) {
  paper_->title_ = title;

  string header;
  preproc_->FindHeader(text, &header);
  if (header.empty()) {
    LOG(INFO) << "empty header, return";
    return -1;
  }

  if (!title.empty()) {
    int cutoff = std::min(25, (int)title.size());
    string topstr = title.substr(0, cutoff);

    int toppos = AcademicUtils::NormalMatch(header, topstr);
    if (toppos >= 0)
      header = header.substr(toppos, header.size());
  }

  decoder_->DecodeHeader(header);

  postproc_->ProcessHeader();  // format paper

  return 0;
}

int AcademicParserEnglish::ParseCitationText(const vector<string>& textvec) {
  string citation;
  preproc_->FindCitation(textvec, &citation);
  if (citation.empty()) {
    LOG(INFO) << "empty citation, return";
    return -1;
  }

  vector<string> items;
  preproc_->SplitCitation(citation, &items);

  decoder_->DecodeCitation(items);

  postproc_->ProcessCitation();

  return 0;
}

int AcademicParserEnglish::WriteHeaderPage(const string& text, const string& title, const string& name) {
  string header;
  preproc_->FindHeader(text, &header);
  if (header.empty()) {
    LOG(INFO) << "empty header, return";
    return -1;
  }
  fprintf(stdout, "rawheader:\n%s\n", header.c_str());

  int cutoff = std::min(25, (int)title.size());
  string topstr = title.substr(0, cutoff);

  int toppos = AcademicUtils::NormalMatch(header, topstr);
  if (toppos >= 0)
    header = header.substr(toppos, header.size());
  fprintf(stdout, "TITLE<%d>: %s\n", toppos, title.c_str());
  fprintf(stdout, "header:\n%s\n", header.c_str());

  string::size_type pos = name.rfind(".");
  string fname = name.substr(0, pos);
  fname.append(".txt");
  if (!file::File::WriteStringToFile(header, fname)) {
    LOG(INFO) << "failed to write sample file";
    return -1;
  }
  return 0;
}

int AcademicParserEnglish::WriteCitationText(const vector<string>& textvec, const string& name) {
  string citation;
  preproc_->FindCitation(textvec, &citation);
  if (citation.empty()) {
    LOG(INFO) << "empty citation, return";
    return -1;
  }
  fprintf(stdout, "citation:\n%s\n", citation.c_str());

  vector<string> items;
  preproc_->SplitCitation(citation, &items);

  string references;
  for (int i = 0; i < items.size(); ++i)
    references.append(items[i] + "\n\n");

  string::size_type pos = name.rfind(".");
  string fname = name.substr(0, pos);
  fname.append(".txt");
  if (!file::File::WriteStringToFile(references, fname)) {
    LOG(INFO) << "failed to write sample file";
    return -1;
  }
  return 0;
}



}


