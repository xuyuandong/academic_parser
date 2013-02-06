/*
 * academic_feature.cc
 *
 *  Created on: 2012-1-18
 *      Author: x
 */

#include <cstdio>
#include <cstring>
#include <sstream>
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_header_english.h"

using namespace std;

namespace academic {

void HeaderLine::PrintDebug() {
  fprintf(stdout, "LF:%s %d %d <", label_.c_str(), linebeg_, lineend_);
  for (int i = 0; i < words_.size(); ++i) {
    fprintf(stdout, "%s ", words_[i].c_str());
  }
  fprintf(stdout, ">\n");
}

void HeaderLine::SetText(const string& text) {
  words_.clear();
  AcademicUtils::SplitText2(text, " \t", &words_);
}

void HeaderLine::Tokenize(string* feature_str) {
  HeaderToken tokenfeat;
  for (int i = 0; i < words_.size(); ++i) {
    tokenfeat.Clear();
    tokenfeat.Extract(this, i);

    string output;
    tokenfeat.ToString(words_[i], &output);
    output.append(label_);

    feature_str->append(output);
    feature_str->append("\n");
  }
}

void HeaderToken::Clear() {
  memset(this, 0, sizeof(HeaderToken));
}

void HeaderToken::Extract(HeaderLine* line, int idx) {
  const string& token = line->words_[idx];

  // character feature
  int length = token.size();

  // Cap
  HasCap_ = BegCap_ = isupper(token[0]);;
  for (int i = 1; i < length; ++i){
    HasCap_ = HasCap_ || isupper(token[i]);
  }

  if (HasCap_) {
    AllCap_ = HasCap_;
    for (int i = 0; i < length; ++i) {
      bool flag = isupper(token[i])||ispunct(token[i])||isdigit(token[i]);
      AllCap_ = AllCap_ && flag;
    }
  }

  // Digit
  BegNum_ = isdigit(token[0]);
  EndNum_ = isdigit(token[length-1]);

  HasNum_ = BegNum_ || EndNum_;
  for (int i = 1; i < length - 1; ++i) {
    HasNum_ = HasNum_ || isdigit(token[i]);
  }

  if (HasNum_) {
    AllNum_ = HasNum_;
    for (int i = 0; i < length; ++i) {
      bool flag = isdigit(token[i])||ispunct(token[i]);
      AllNum_ = AllNum_ && flag;
    }
  }

  // "M." 
  if (length == 2 && BegCap_ && token[length-1] == '.')
    CapDot_ = true;

  if (token.find(".") != string::npos)
    HasDot_ = true;

  // At
  if (token.find("@") != string::npos)
    HasAt_ = true;

  // Punctuation
  BegPunc_ = ispunct(token[0]);
  EndPunc_ = ispunct(token[length-1]);

  AllPunc_ = BegPunc_ && EndPunc_;
  HasPunc_ = BegPunc_ || EndPunc_;
  for (int i = 1; i < length - 1; ++i) {
    bool flag = ispunct(token[i]);
    AllPunc_ = AllPunc_ && flag;
    HasPunc_ = HasPunc_ || flag;
  }

  AllWord_ = true;
  for (int i = 0; i < length; ++i) {
    AllWord_ = AllWord_ && isalpha(token[i]);
  }

  for (int i = 0; i < length; ++i) {
    if (token[i] == '{' || token[i] == '}') {
      HasBracket_ = true;
      break;
    }
  }

  if (length == 1) {
    SingleNumber_ = BegNum_;
    SingleLetter_ = AllWord_;
  }

  // layout feature
  if (idx == 0)
    LineBeg_ = line->linebeg_;
  if (idx + 1 == line->words_.size())
    LineEnd_ = line->lineend_;

  LineMid_ = !(LineBeg_ || LineEnd_);

}

void HeaderToken::ToString(const string& token, string* output) {
  ostringstream os;
  os << token << " ";

  // Capitalization
  if (AllCap_)
    os << "AllCap" << " ";
  else if (BegCap_)
    os << "BegCap" << " ";
  else if (HasCap_)
    os << "HasCap" << " ";
  else
    os << "NoCap" << " ";

  // Digits
  if (AllNum_)
    os << "AllNum" << " ";
  else if (HasNum_)
    os << "HasNum" << " ";
  else
    os << "NoNum" << " ";

  if (BegNum_ && EndNum_)
    os << "NN" << " ";
  else if (EndNum_)
    os << "+N" << " ";
  else if (BegNum_)
    os << "N+" << " ";
  else
    os << "++" << " ";

  // "M."
  if (CapDot_)
    os << "CapDot" << " ";
  else
    os << "NotCD" << " ";

  // Dot "."
  if (HasDot_)
    os << "HasDot" << " ";
  else
    os << "NoDot" << " ";

  // Address "@"
  if (HasAt_)
    os << "HasAt" << " ";
  else
    os << "NoAt" << " ";

  // Punctuation
  if (AllPunc_)
    os << "P" << " ";
  else if (BegPunc_ && EndPunc_)
    os << "P+P" << " ";
  else if (BegPunc_)
    os << "P+" << " ";
  else if (EndPunc_)
    os << "+P" << " ";
  else if (HasPunc_)
    os << "+P+" << " ";
  else
    os << "NP" << " ";

  // Bracket
  if (HasBracket_)
    os << "HasBracket" << " ";
  else
    os << "NoBracket" << " ";

  // Word
  if (AllWord_)
    os << "AllWord" << " ";
  else
    os << "NotWord" << " ";

  // Single
  if (SingleNumber_)
    os << "SingleNumber" << " ";
  else if (SingleLetter_)
    os << "SingleLetter" << " ";
  else if (token.size() == 1)
    os << "SingleOther" << " ";
  else
    os << "NotSingle" << " ";

  // Layout
  if (LineBeg_ && LineEnd_)
    os << "L" << " ";
  else if (LineBeg_)
    os << "L+" << " ";
  else if (LineEnd_)
    os << "+L" << " ";
  else
    os << "+L+" << " ";

  // Proto word, pre- and post punctuation
  string proto, prepunc, postpunc;
  GetProto(token, &proto);
  GetPunctuation(token, &prepunc, &postpunc);

  os << proto << " ";
  os << prepunc << " ";
  os << postpunc << " ";

  // special case
  if (proto.size() == 2 && AcademicDict::MatchAddr(proto))
    HasAddr_ = true;

  GetRawProto(&proto);
  std::transform(proto.begin(), proto.end(), proto.begin(), ::tolower);
  os << proto << " ";

  // Matching
  if (token.find("http://") != string::npos)
    HasHttp_ = true;
  if (AcademicDict::MatchName(proto))
    HasName_ = true;
  if (AcademicDict::MatchAddr(proto))
    HasAddr_ = true;

  // Match feature
  if (HasHttp_)
    os << "HasHttp" << " ";
  else
    os << "NotHttp" << " ";

  if (HasName_)
    os << "HasName" << " ";
  else
    os << "NotName" << " ";

  if (HasAddr_)
    os << "HasAddr" << " ";
  else
    os << "NotAddr" << " ";

  // To string
  *output = os.str();
}

void HeaderToken::GetProto(const string& token, string* proto) {
  bool flag = AcademicUtils::RemoveBegEndPunc(token, proto);
  if (!flag)
    *proto = "<>";
}

void HeaderToken::GetRawProto(string* proto) {
  string& oldproto = *proto;
  int protolen = oldproto.size();

  string rawproto;
  rawproto.reserve(protolen);

  for (int i = 0; i < protolen; ++i) {
    if (isalpha(oldproto[i])) {
      rawproto.push_back(oldproto[i]);
    }
  }

  *proto = (rawproto.empty())? "<>" : rawproto;
}

void HeaderToken::GetPunctuation(const string& token, string* prepunc,
    string* postpunc) {
  int length = token.size();
  int beg = 0, end = length;

  for (; beg < end; ++beg) {
    if (isalnum(token[beg]))
      break;
  }
  beg = beg - 1;

  for (end = end - 1; end >= 0; --end) {
    if (isalnum(token[end]))
      break;
  }
  end = end + 1;

  // fprintf(stdout, "%s %d %d %d\n", token.c_str(), beg, end, length);
  *prepunc = (beg < 0) ? "<>" : token.substr(0, beg + 1);
  *postpunc = (end >= length) ? "<>" : token.substr(end, length - end);
}

}
