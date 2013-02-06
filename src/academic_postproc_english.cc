/*
 * academic_postproc_english.cc
 *
 *  Created on: 2012-2-29
 *      Author: x
 */


#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "base/logging.h"
#include "ext_libs/search/public/mmatch.h"
#include "academic_dict.h"
#include "academic_data.h"
#include "academic_utils.h"
#include "academic_regex.h"
#include "academic_postproc_english.h"

using namespace std;
using namespace extlibs;

namespace academic {

AcademicPostprocEnglish::AcademicPostprocEnglish(boost::shared_ptr<AcademicPaper> paper)
  : AcademicPostproc(paper) {

}

AcademicPostprocEnglish::~AcademicPostprocEnglish() {

}

void AcademicPostprocEnglish::ProcessHeader() {
  FormatHeaderToWordList();
  ReviseHeaderProperty();
  NormalizeHeaderToPaper();
}
void AcademicPostprocEnglish::ProcessCitation() {
  for (int i = 0; i < paper_->crf_vec_.size(); ++i) {
    FormatCitationToWordList(i);
    ReviseCitationProperty(i);
    NormalizeCitationToPaper(i);
  }
}

void AcademicPostprocEnglish::FormatHeaderToWordList() {
  vector<string> lines;
  AcademicUtils::SplitText(paper_->crf_str_, "\n", &lines);

  // transform to WordProperty list
  wordlist_.clear();
  for (int i = 0; i < lines.size(); ++i) {
    vector<string> words;
    AcademicUtils::SplitText(lines[i], "\t", &words);

    WordProperty wp;
    wp.word_ = words[0];
    wp.label_ = AcademicDict::GetHeaderTag(words.back());
    wp.bemail_ = words[6].compare("NoAt"); // @
    wp.bhttp_ = words[16].compare("NotHttp"); // http://

    if (!words[11].compare("L+"))
      wp.linepos_ = LBEGIN;
    else if (!words[11].compare("+L"))
      wp.linepos_ = LENDLE;
    else if (!words[11].compare("L"))
      wp.linepos_ = LALONE;
    else
      wp.linepos_ = LMIDDLE; // +L+

    wordlist_.push_back(wp);
  }

  //for (int i = 0; i < wordlist_.size(); ++i)
  //  wordlist_[i].PrintDebug();
}

void AcademicPostprocEnglish::ReviseHeaderProperty() {
  ReviseEvent event = RDEFAULT;
  int beg = 0, end = 0;
  int emailpos = 0;
  for (int i = 0; i < wordlist_.size(); ++i) {
    WordProperty& wp = wordlist_[i];
    if (wp.bemail_) {
      event = REMAIL; //TODO: change all tag email and extract
      emailpos = i;
    } else if (wp.bhttp_) {
      event = RHTTP;  //TODO: change all tag note and remove
    }

    if (wp.linepos_ == LBEGIN || wp.linepos_ == LALONE)
      beg = i;
      event = RDEFAULT;
    if (wp.linepos_ == LENDLE || wp.linepos_ == LALONE) {
      end = i;
      HeaderInnerLineRevise(event, emailpos, beg, end);
    }
  }
}

void AcademicPostprocEnglish::HeaderInnerLineRevise(enum ReviseEvent event, int emailpos, int beg, int end) {
  const int tagemail = AcademicDict::GetHeaderTag("email");
  const int tagnote = AcademicDict::GetHeaderTag("note");
  const int tagtitle = AcademicDict::GetHeaderTag("title");
  const int tagauthor = AcademicDict::GetHeaderTag("author");

  if (event == REMAIL) {
    bool emailgroup = false;
    if (wordlist_[emailpos].word_.find("}@") != string::npos)
      emailgroup = true;

    int start = emailpos;
    int findbracket = false;
    for (; emailgroup && start >= beg; --start) {
      if (wordlist_[start].word_[0] == '{')
        findbracket = true;
        break;
    }

    if (emailgroup && !findbracket) {
      for (int i = beg - 1; i >= 0 && i > beg - 3; --i) {
        if (wordlist_[i].word_[0] == '{') {
          findbracket = true;
          start = i;
          break;
        }
      }
    }

    for (int i = beg; i <= end; ++i) {
      if (i >= start && i <= emailpos)
        wordlist_[i].label_ = tagemail;
      else
        wordlist_[i].label_ = tagnote;
    }

    if (emailgroup && findbracket && start < beg) {
      for (int i = start; i < beg; ++i)
        wordlist_[i].label_ = tagemail;
    }

  } else if (event == RHTTP) {
    for (int i = beg; i <= end; ++i)
      wordlist_[i].label_ = tagnote;
  } else {
    // default: check title and author
    int titlenum = 0, authornum = 0;
    int allnum = end - beg + 1;
    for (int i = beg; i <= end; ++i) {
      if (wordlist_[i].label_ == tagtitle)
        titlenum ++;
      else if (wordlist_[i].label_ == tagauthor)
        authornum ++;
    }
    // TODO:tricks:
    if (titlenum + authornum == allnum) {
      if (allnum >= 5 && titlenum > 2*authornum) {
        for (int i = beg; i <= end; ++i)
          wordlist_[i].label_ = tagtitle;
      }
    }
  }
}

void AcademicPostprocEnglish::NormalizeHeaderToPaper() {
  const int IDX_TITLE = AcademicDict::GetHeaderTag("title");
  const int IDX_AUTHOR = AcademicDict::GetHeaderTag("author");

  vector<string> titles;
  vector<string> authors;

  // collection
  int lastid = -1;
  for (int i = 0; i < wordlist_.size(); ++i) {
    int cur_id = wordlist_[i].label_;
    if (cur_id == IDX_TITLE) {  // title
      if (cur_id == lastid) {
        string& title = titles.back();
        title.append(" " + wordlist_[i].word_);
      } else {
        titles.push_back(wordlist_[i].word_);
      }
    } else if (cur_id == IDX_AUTHOR) {  // author
      if (cur_id == lastid) {
        string& author = authors.back();
        author.append(" " + wordlist_[i].word_);
      } else {
        authors.push_back(wordlist_[i].word_);
      }
    }

    lastid = cur_id;
  }

  // computing title
  if (!titles.empty()) {
    if (titles.size() == 1 || paper_->title_.empty()) {
      paper_->title_ = titles[0];
    } else {
      int cand = 0;
      int cost = AcademicUtils::FuzzyEditDistance(paper_->title_, titles[0]);
      for (int i = 1; i < titles.size(); ++i) {
        int c = AcademicUtils::FuzzyEditDistance(paper_->title_, titles[i]);
        if (c < cost) {
          cand = i;
          cost = c;
        }
      }
      paper_->title_ = titles[cand];
    }
  }

  // divide authors
  vector<string> names;
  mm_pack_t* ppack = mm_pack_create(MAX_LEMMA_NUM);
  for (int i = 0; i < authors.size(); ++i) {
    int ret = mm_search(AcademicDict::EnAuthorDivCharDict(),
        ppack, authors[i].c_str(), authors[i].size());
    if (ret < 0 || !ppack->ppseg_cnt) {
      names.push_back(authors[i]);
      continue;
    }

    int beg = 0, end = authors[i].size();
    for (int k = 0; k < ppack->ppseg_cnt; ++k) {
      end = ppack->poff[k];
      names.push_back(authors[i].substr(beg, end - beg));
      beg = end + ppack->ppseg[k]->len;
    }
    if (beg < authors[i].size()) {
      names.push_back(authors[i].substr(beg));
    }
  }
  mm_pack_del(ppack);

  for (int i = 0; i < names.size(); ++i) {
    if (AcademicUtils::RemoveBegEndPunc(names[i], &names[i])) {
      Author au;
      au.name_ = names[i];
      paper_->authors_.push_back(au);
    }
  }

  /*for (int i = 0; i < titles.size(); ++i)
    fprintf(stdout, "TITLE<%d>:%s\n", i, titles[i].c_str());
  for (int i = 0; i < authors.size(); ++i)
    fprintf(stdout, "Author<%d>:%s\n", i, authors[i].c_str());*/
}

// ===================================================================================

void AcademicPostprocEnglish::FormatCitationToWordList(int idx) {
  vector<string> lines;
  AcademicUtils::SplitText(paper_->crf_vec_[idx], "\n", &lines);

  wordlist_.clear();
  for (int i = 0; i < lines.size(); ++i) {
    vector<string> words;
    AcademicUtils::SplitText(lines[i], "\t", &words);

    WordProperty wp;
    wp.word_ = words[0];
    wp.label_ = AcademicDict::GetCitationTag(words.back());
    wp.bhttp_ = words[17].compare("notUrl");
    wp.byear_ = !words[11].compare("year");

    wordlist_.push_back(wp);
  }
}

void AcademicPostprocEnglish::ReviseCitationProperty(int idx) {
  //const int IDX_AUTHOR = AcademicDict::GetHeaderTag("author");  // 1
  const int IDX_TITLE = AcademicDict::GetHeaderTag("title");   // 2
  const int IDX_DATE = AcademicDict::GetHeaderTag("date");     // 3
  //const int IDX_BOOK = AcademicDict::GetHeaderTag("book");     // 4

  string date_str;
  bool hasDate = false;

  for (int i = 0; i < wordlist_.size(); ++i) {
    const WordProperty& wp = wordlist_[i];
    if (wp.label_ < IDX_TITLE)
      continue;

    if (wp.label_ == IDX_DATE) {
      hasDate = true;
      break;
    }

    pair<int, int> pos;
    if (AcademicRegex::IsYearExt(wp.word_, &pos)) {
      date_str = wp.word_.substr(pos.first, pos.second - pos.first);
    }
  }

  if (!hasDate && !date_str.empty()) {
    WordProperty wp;
    wp.label_ = IDX_DATE;
    wp.word_ = date_str;
    wordlist_.push_back(wp);
  }

}

void AcademicPostprocEnglish::NormalizeCitationToPaper(int idx) {
  const int IDX_TITLE = AcademicDict::GetCitationTag("title");
  const int IDX_AUTHOR = AcademicDict::GetCitationTag("author");
  const int IDX_DATE = AcademicDict::GetCitationTag("date");
  const int IDX_BOOK = AcademicDict::GetCitationTag("book");
  const int IDX_PUBLISHER = AcademicDict::GetCitationTag("publisher");

  vector<string> dates;
  vector<string> books;
  vector<string> titles;
  vector<string> authors;
  vector<string> publishers;

  // collection
  int lastid = -1;
  for (int i = 0; i < wordlist_.size(); ++i) {
    int cur_id = wordlist_[i].label_;
    if (cur_id == IDX_AUTHOR) {  // author
      if (cur_id == lastid) {
        string& author = authors.back();
        author.append(" " + wordlist_[i].word_);
      } else {
        authors.push_back(wordlist_[i].word_);
      }
    } else if (cur_id == IDX_TITLE) {  // title
      if (cur_id == lastid) {
        string& title = titles.back();
        title.append(" " + wordlist_[i].word_);
      } else {
        titles.push_back(wordlist_[i].word_);
      }
    } else if (cur_id == IDX_DATE) {  // date
      if (cur_id == lastid) {
        string& date = dates.back();
        date.append(" " + wordlist_[i].word_);
      } else {
        dates.push_back(wordlist_[i].word_);
      }
    } else if (cur_id == IDX_BOOK) {  // book
      if (cur_id == lastid) {
        string& book = books.back();
        book.append(" " + wordlist_[i].word_);
      } else {
        books.push_back(wordlist_[i].word_);
      }
    } else if (cur_id == IDX_PUBLISHER) { // publisher
      if (cur_id == lastid) {
        string& pub = publishers.back();
        pub.append(" " + wordlist_[i].word_);
      } else {
        publishers.push_back(wordlist_[i].word_);
      }
    }

    lastid = cur_id;
  }

  // debug print
  /*for (int i = 0; i < titles.size(); ++i)
    fprintf(stdout, "Title<%d>:%s\n", i, titles[i].c_str());
  for (int i = 0; i < authors.size(); ++i)
    fprintf(stdout, "Author<%d>:%s\n", i, authors[i].c_str());
  for (int i = 0; i < books.size(); ++i)
    fprintf(stdout, "Book<%d>:%s\n", i, books[i].c_str());
  for (int i = 0; i < dates.size(); ++i)
    fprintf(stdout, "Date<%d>:%s\n", i, dates[i].c_str());*/

  // add citation
  Citation citation;

  // title
  if (!titles.empty()) {
    citation.title_ = titles[0];
  }
  // book
  if (!books.empty()) {
    citation.publication_ = books[0];
  } else if (!publishers.empty()) {
    citation.publication_ = publishers[0];
  }
  // year
  if (!dates.empty()) {
    AcademicRegex::GetYearExt(dates[0], &citation.date_);
  }

  // divide authors
  vector<string> names;
  mm_pack_t* ppack = mm_pack_create(MAX_LEMMA_NUM);
  for (int i = 0; i < authors.size(); ++i) {
    int ret = mm_search(AcademicDict::EnAuthorDivCharDict(), ppack,
        authors[i].c_str(), authors[i].size());
    if (ret < 0 || !ppack->ppseg_cnt) {
      names.push_back(authors[i]);
      continue;
    }

    int beg = 0, end = authors[i].size();
    for (int k = 0; k < ppack->ppseg_cnt; ++k) {
      end = ppack->poff[k];
      names.push_back(authors[i].substr(beg, end - beg));
      beg = end + ppack->ppseg[k]->len;
    }
    if (beg < authors[i].size()) {
      names.push_back(authors[i].substr(beg));
    }
  }
  mm_pack_del(ppack);

  // combine Abc, M., Hij, K., case.
  for (int i = 0; i < names.size(); ++i) {
    bool capdot = false;
    const string& token = names[i];
    if (token.size() == 2 && isupper(token[0]) && token[1] == '.')
      capdot = true;

    if (capdot && i > 0) {
      int last = citation.names_.size() - 1;
      citation.names_[last].append(" " + token);
    } else {
      if (AcademicUtils::RemoveBegEndPunc(names[i], &names[i]))
        citation.names_.push_back(names[i]);
    }
  }

  // add to paper
  paper_->citations_.push_back(citation);
}

}
