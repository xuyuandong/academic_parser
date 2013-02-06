/*
 * academic_trainer.cc
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#include "base/logging.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_trainer_english.h"
#include "academic_header_english.h"
#include "academic_reference_english.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;

namespace academic {

AcademicTrainerEnglish::AcademicTrainerEnglish(Conf* config) {

}

int AcademicTrainerEnglish::TrainHeader(const string& text, FILE* fp) {
  vector<string> lines;
  AcademicUtils::SplitText(text, "\n", &lines);

  vector<HeaderLine> linefeats;
  linefeats.reserve(lines.size());

  for (int i = 0; i < lines.size(); ++i) {
    bool linebeg = true;
    bool lineend = false;

    int beg = 0, end = 0;
    while ((beg = lines[i].find("<", beg)) != string::npos
        && (end = lines[i].find(">", beg)) != string::npos) {
      string word = lines[i].substr(beg + 1, end - beg - 1);
      if (AcademicDict::GetHeaderTag(word) < 0) {
        LOG(ERROR) << "unexpected tag " << word;
        return -1;
      }

      beg = end + 1;
      string mark = "</" + word + ">";
      end = lines[i].find(mark, beg);
      if (end == string::npos) {
        LOG(ERROR) << "unexpected mark " << mark;
        return -1;
      }
      string content = lines[i].substr(beg, end - beg);

      if (end + mark.size() == lines[i].size())
        lineend = true;

      linefeats.push_back(HeaderLine());
      HeaderLine& linefeat = linefeats.back();
      linefeat.label_ = word;
      linefeat.linebeg_ = linebeg;
      linefeat.lineend_ = lineend;
      linefeat.SetText(content);

      if (linebeg)
        linebeg = false;

      beg = end + mark.size();
    }
  }

  string feature_str;
  for (int i = 0; i < linefeats.size(); ++i) {
    linefeats[i].Tokenize(&feature_str);
  }
  fprintf(fp, "%s\n", feature_str.c_str());
  fprintf(fp, "\n");

  return 0;
}

int AcademicTrainerEnglish::TrainCitation(const string& text, FILE* fp) {
  vector<string> lines;
  AcademicUtils::SplitText(text, "\n", &lines);

  vector<ReferenceLine> rlines;
  rlines.reserve(lines.size());

  for (int i = 0; i < lines.size(); ++i) {
    if (lines[i].size() < 10)
      continue;

    string feature_str;
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

      rlines.push_back(ReferenceLine());
      ReferenceLine& rline = rlines.back();
      rline.label_ = word;
      rline.SetText(content);
      rline.Tokenize(&feature_str);

      beg = end + mark.size();
    }

    if (beg != string::npos) {
      LOG(ERROR) << "invalid line " << i;
      return -1;
    }
    fprintf(fp, "%s\n", feature_str.c_str());
  }

  fprintf(fp, "\n");

  return 0;
}

}

