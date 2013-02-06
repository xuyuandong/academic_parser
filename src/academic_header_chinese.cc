/*
 * academic_header_chinese.cc
 *
 *  Created on: 2012-3-1
 *      Author: x
 */


#include <cstdio>
#include <cstring>
#include <sstream>
#include "word_segmentor/public/gswssegmenter.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_regex.h"
#include "academic_postagger.h"
#include "academic_header_chinese.h"

using namespace std;
using namespace segmenter;

namespace academic {

void ChineseHeaderLine::PrintDebug() {

}

void ChineseHeaderLine::SetText(const string& text) {
  vector<SegmentedToken> tokens;
  seg_->FeedText(text.c_str(), text.size());

  SegmentedToken tn;
  while(seg_->GetNextToken(&tn)) {
    if (tn.semantic_attribute_.space_like)
      continue;
    tokens.push_back(tn);
  }

  vector<string> pospack;
  tag_->Tag(&pospack, tokens);

  words_.clear();
  SegmentedToken subtokens[10];
  for (int i = 0; i < tokens.size(); ++i) {
    words_.push_back(ChineseToken());
    ChineseToken& ct = words_.back();

    const SegmentedToken& token = tokens[i];
    ct.pos_ = pospack[i];
    ct.word_ = token.word_;

    // type
    ct.bSTOP_ = token.semantic_attribute_.stopword;
    ct.bPUNCT_ = token.semantic_attribute_.puctuation;
    ct.bPHRASE_ = token.is_phrase();

    // revise
    int beg = 0;
    while (beg < ct.word_.size()) {
      int bytes = AcademicUtils::GetCharBytes(ct.word_[beg]);
      if (bytes == 1) {
        ct.bPHRASE_ = false;
        break;
      }
      beg += bytes;
    }
    if (beg >= ct.word_.size() && !ct.bPHRASE_)
      ct.bCJK_ = true;

    // attribute
    ct.bName_ = token.semantic_attribute_.people_name;
    ct.bPlace_ = token.semantic_attribute_.place_name;
    ct.bOrgan_ = token.semantic_attribute_.organization;

    // N-gram
    if (ct.bPHRASE_) {
      int num = seg_->GetSubTokens(token, false, subtokens, 10);
      for (int k = 0; k < num; ++k) {
        ct.subwords_.push_back(subtokens[k].word_);
        ct.subName_ |= subtokens[k].semantic_attribute_.people_name;
        ct.subPlace_ |= subtokens[k].semantic_attribute_.place_name;
        ct.subOrgan_ |= subtokens[k].semantic_attribute_.organization;
      }
    } else {
      int idx = 0;
      while(idx < ct.word_.size()) {
        int bytes = AcademicUtils::GetCharBytes(ct.word_[idx]);
        ct.subwords_.push_back(ct.word_.substr(idx, bytes));
        idx += bytes;
      }
    }

  }

}

void ChineseHeaderLine::Tokenize(string* features) {
  for (int i = 0; i < words_.size(); ++i) {
    string output;
    words_[i].ToString(&output);
    output.append(label_);

    features->append(output);
    features->append("\n");
  }
}

ChineseToken::ChineseToken() : bName_(false), bPlace_(false), bOrgan_(false),
    subName_(false), subPlace_(false), subOrgan_(false), bCJK_(false),
    bSTOP_(false), bPUNCT_(false), bPHRASE_(false),
    eng_(0), digit_(0), bCap_(false) {
}

void ChineseToken::ToString(string* output) {
  ostringstream os;
  os << word_ << "\t\t ";

  // N-gram
  int len, size = subwords_.size();

  // forward
  os << subwords_[0] << " ";

  len = std::min(2, size);
  for (int i = 0; i < len; ++i)
    os << subwords_[i];
  os << " ";

  len = std::min(3, size);
  for (int i = 0; i < len; ++i)
    os << subwords_[i];
  os << " ";

  // backward
  os << subwords_[size - 1] << " ";

  len = std::max(0, size - 2);
  for (int i = len; i < size; ++i)
    os << subwords_[i];
  os << " ";

  len = std::max(0, size - 3);
  for (int i = len; i < size; ++i)
    os << subwords_[i];
  os << " ";

  // type
  if (bPHRASE_)
    os << "Phrase ";
  else if (bPUNCT_)
    os << "Punct ";
  else if (bSTOP_)
    os << "Stop ";
  else if (bCJK_)
    os << "CJKWord ";
  else {
    int other = 0;
    for (int i = 0; i < word_.size(); ++i) {
      char c = word_[i];
      if (ispunct(c))
        continue;

      if (isalpha(c)) {
        ++eng_;
      } else if (isdigit(c)) {
        ++digit_;
      } else {
        ++other;
      }
    }

    if (eng_ && digit_)
      os << "EngDig ";
    else if (eng_)
      os << "English ";
    else if (digit_)
      os << "Digit ";
    else
      os << "Other ";
  }

  // attribute
  if (bName_)
    os << "Name ";
  else if (bPlace_)
    os << "Place ";
  else if (bOrgan_)
    os << "Organ ";
  else if (AcademicRegex::IsDate(word_) && digit_ < 5)
    os << "Date ";
  else
    os << "Other ";

  // sub attribute
  if (subName_)
    os << "yna ";
  else
    os << "^na ";

  if (subPlace_)
    os << "ypl ";
  else
    os << "^pl ";

  if (subOrgan_)
    os << "yor ";
  else
    os << "^or ";

  // word compose
  if (bPHRASE_ || bCJK_ || bSTOP_) {
    int num = 0, beg = 0;
    while(beg < word_.size()) {
      ++num;
      beg += AcademicUtils::GetCharBytes(word_[beg]);
    }
    if (num < 4)
      os << num << "W ";
    else
      os << "4W+ ";
  } else if (bPUNCT_) {
    os << "P ";
  } else {
    if (eng_ && digit_) {
      if (eng_ > digit_ + 1)
        digit_ = 0;
      else if (digit_ > eng_ + 1)
        eng_ = 0;
    }

    if (eng_ && digit_) {
      if (eng_ <= 2 || digit_ <= 2)
        os << eng_ << digit_ << "CD ";
      else
        os << "MMCD ";
    } else if (eng_) {
      if (eng_ < 4)
        os << eng_ << "C ";
      else
        os << "4C+ ";
    } else if (digit_) {
      if (digit_ <= 6)
        os << digit_ << "D ";
      else
        os << "7D+ ";
    } else {
      os << "Other ";
    }
  }

  if (word_.find("@") != string::npos)
    os << "y@ ";
  else
    os << "^@ ";

  // pos tag
  os << pos_ << " ";

  // to string
  *output = os.str();
}

}

