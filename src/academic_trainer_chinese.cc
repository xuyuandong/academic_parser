/*
 * academic_trainer.cc
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#include "base/logging.h"
#include "word_segmentor/public/gswssegmenter.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_postagger.h"
#include "academic_trainer_chinese.h"
#include "academic_header_chinese.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;
using namespace segmenter;

namespace academic {

AcademicTrainerChinese::AcademicTrainerChinese(Conf* config) {
  char value[WORD_SIZE + 1];
  *value = '\0';

  // word segmenter
  int ret = config->ConfString(const_cast<char*>("WordSegmentDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find WordSegmentDictPath in config";

  wordseg_.reset(new(nothrow) GswsSegmenter("gsws", value));
  CHECK(wordseg_.get() != NULL) << "failed to create word segmenter";
  CHECK(wordseg_->Init()) << "failed to initialize word segmenter";

  // pos tagger
  ret = config->ConfString(const_cast<char*>("PosTaggerDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find PosTaggerDictPath in config";

  postag_.reset(new(nothrow) PosTagger(value));
  CHECK(postag_.get() != NULL) << "failed to create pos tagger";
}

int AcademicTrainerChinese::TrainHeader(const string& text, FILE* fp) {
  ChineseHeaderLine feature;
  feature.seg_ = wordseg_;
  feature.tag_ = postag_;

  string featstr;
  int beg = 0, end = 0;
  while ((beg = text.find("<", beg)) != string::npos
      && (end = text.find(">", beg)) != string::npos) {
    string word = text.substr(beg + 1, end - beg - 1);
    if (AcademicDict::GetHeaderTag(word) < 0) {
      LOG(ERROR) << "unexpected tag " << word;
      return -1;
    }
    
    beg = end + 1;
    string mark = "</" + word + ">";
    end = text.find(mark, beg);
    if (end == string::npos) {
      LOG(ERROR) << "unexpected mark " << mark;
      return -1;
    }
    string content = text.substr(beg, end - beg);
   

    feature.label_ = word;
    feature.SetText(content);
    feature.Tokenize(&featstr);

    beg = end + mark.size();
  }

  fprintf(fp, "%s\n\n", featstr.c_str());
  return 0;
}
    
int AcademicTrainerChinese::TrainCitation(const string& text, FILE* fp) {
  ChineseHeaderLine feature;
  feature.seg_ = wordseg_;
  feature.tag_ = postag_;

  vector<string> lines;
  AcademicUtils::SplitText(text, "\n", &lines);

  for (int i = 0; i < lines.size(); ++i) {
    fprintf(stdout, "line:%d\n", i);
    fflush(stdout);
    if (lines[i].size() < 10)
      continue;

    string featstr;

    int beg = 0, end = 0;
    while ((beg = lines[i].find("<", beg)) != string::npos
        && (end = lines[i].find(">", beg)) != string::npos) {
      string word = lines[i].substr(beg + 1, end - beg - 1);
      if (AcademicDict::GetCitationTag(word) < 0) {
        LOG(ERROR) << "unexpected tag " << word;
        return -1;
      }

      beg = end + 1;
      string mark = "</" + word + ">";
      end = lines[i].find(mark, beg);
      if (end == string::npos) {
        LOG(ERROR) << "unmatched mark " << word;
        return -1;
      }
      string content = lines[i].substr(beg, end - beg);

      feature.label_ = word;
      feature.SetText(content);
      feature.Tokenize(&featstr);
    
      beg = end + mark.size();
    }

    fprintf(fp, "%s\n\n", featstr.c_str());
  }

  return 0;
}


}

