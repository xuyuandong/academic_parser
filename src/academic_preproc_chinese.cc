/*
 * academic_preproc_chinese.cc
 *
 *  Created on: 2012-1-12
 *      Author: x
 */

#include <set>
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
#include "base/logging.h"
#include "ext_libs/search/public/mmatch.h"
#include "academic_dict.h"
#include "academic_data.h"
#include "academic_utils.h"
#include "academic_regex.h"
#include "academic_preproc_chinese.h"

using namespace std;
using namespace extlibs;  // NOLINT

namespace academic {

AcademicPreprocChinese::AcademicPreprocChinese(boost::shared_ptr<AcademicPaper> paper)
  : AcademicPreproc(paper) {

}

AcademicPreprocChinese::~AcademicPreprocChinese() {

}

int AcademicPreprocChinese::GetHeaderPage(const vector<string>& pages) {
  return 0;
}

int AcademicPreprocChinese::FindHeader(const string& text, string* header) {
  const int IDX_ABSTRACT = AcademicDict::GetHeaderTag("abstract");
  const int IDX_KEYWORDS = AcademicDict::GetHeaderTag("keywords");

  int ret = SearchDictionary(AcademicDict::ZhHeaderDict(), text);
  if (ret < 0) {
    *header = text;
    return -1;
  }

  int offset = keyposList_[0].poff_;
  *header = text.substr(0, offset);

  int cur = 0, num = keyposList_.size();
  for (int i = cur + 1; i <= num; ++i) {
    int cur_prop = keyposList_[cur].prop_;
    if (i < num && keyposList_[i].prop_ == cur_prop) {
      continue;
    }

    int beg = keyposList_[cur].poff_;
    int end = (i < num) ? keyposList_[i].poff_ : text.size();

    if (cur_prop == IDX_ABSTRACT) {  // abstract
      paper_->abstract_ = text.substr(beg, end - beg);
      // LOG(INFO) << "Abstract " << text.substr(beg, end - beg);
    } else if (cur_prop == IDX_KEYWORDS) {  // key words
      paper_->keywords_.push_back(text.substr(beg, end - beg));
      // LOG(INFO) << "Keywords " << text.substr(beg, end - beg);
    }

    cur = i;
  }

  return 0;
}

int AcademicPreprocChinese::RecallSummary(const vector<string>& textvec, int headpage) {
  return 0;
}

int AcademicPreprocChinese::FindCitation(const vector<string>& textvec, string* citation) {
  int pages = textvec.size();
  int beg = -1, end = -1;

  // locate citation beginning
  int idx = pages - 1;
  for (; idx > 0; --idx) {
    int ret = SearchDictionary(AcademicDict::ZhCitationDict(), textvec[idx]);
    if (ret < 0)
      continue;

    int cur = 0, num = keyposList_.size();
    for (int i = cur + 1; i <= num; ++i) {
      int cur_prop = keyposList_[cur].prop_;
      /*if (i < num && cur_prop == 1 && keyposList_[i].prop_ == cur_prop)
        continue;
      */
      if (cur_prop == 1) {  // reference
        beg = keyposList_[cur].poff_ + keyposList_[cur].keylen_;
        end = (i < num) ? keyposList_[i].poff_ : textvec[idx].size();
        citation->append(textvec[idx].substr(beg, end - beg));
      }
      cur = i;
    }

    if (beg >= 0)
      break;
  }

  //fprintf(stdout, "BegCit:%s\n", textvec[idx].c_str());
  if (beg < 0 || 2*idx < pages)
    return -1; // no reference or reference is too ahead

  // locate citation ending
  if (end == textvec[idx].size()) {
    // not found cut off keywords
    int i = idx + 1;
    int maxpages = std::min(pages, idx + 4);
    for (; i < maxpages; ++i) {
      int ret = SearchDictionary(AcademicDict::ZhCitationDict(), textvec[i]);
      if (ret < 0 || keyposList_.empty()) {
        citation->append("\n" + textvec[i]);
        continue;
      }
      end = keyposList_[0].poff_;
      citation->append("\n" + textvec[i].substr(0, end));
      break;
    }
  }

  return 0;
}

int AcademicPreprocChinese::SplitCitation(const string& citation, vector<string>* items) {
  //fprintf(stdout, "citation:\n%s\n", citation.c_str());
  CiteMarker marker = GuessMarkerType(citation);

  if (marker == UNKNOWN) {
    SplitUnmarkedCitation(citation, items);
  } else {
    SplitCitationByMarker(citation, marker, items);
  }

  /*for (int i = 0; i < items->size(); ++i) {
    fprintf(stdout, "R: %s\n\n", items->at(i).c_str());
  }*/

  return 0;
}

// ===================================================================================

CiteMarker AcademicPreprocChinese::GuessMarkerType(const string& text) {
  int count = 0;
  int maxcount = 0;
  CiteMarker marker = UNKNOWN;

  if ((count = AcademicRegex::CountZh(SQUARE, text)) > maxcount) {
    maxcount = count;
    marker = SQUARE;
  }

  int lines = 0;
  string::size_type beg = 0;
  string::size_type end = text.find('\n', beg);
  while (end != string::npos) {
    if (end - beg >= 10)
      lines ++;
    beg = end + 1;
    end = text.find('\n', beg);
  }

  //fprintf(stdout, "Marker: %d, Maxcount: %d, Lines: %d\n", marker, maxcount, lines);
  if (maxcount > std::max(2, lines/6))
    return marker;

  return UNKNOWN;
}

void AcademicPreprocChinese::SplitCitationByMarker(const string& citation, enum CiteMarker marker, vector<string>* items) {
  pair<int, int> position = AcademicRegex::SearchZh(marker, citation);

  for(;;) {
    // current reference
    string text;
    int beg = position.second - 3; // 3 is defined in academic_regex.cc
    while(ispunct(citation[beg]))
      beg ++;
    int end = std::max(beg, position.second);

    // search next reference
    position = AcademicRegex::SearchZh(marker, citation, end);

    // current cut off
    int cutoff = position.first;
    if (cutoff< 0)
      cutoff = citation.size();

    // collect current
    while ((end = citation.find('\n', beg)) != string::npos) {
      if (end > cutoff)
        break;

      // append line to reference
      text.append(citation.substr(beg, end - beg) + " ");
      
      // update to search next line
      beg = end + 1; 
    }
    items->push_back(text);

    // not found next reference, break
    if (position.first < 0)
      break;
  }
}

void AcademicPreprocChinese::SplitUnmarkedCitation(const string& citation, vector<string>* items) {

}

}

