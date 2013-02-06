/*
 * academic_feature.cc
 *
 *  Created on: 2012-1-18
 *      Author: x
 */

#include <cstdio>
#include <cstring>
#include <sstream>
#include "base/logging.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_regex.h"
#include "academic_reference_english.h"

using namespace std;

namespace academic {

void ReferenceLine::PrintDebug() {
  fprintf(stdout, "RL:%s <", label_.c_str());
  for (int i = 0; i < words_.size(); ++i)
    fprintf(stdout, "%s ", words_[i].c_str());
  fprintf(stdout, ">\n");
}

void ReferenceLine::SetText(const string& text) {
  words_.clear();
  AcademicUtils::SplitText2(text, "\t  ", &words_);
}

void ReferenceLine::Tokenize(string* feature_str) {
  ReferenceToken tokenfeat;
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

void ReferenceToken::Clear() {
  memset(this, 0, sizeof(ReferenceToken));
}

void ReferenceToken::Extract(ReferenceLine* line, int idx) {
  //const string& token = line->words_[idx];
}

void ReferenceToken::ToString(const string& token, string* output) {
  ostringstream os;
  os << token << " ";

  // last char type
  char p = token[token.size() - 1];
  if (islower(p))
    p = 'a';
  else if (isupper(p))
    p = 'A';
  else if (isdigit(p))
    p = '0';
  os << p << " ";

  os << token.substr(0, 1) << " ";
  os << token.substr(0, 2) << " ";
  os << token.substr(0, 3) << " ";
  //os << token.substr(0, 4) << " ";

  int pos = token.size() - 1, len = 1;
  os << token.substr(pos--, len++) << " ";
  os << token.substr(std::max(pos--, 0), len++) << " ";
  os << token.substr(std::max(pos--, 0), len++) << " ";
  //os << token.substr(std::max(pos, 0), len) << " ";

  string proto;
  GetProto(token, &proto);

  string protolower(proto);
  std::transform(protolower.begin(), protolower.end(), protolower.begin(), ::tolower);

  //os << proto << " ";
  os << protolower << " ";

  bool AllCap_ = false;
  bool BegCap_ = false;
  if (!proto.compare("<>")) {
    AllCap_ = true;
    BegCap_ = false;
  } else {
    AllCap_ = isupper(proto[0]);
    if (AllCap_) {
      for (int i = 1; i < proto.size(); ++i) {
        char c = proto[i];
        bool flag = (isalpha(c) && isupper(c));
        if (!flag) {
          AllCap_ = false;
          break;
        }
      }
    }

    BegCap_ = isupper(proto[0]);
    int idx = 1;
    for (; idx < proto.size(); ++idx) {
      if (isupper(proto[idx]))
        continue;
      if (islower(proto[idx])) {
        break;
      } else {
        BegCap_ = false;
        break;
      }
    }
    if (idx == proto.size())
      BegCap_ = false;
  }

  int acount = 0, dcount = 0;
  for (int i = 0; i < proto.size(); ++i) {
    char c = proto[i];
    if (isalpha(c))
      ++acount;
    else if (isdigit(c))
      ++dcount;
  }
  bool SinCap_ = false;
  if (acount == 1 && dcount == 0 && AllCap_)
    SinCap_ = true;

  if (SinCap_)
    os << "sinCap ";
  else if (BegCap_)
    os << "begCap ";
  else if (AllCap_)
    os << "allCap ";
  else
    os << "nonCap ";

  // Dot
  if (token.size() == 2 && isupper(token[0]) && token[1] == '.')
    os << "CapDot ";
  else
    os << "nonCD ";

  string protoNSide;
  RemoveSidePunct(token, &protoNSide);

  if (AcademicRegex::IsYear(protoNSide))
    os << "year ";
  else if (AcademicRegex::IsYearExt(token))
    os << "year ";
  else if (AcademicRegex::IsPage(token))
    os << "page ";
  else if (AcademicRegex::IsVolume(token))
    os << "volume ";
  else if (AcademicRegex::IsOrdinal(protoNSide))
    os << "order ";
  else {
    if (acount != 0 && dcount != 0)
      os << "hasNum ";
    else if (dcount == 0)
      os << "nonNum ";
    else if (dcount == 1)
      os << "1dig ";
    else if (dcount == 2)
      os << "2dig ";
    else if (dcount == 3)
      os << "3dig ";
    else if (dcount >= 4)
      os << "4+dig ";
  }

  if (AcademicDict::MatchName(protolower))
    os << "hasName ";
  else
    os << "notName ";

  if (AcademicDict::MatchDate(protolower))
    os << "hasDate ";
  else
    os << "notDate ";

  if (AcademicDict::MatchAddr(protoNSide))
    os << "hasAddr ";
  else if (AcademicDict::MatchAddr(protolower))
    os << "hasAddr ";
  else
    os << "notAddr ";

  if (AcademicDict::MatchPublisher(protolower))
    os << "hasPub ";
  else
    os << "notPub ";

  if (AcademicRegex::IsEditors(token))
    os << "hasEds ";
  else
    os << "notEds ";

  if (token.find("http://") != string::npos)
    os << "hasUrl ";
  else
    os << "notUrl ";

  int flag = AcademicRegex::GetPunctType(token);
  switch (flag) {
  case 1: os << "leadQuote "; break;
  case 2: os << "endQuote "; break;
  case 3: os << "multiHyphen "; break;
  case 4: os << "contPunct "; break;
  case 5: os << "stopPunct "; break;
  case 6: os << "braces "; break;
  default: os << "others "; break;
  }

  string prepunc, postpunc;
  GetPunctuation(token, &prepunc, &postpunc);
  os << prepunc << " ";
  os << postpunc << " ";

  // To string
  *output = os.str();
}

void ReferenceToken::GetProto(const string& token, string* proto) {
  string str;
  str.reserve(token.size());
  for (int i = 0; i < token.size(); ++i) {
    char c = token[i];
    if (isalnum(c))
      str.push_back(c);
  }
  if (str.empty())
    str = "<>";
  proto->swap(str);
}

void ReferenceToken::RemoveAllPunct(const string& token, string* proto) {
  string str;
  str.reserve(token.size());
  for (int i = 0; i < token.size(); ++i) {
    char c = token[i];
    if (isalnum(c))
      str.push_back(c);
  }
  proto->swap(str);
}

void ReferenceToken::RemoveSidePunct(const string& token, string* proto) {
  int beg = 0, end = token.size();

  for (beg = 0; beg < end; ++beg) {
    if (isalnum(token[beg]))
      break;
  }

  for (end = end - 1; end > beg; --end) {
    if (isalnum(token[end]))
      break;
  }

  if (end < beg)
    *proto = "";
  else
    *proto = token.substr(beg, end - beg + 1);
}

void ReferenceToken::GetPunctuation(const string& token, string* prepunc,
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

  *prepunc = (beg < 0) ? "<>" : token.substr(0, beg + 1);
  *postpunc = (end >= length) ? "<>" : token.substr(end, length - end);
}

}
