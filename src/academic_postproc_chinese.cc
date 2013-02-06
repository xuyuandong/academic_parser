/*
 * academic_postproc_chinese.cc
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "base/logging.h"
#include "academic_data.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_regex.h"
#include "academic_postproc_chinese.h"

using namespace std;

namespace academic {

AcademicPostprocChinese::AcademicPostprocChinese(boost::shared_ptr<AcademicPaper> paper)
  : AcademicPostproc(paper) {

}

AcademicPostprocChinese::~AcademicPostprocChinese() {

}

void AcademicPostprocChinese::ProcessHeader() {
  FormatHeaderToWordList();
  ReviseHeaderProperty();
  NormalizeHeaderToPaper();
}

void AcademicPostprocChinese::ProcessCitation() {
  for (int i = 0; i < paper_->crf_vec_.size(); ++i) {
    FormatCitationToWordList(i);
    ReviseCitationProperty(i);
    NormalizeCitationToPaper(i);
  }
}

void AcademicPostprocChinese::FormatHeaderToWordList() {
  vector<string> lines;
  AcademicUtils::SplitText(paper_->crf_str_, "\n", &lines);

  wordlist_.clear();
  for (int i = 0; i < lines.size(); ++i) {
    vector<string> words;
    AcademicUtils::SplitText(lines[i], "\t", &words);

    WordProperty wp;
    wp.word_ = words[0];
    wp.label_ = AcademicDict::GetHeaderTag(words.back());

    wp.name_ = !words[8].compare("Name");
    wp.phrase_ = !words[7].compare("Phrase");

    wordlist_.push_back(wp);
  }
}

void AcademicPostprocChinese::ReviseHeaderProperty() {

}

void AcademicPostprocChinese::NormalizeHeaderToPaper() {
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
        title.append(wordlist_[i].word_);
      } else {
        titles.push_back(wordlist_[i].word_);
      }
    } else if (cur_id == IDX_AUTHOR) {  // author
      if (cur_id == lastid) {
        string& author = authors.back();
        author.append(wordlist_[i].word_);
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

  // divide author string into names
  vector<string> names;
  for (int i = 0; i < authors.size(); ++i) {
    AcademicUtils::SplitText2(authors[i], "'.,;1234567890", &names);

    for (int k = 0; k < names.size(); ++k) {
      if (names[k].empty())
        continue;
      Author au;
      au.name_ = names[k];
      paper_->authors_.push_back(au);
    }
  }

}

void AcademicPostprocChinese::FormatCitationToWordList(int idx) {
  vector<string> lines;
  AcademicUtils::SplitText(paper_->crf_vec_[idx], "\n", &lines);

  wordlist_.clear();
  for (int i = 0; i < lines.size(); ++i) {
    vector<string> words;
    AcademicUtils::SplitText(lines[i], "\t", &words);

    WordProperty wp;
    wp.word_ = words[0];
    wp.label_ = AcademicDict::GetCitationTag(words.back());

    wp.name_ = !words[8].compare("Name");
    wp.phrase_ = !words[7].compare("Phrase");

    wordlist_.push_back(wp);
  }
}

void AcademicPostprocChinese::ReviseCitationProperty(int idx) {

}

void AcademicPostprocChinese::NormalizeCitationToPaper(int idx) {
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
        date.append(wordlist_[i].word_);
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
    } else if (cur_id == IDX_PUBLISHER) {  // publisher
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
  AcademicUtils::MergeRedundantSpace(&citation.title_);

  // book
  if (!books.empty()) {
    citation.publication_ = books[0];
  } else if (!publishers.empty()) {
    citation.publication_ = publishers[0];
  }
  AcademicUtils::MergeRedundantSpace(&citation.publication_);

  // year
  if (!dates.empty()) {
    AcademicRegex::GetYearExt(dates[0], &citation.date_);
  }

  // TODO: divide authors into names
  vector<string> names;
  for (int i = 0; i < authors.size(); ++i) {
    AcademicUtils::SplitText2(authors[i], "'.,;1234567890", &names);

    for (int k = 0; k < names.size(); ++k) {
      AcademicUtils::MergeRedundantSpace(&names[k]);
      citation.names_.push_back(names[k]);
    }
  }

  // add to paper
  paper_->citations_.push_back(citation);
}

}
