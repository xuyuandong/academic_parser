/*
 * academic_preproc_english.cc
 *
 *  Created on: 2012-2-29
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
#include "page_analysis/academic_parser/internal/academic_dict.h"
#include "page_analysis/academic_parser/internal/academic_data.h"
#include "page_analysis/academic_parser/internal/academic_utils.h"
#include "page_analysis/academic_parser/internal/academic_regex.h"
#include "academic_preproc_english.h"

using namespace std;
using namespace extlibs;  // NOLINT

namespace academic {

AcademicPreprocEnglish::AcademicPreprocEnglish(boost::shared_ptr<AcademicPaper> paper)
  : AcademicPreproc(paper) {

}

AcademicPreprocEnglish::~AcademicPreprocEnglish() {

}

int AcademicPreprocEnglish::GetHeaderPage(const vector<string>& pages) {
  const int num = std::min(3, (int)pages.size());
  mm_pack_t* ppack = mm_pack_create(MAX_LEMMA_NUM);

  int idx = -1;
  int keynum = 0;
  int maxkeynum = 0;

  set<int> keyset;
  for (int i = 0; i < num; ++i) {
    int ret = mm_search(AcademicDict::EnHeaderDict(), ppack, pages[i].c_str(),
        pages[i].size());
    if (ret < 0) continue;

    bool flag = true;
    keyset.clear();
    for (int k = 0; k < ppack->ppseg_cnt; ++k) {
      int property = ppack->ppseg[k]->prop;
      if (property < MAX_HEADER_PROP)
        keyset.insert(property);

      // abstract/introduction is not on the top of the page
      if (ppack->poff[k] < MIN_HEADER_SIZE)
        flag = false;
    }

    //fprintf(stdout, "page: %d, num: %d, flag: %d\n", i, (int)keyset.size(), flag);
    if (idx < 0 || (keyset.size() > maxkeynum && !keynum && flag)) {
      idx = i;
      keynum = keyset.size();
    }
    if (keyset.size() > maxkeynum)
      maxkeynum = keyset.size();
  }

  mm_pack_del(ppack);
  return idx;
}

int AcademicPreprocEnglish::FindHeader(const string& text, string* header) {
  const int IDX_ABSTRACT = AcademicDict::GetHeaderTag("abstract");
  const int IDX_KEYWORDS = AcademicDict::GetHeaderTag("keywords");
  const int IDX_INTRODUCTION = AcademicDict::GetHeaderTag("introduction");

  int ret = SearchDictionary(AcademicDict::EnHeaderDict(), text);
  if (ret < 0) {
    *header = text;
    return -1;
  }

  // filter header text
  int offset = keyposList_[0].poff_;
  *header = text.substr(0, offset);

  // format other structure
  int maxid = 0, lastid = 0;
  int cur = 0, num = keyposList_.size();
  for (int i = cur + 1; i <= num; ++i) {
    int cur_prop = keyposList_[cur].prop_;
    if (i < num && keyposList_[i].prop_ == cur_prop)
      continue;

    if (maxid < cur_prop)
      maxid = cur_prop;

    int beg = keyposList_[cur].poff_ + keyposList_[cur].keylen_;
    int end = (i < num) ? keyposList_[i].poff_ : text.size();

    if (cur_prop == IDX_ABSTRACT) {  // abstract
      if (paper_->abstract_.empty()) {
        lastid = IDX_ABSTRACT;
        paper_->abstract_ = text.substr(beg, end - beg);
      }
    } else if (cur_prop ==  IDX_KEYWORDS) {  // key words
      if (paper_->keywords_.empty()) {
        lastid = IDX_KEYWORDS;
        paper_->keywords_.push_back(text.substr(beg, end - beg));
      }
    }

    cur = i;
  }

  // if introduction is not found, cut off the last word segment
  if (maxid < IDX_INTRODUCTION) {
    if (lastid == IDX_ABSTRACT) {  // abstract
      if (paper_->abstract_.size() > MAX_ABSTRACT_SIZE)
        paper_->abstract_ = paper_->abstract_.substr(0, MAX_ABSTRACT_SIZE);
    }
    else if (lastid == IDX_KEYWORDS) {  // keywords
      if (paper_->keywords_.size() > MAX_KEYWORDS_SIZE) {
        int pos1 = paper_->keywords_[0].find(" ", MAX_KEYWORDS_SIZE);
        int pos2 = paper_->keywords_[0].find("\n", MAX_KEYWORDS_SIZE);
        int pos = std::max(MAX_KEYWORDS_SIZE, std::min(pos1, pos2));
        paper_->keywords_[0] = paper_->keywords_[0].substr(0, pos);
      }
    }
  }

  return 0;
}

int AcademicPreprocEnglish::RecallSummary(const vector<string>& textvec, int headpage) {
  if (headpage + 1 >= textvec.size())
    return -1;

  const string& text = textvec[headpage + 1];
  if (SearchDictionary(AcademicDict::EnHeaderDict(), text) < 0)
    return -1;

  int num = keyposList_.size();
  for (int i = 0; i < num; ++i) {
    int prop = keyposList_[i].prop_;
    if (prop == 1) { // abstract
      int beg = keyposList_[i].poff_ + keyposList_[i].keylen_;
      int end = (i + 1 < num) ? keyposList_[i + 1].poff_ : text.size();
      paper_->abstract_ = text.substr(beg, end - beg);
      break;
    }
  }

  return 0;
}

int AcademicPreprocEnglish::FindCitation(const vector<string>& textvec, string* citation) {
  int pages = textvec.size();
  int beg = -1, end = -1;

  // locate citation beginning
  int idx = pages - 1;
  for (; idx > 0; --idx) {
    int ret = SearchDictionary(AcademicDict::EnCitationDict(), textvec[idx]);
    if (ret < 0)
      continue;

    int cur = 0, num = keyposList_.size();
    for (int i = cur + 1; i <= num; ++i) {
      int cur_prop = keyposList_[cur].prop_;
      if (i < num && cur_prop == 1 && keyposList_[i].prop_ == cur_prop)
        continue;

      if (cur_prop == 1) {  // reference
        beg = keyposList_[cur].poff_ + keyposList_[cur].keylen_;
        end = (i < num) ? keyposList_[i].poff_ : textvec[idx].size();
        *citation = textvec[idx].substr(beg, end - beg);
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
    for (; i < pages; ++i) {
      int ret = SearchDictionary(AcademicDict::EnCitationDict(), textvec[i]);
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

int AcademicPreprocEnglish::SplitCitation(const string& citation, vector<string>* items) {
  //fprintf(stdout, "citation:\n%s\n", citation.c_str());
  CiteMarker marker = GuessMarkerType(citation);

  if (marker == UNKNOWN) {
    SplitUnmarkedCitation(citation, items);
  } else {
    SplitCitationByMarker(citation, marker, items);
  }

  /*for (int i = 0; i < items->size(); ++i) {
    fprintf(stdout, "R: %s\n", items->at(i).c_str());
  }*/
  return 0;
}

