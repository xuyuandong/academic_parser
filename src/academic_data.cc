/*
 * academic_data.cc
 *
 *  Created on: 2012-1-19
 *      Author: x
 */

#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include "academic_data.h"
#include "academic_utils.h"

using namespace std;

namespace academic {

void AcademicPaper::Clear() {
  title_.clear();
  abstract_.clear();
  keywords_.clear();

  authors_.clear();
  citations_.clear();

  crf_str_.clear();
  crf_vec_.clear();
}

void AcademicPaper::PrintDebug() {
  fprintf(stdout, "Title: %s\n", title_.c_str());

  fprintf(stdout, "Author:\n");
  for (int i = 0; i < authors_.size(); ++i)
    fprintf(stdout, "<%s> <%s> <%s>\n", authors_[i].name_.c_str(),
        authors_[i].affiliate_.c_str(), authors_[i].email_.c_str());

  fprintf(stdout, "Abstract: %s\n", abstract_.c_str());

  fprintf(stdout, "Keywords: ");
  for (int i = 0; i < keywords_.size(); ++i)
    fprintf(stdout, "<%s> ", keywords_[i].c_str());
  fprintf(stdout, "\n");

  fprintf(stdout, "Citation:\n");
  for (int i = 0; i < citations_.size(); ++i) {
    fprintf(stdout, "[%d]", i);
    for (int k = 0; k < citations_[i].names_.size(); ++k)
      fprintf(stdout, "<%s> ", citations_[i].names_[k].c_str());
    fprintf(stdout, "<%s> <%s> <%s>\n", citations_[i].title_.c_str(),
        citations_[i].date_.c_str(), citations_[i].publication_.c_str());
  }

  fflush(stdout);
}

void AcademicPaperEnglish::Normalize() {
  // keywords
  if (!keywords_.empty()) {
    vector<string> words;
    AcademicUtils::MergeLinesByDash(&keywords_[0]);
    AcademicUtils::SplitText2(keywords_[0], ",;\n", &words);

    int endidx = 0, wordnum = words.size();
    for (; endidx < wordnum; ++endidx) {
      if (words[endidx].find("1.") != string::npos)
        break;
      if (!AcademicUtils::RemoveBegEndPunc(words[endidx], &words[endidx]))
        break;
    }

    while(endidx < wordnum) {
      words.pop_back();
      endidx = endidx + 1;
    }

    keywords_.swap(words);
  }

  // abstract
  AcademicUtils::MergeLinesByDash(&abstract_);
}

void AcademicPaperChinese::Normalize() {
  // keywords
  if (!keywords_.empty()) {
    vector<string> words;
    AcademicUtils::SplitText2(keywords_[0], ",;\n", &words);

    int maxnum = 0;
    keywords_.clear();
    for (int i = 0; i < words.size(); ++i) {
      maxnum += words[i].size();
      keywords_.push_back(words[i]);
      if (maxnum > MAX_ZH_KEYWORDS_WORD*3)
        break;
    }
  }
}

}
