/*
 * academic_parser_chinese.cc
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
#include "academic_parser_chinese.h"
#include "academic_decoder_chinese.h"
#include "academic_preproc_chinese.h"
#include "academic_postproc_chinese.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;

namespace academic {

AcademicParserChinese::AcademicParserChinese(Conf* config) {
  paper_.reset(new(nothrow) AcademicPaperChinese());
  CHECK(paper_.get() != NULL) << "failed to create paper";

  decoder_.reset(new(nothrow) AcademicDecoderChinese(paper_, config));
  CHECK(decoder_.get() != NULL) << "failed to create decoder";

  preproc_.reset(new(nothrow) AcademicPreprocChinese(paper_));
  CHECK(preproc_.get() != NULL) << "failed to create pre-processor";

  postproc_.reset(new(nothrow) AcademicPostprocChinese(paper_));
  CHECK(postproc_.get() != NULL) << "failed to create post-processor";
}

AcademicParserChinese::~AcademicParserChinese() {

}

int AcademicParserChinese::GetHeaderPage(const vector<string>& pages) {
  return 0;
}

int AcademicParserChinese::Parse(const vector<string>& textvec, int headpage, const string& title) {
  paper_->Clear();

  string normtitle;
  AcademicUtils::NormalizeTextZh(title, &normtitle);

  string text;
  vector<string> normvec;
  for (int i = 0; i < textvec.size(); ++i) {
    AcademicUtils::NormalizeTextZh(textvec[i], &text);
    normvec.push_back(text);
  }

  ParseHeaderPage(normvec[headpage], normtitle);
  ParseCitationText(normvec);

  paper_->Normalize();
  paper_->PrintDebug();

  return 0;
}

int AcademicParserChinese::ParseHeaderPage(const string& text, const string& title) {
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

  int margin = AcademicUtils::GetChinaEuropeMargin(header);
  header = header.substr(0, margin);
  //fprintf(stdout, "\n<Header>:\n%s\n\n", header.c_str());

  decoder_->DecodeHeader(header);

  postproc_->ProcessHeader();  // format paper

  return 0;
}

int AcademicParserChinese::ParseCitationText(const vector<string>& textvec) {
  string citation;
  preproc_->FindCitation(textvec, &citation);
  if (citation.empty()) {
    LOG(INFO) << "empty citation, return";
    return -1;
  }

  AcademicUtils::MergeWordsByDash(&citation);

  vector<string> items;
  preproc_->SplitCitation(citation, &items);

  decoder_->DecodeCitation(items);

  postproc_->ProcessCitation();

  return 0;
}

int AcademicParserChinese::WriteHeaderPage(const string& text, const string& title, const string& name) {
  string normtitle, normtext;
  AcademicUtils::NormalizeTextZh(title, &normtitle);
  AcademicUtils::NormalizeTextZh(text, &normtext);

  string header;
  preproc_->FindHeader(normtext, &header);
  if (header.empty()) {
    LOG(INFO) << "empty header, return";
    return -1;
  }

  int cutoff = std::min(25, (int)normtitle.size());
  string topstr = normtitle.substr(0, cutoff);

  int toppos = AcademicUtils::NormalMatch(header, topstr);
  if (toppos >= 0)
    header = header.substr(toppos, header.size());
  fprintf(stdout, "TITLE<%d>: %s\n", toppos, normtitle.c_str());

  fprintf(stdout, "Header:\n%s\n", header.c_str());
  int margin = AcademicUtils::GetChinaEuropeMargin(header);
  header = header.substr(0, margin);
  fprintf(stdout, "MHeader:\n%d\n%s\n", margin, header.c_str());

  string fname(name);
  fname.replace(fname.rfind("."), 4, ".txt");

  if (!file::File::WriteStringToFile(header, fname)) {
    LOG(INFO) << "failed to write sample file";
    return -1;
  }
  return 0;
}

int AcademicParserChinese::WriteCitationText(const vector<string>& textvec, const string& name) {
  string text;
  vector<string> normvec;
  for (int i = 0; i < textvec.size(); ++i) {
    AcademicUtils::NormalizeTextZh(textvec[i], &text);
    normvec.push_back(text);
  }

  string citation;
  preproc_->FindCitation(normvec, &citation);
  if (citation.empty()) {
    LOG(INFO) << "empty citation, return";
    return -1;
  }
  fprintf(stdout, "citation:\n%s\n", citation.c_str());

  AcademicUtils::MergeWordsByDash(&citation);

  vector<string> items;
  preproc_->SplitCitation(citation, &items);

  string references;
  for (int i = 0; i < items.size(); ++i)
    references.append(items[i] + "\n\n");

  string fname(name);
  fname.replace(fname.rfind("."), 4, ".txt");

  if (!file::File::WriteStringToFile(references, fname)) {
    LOG(INFO) << "failed to write sample file";
    return -1;
  }

  return 0;
}

}