// ===================================================================================

CiteMarker AcademicPreprocEnglish::GuessMarkerType(const string& text) {
  int count = 0;
  int maxcount = 0;
  CiteMarker marker = UNKNOWN;

  if ((count = AcademicRegex::Count(SQUARE, text)) > maxcount) {
    maxcount = count;
    marker = SQUARE;
  }
  if ((count = AcademicRegex::Count(PAREN, text)) > maxcount) {
    maxcount = count;
    marker = PAREN;
  }
  if ((count = AcademicRegex::Count(BRACES, text)) > maxcount) {
    maxcount = count;
    marker = BRACES;
  }
  if ((count = AcademicRegex::Count(NUMDOT, text)) > maxcount) {
    maxcount = count;
    marker = NUMDOT;
  }
  if ((count = AcademicRegex::Count(NAKENUM, text)) > maxcount) {
    maxcount = count;
    marker = NAKENUM;
  }

  int lines = 0;
  string::size_type beg = 0;
  string::size_type end = text.find('\n', beg);
  while (end != string::npos) {
    if (end - beg >= 10) {
      lines ++;
    }
    beg = end + 1;
    end = text.find('\n', beg);
  }

  //fprintf(stdout, "Marker: %d, Maxcount: %d, Lines: %d\n", marker, maxcount, lines);
  if (maxcount > std::max(2, lines/6))
    return marker;

  return UNKNOWN;
}

void AcademicPreprocEnglish::SplitCitationByMarker(const string& citation, enum CiteMarker marker, vector<string>* items) {
  pair<int, int> position = AcademicRegex::Search(marker, citation);

  for(;;) {
    // current reference
    string text;
    int beg = position.second - 3; // 3 is defined in academic_regex.cc
    while(ispunct(citation[beg]))
      beg ++;
    int end = std::max(beg, position.second);

    // search next reference
    position = AcademicRegex::Search(marker, citation, end);

    // current cut off
    int cutoff = position.first;
    if (cutoff< 0)
      cutoff = citation.size();

    // collect current
    bool flag = true; // first short cut flag
    int linesize = 0; // current max line size
    while ((end = citation.find('\n', beg)) != string::npos) {
      if (end > cutoff)
        break;

      int length = end - beg;
      if (length > linesize) {
        linesize = length;  // update max line size
      } else if (length < linesize / 2) {
        if (!flag) // later than first short cut, break
          break;
        flag = false;
      }
      // append line to reference
      text.append(citation.substr(beg, length) + " ");
      beg = end + 1; // update to search next line
    }
    items->push_back(text);

    // not found next reference, break
    if (position.first < 0)
      break;
  }
}

void AcademicPreprocEnglish::SplitUnmarkedCitation(const string& citation, vector<string>* items) {
  vector<string> lines;
  AcademicUtils::SplitText(citation, "\n", &lines);

  int cstart = 0;
  for (int i = 0; i < lines.size(); ++i) {
    if (AcademicRegex::IsYearExt(lines[i])) {
      // check start
      bool findBig = false;
      int newstart = cstart;
      for (int k = i; k >= newstart; --k) {
        // search big letter beginning
        bool fb = AcademicRegex::IsBigStart(lines[k]);
        if (findBig && !fb)
          break;
        else
          findBig = fb;

        // big letter begins, it may be an author name
        if (!fb)
          continue;

        // pass big letter, candidate start
        cstart = k;

        // previous line is small, current line is beginning
        if (k > 0 && lines[k-1].size() < lines[k].size() * 0.85) {
          break;
        }
        // too many lines
        if (i - k >= 3) {
          break;
        }
      }

      // collect
      string ref(lines[cstart]);
      for (int k = cstart + 1; k <= i; ++k) {
        if (ref[ref.size() - 1] != '-')
          ref.append(" ");
        ref.append(" " + lines[k]);
      }

      // recall remain tail
      int avgsize = lines[i].size();
      while (i+1 < lines.size()) {
        if (!AcademicRegex::IsBigStart(lines[i+1])
          || lines[i+1].size() < avgsize * 0.85) {
          i += 1;
          if (ref[ref.size() - 1] != '-')
            ref.append(" ");
          ref.append(lines[i]);
          avgsize = lines[i].size();
        } else {
          break;
        }
      }

      // push back
      items->push_back(ref);
      cstart = i + 1;
    }
  }
}




}


