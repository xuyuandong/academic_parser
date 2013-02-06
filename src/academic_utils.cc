/*
 * academic_utils.cc
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#include <cstring>
#include "page_analysis/academic_parser/internal/academic_dict.h"
#include "page_analysis/academic_parser/internal/academic_utils.h"

using namespace std;

namespace academic {

// get the byte number of a utf-8 character
int AcademicUtils::GetCharBytes(const char c) {
  if ((c & 0xF0) == 0xE0) {
    return 3;
  } else if ((c & 0xE0) == 0xC0) {
    return 2;
  }
  return 1;
}

void AcademicUtils::SplitText(const string& text, const string& tag, vector<string>* output) {
  string::size_type beg = 0;
  //while(text.find(tag, beg) == beg) {
  while(!text.compare(beg, tag.size(), tag)) {
    beg = beg + tag.size();
  }

  string::size_type end = text.size();

  output->clear();
  while ((end = text.find(tag, beg)) != string::npos) {
    output->push_back(text.substr(beg, end - beg));

    beg = end + tag.size();
    //while(text.find(tag, beg) == beg) {
    while(!text.compare(beg, tag.size(), tag)) {
      beg = beg + tag.size();
    }
  }

  if (beg < text.size()) {
    output->push_back(text.substr(beg));
  }
}

void AcademicUtils::SplitText2(const string& text, const string& tag, vector<string>* output) {
  string::size_type beg = text.find_first_not_of(tag);
  string::size_type end = text.size();

  output->clear();
  while(beg != string::npos) {
    end = text.find_first_of(tag, beg + 1);
    if (end == string::npos) {
      output->push_back(text.substr(beg));
      break;
    } else {
      output->push_back(text.substr(beg, end - beg));
      beg = text.find_first_not_of(tag, end + 1);
    }
  }

  if (beg < text.size()) {
    output->push_back(text.substr(beg));
  }
}

void AcademicUtils::NormalizeText(const string& text, string* output) {
  output->reserve(text.size());
  output->push_back(text[0]);

  int j = 0;
  string& outstr = *output;
  for (int i = 1; i < text.size(); ++i) {
    if (text[i] == ' ' && !isgraph(outstr[j]))
      continue;
    if (text.find("　", i) == i && !isgraph(outstr[j])) {
      i = i + strlen("　") - 1;
      continue;
    }
    outstr.push_back(text[i]);
    ++j;
  }
}

void AcademicUtils::NormalizeTextZh(const string& text, string* output) {
  // convert characters
  string norm;
  norm.reserve(text.size());

  int i = 0, bytes = 0;
  for (; i < text.size(); ) {
    char c = text[i];

    bytes = GetCharBytes(c);
    string str(text, i, bytes);
    if (bytes > 1) {
      AcademicDict::ConvertChar(str, &str);
    }

    norm.append(str);
    i += bytes;
  }
  //fprintf(stderr, "\nNorm:\n%s\n", norm.c_str());

  // remove redundant chars
  output->reserve(text.size());
  string& result = *output;

  int prlen = GetCharBytes(norm[0]);
  string prev(norm, 0, bytes);
  result.append(prev);

  for (i = bytes; i < norm.size(); ) {
    char c = norm[i];
    bytes = GetCharBytes(c);
    string cur(norm, i, bytes);

    if (!cur.compare(" ")) {
      if (prev.compare(" ") && prev.compare("\n")) {
        if (i + bytes >= norm.size()) {
          result.append(cur);
          prev = cur;
          prlen = bytes;
        } else {
          char nc = norm[i + bytes];
          int nxlen = GetCharBytes(nc);
          if (prlen == 1
              || (nxlen == 1 && nc != ' ' && nc != '\n')) {
            if (prev.compare(".") || !islower(nc)) { // merge "edu. cn"
              result.append(cur);
              prev = cur;
              prlen = bytes;
            }
          }
        }
      }
    } else if (!cur.compare("\n")) {
      if (prev.compare("\n")) {
        if (AcademicDict::StopPunctChar(prev)) {
          char nc = norm[i + bytes];
          if (!prev.compare(".") && islower(nc)) { // merge "edu. cn"
            cur = "";
          }
          result.append(cur);
          prev = cur;
          prlen = bytes;
        } else {
          if (prlen == 1 && isdigit(prev[0]) && isalpha(norm[i+bytes])) {
            cur = " ";
          } else {
            cur = "";
          }
          result.append(cur);
          prev = cur;
          prlen = bytes;
        }
      }
    } else {
      if (!cur.compare("[") && prev.compare("\n"))
        result.append("\n");
      if (AcademicDict::CondPunctChar(prev))
        result.append(" ");

      result.append(cur);
      prev = cur;
      prlen = bytes;
    }

    i += bytes;
  }

  //fprintf(stdout, "\nResult:\n%s\n", result.c_str());
}

int AcademicUtils::NormalMatch(const string& text, const string& str) {
  vector<int> index;
  index.reserve(text.size());

  string normtext;
  normtext.reserve(text.size());
  for (int i = 0; i < text.size(); ++i) {
    char c = text[i];
    if (c == ' ' || c == '\n' || c == '?')
      continue;
    normtext.push_back(c);
    index.push_back(i);
  }

  string normstr;
  normstr.reserve(str.size());
  for (int i = 0; i < str.size(); ++i) {
    char c = str[i];
    if (c == ' ' || c == '\n' || c == '?')
      continue;
    normstr.push_back(c);
  }

  string::size_type pos = normtext.find(normstr);
  if (pos == string::npos)
    return -1;

  return index[pos];
}

void AcademicUtils::MergeLinesByComma(vector<string>* plines) {
  vector<string>& lines = *plines;
  if (lines.empty())
     return;

  vector<string> newlines;
  newlines.reserve(lines.size());

  string str = lines[0];
  for (int i = 1; i < lines.size(); ++i) {
    int last = str.size() - 1;
    if (str[last] == ',') {
      str.append(" " + lines[i]);
    } else if (lines[i][0] == ',') {
      str.append(lines[i]);
    } else {
      newlines.push_back(str);
      str = lines[i];
    }
  }
  newlines.push_back(str);

  lines.swap(newlines);
}

void AcademicUtils::MergeLinesByDash(string* ptext) {
  if (ptext->empty())
    return;

  string& text = *ptext;
  int num = text.size();

  string newtext;
  newtext.reserve(num);

  int i = 0;
  for (; i < num - 1; ++i) {
    if (text[i] == '-' && text[i + 1] == '\n') {
      i = i + 1;
    } else {
      newtext.push_back(text[i]);
    }
  }
  if (i < num)
    newtext.push_back(text[num - 1]);

  text.swap(newtext);
}

void AcademicUtils::MergeWordsByDash(string* ptext) {
  if (ptext->empty())
    return;

  string& text = *ptext;
  int num = text.size();

  string newtext;
  newtext.reserve(num);
  newtext.push_back(text[0]);

  int i = 1;
  for (; i < num - 1; ++i) {
    if (text[i] == '-') {
      if (islower(text[i-1]) && islower(text[i+1]))
        i += 1;
    } else {
      newtext.push_back(text[i]);
    }
  }
  if (i < num)
    newtext.push_back(text[num - 1]);

  text.swap(newtext);
}

void AcademicUtils::MergeRedundantSpace(string* ptext) {
  string& text = *ptext;
  if (text.empty())
    return;

  int nsize = text.size();
  int prlen = GetCharBytes(text[0]);

  string newtext;
  newtext.reserve(nsize);

  if (text[0] != ' ')
    newtext.append(text.substr(0, prlen));

  int cur = prlen;
  for (; cur < nsize; ) {
    int bytes = GetCharBytes(text[cur]);
    string curtext(text, cur, bytes);

    if (!curtext.compare(" ")) {
      int nx = cur + bytes;
      if (nx < nsize) {
        int nxlen = GetCharBytes(text[nx]);
        if (prlen == 1 || nxlen == 1) {
          newtext.append(curtext);
        }
      }
    } else {
      newtext.append(curtext);
    }

    cur += bytes;
    prlen = bytes;
  }

  text.swap(newtext);
}

enum Op{
  OP_UNDEF = -1,
  OP_MATCH = 0,
  OP_INSERT = 1,
  OP_DELETE = 2
};

typedef struct {
  int cost;
  Op parent;
} Cell;

int AcademicUtils::FuzzyEditDistance(const string& src, const string& tar) {
  int ns = src.size(), nt = tar.size();

  // allocate memory
  int maxlen = std::max(ns, nt) + 1;
  Cell** m = new(nothrow) Cell*[maxlen];
  m[0] = new(nothrow) Cell[maxlen*maxlen];
  for (int i = 1; i < maxlen; ++i) {
    m[i] = m[0] + i*maxlen;
  }

  // init
  for (int i = 0; i < maxlen; ++i) {
    m[0][i].cost = 0;  // row init
    m[0][i].parent = OP_UNDEF;
    m[i][0].cost = i;  // col init
    m[i][0].parent = (i > 0)? OP_DELETE: OP_UNDEF;
  }

  int opt[3];
  for (int i = 1; i < ns; ++i) {
    for (int j = 1; j < nt; ++j) {
      opt[OP_MATCH] = m[i-1][j-1].cost + (src[i] == tar[j]);
      opt[OP_INSERT] = m[i][j-1].cost + 1;
      opt[OP_DELETE] = m[i-1][j].cost + 1;

      m[i][j].cost = opt[OP_MATCH];
      m[i][j].parent = OP_MATCH;
      for (int k = OP_INSERT; k <= OP_DELETE; ++k) {
        if (opt[k] < m[i][j].cost) {
          m[i][j].cost = opt[k];
          m[i][j].parent = static_cast<Op>(k);
        }
      }
    }
  }

  return m[ns][nt].cost;
}

int AcademicUtils::GetChinaEuropeMargin(const string& text) {
  int num = text.size();
  vector<int> accumulation(num + 1, 0);

  int bytes = 0;
  for (int i = 1; i <= num; i += bytes) {
    char c = text[i-1];
    bytes = GetCharBytes(c);

    if (bytes < 2) {
      if (isalpha(c))
        accumulation[i] = accumulation[i-1] + 1;
      else
        accumulation[i] = accumulation[i-1];
    } else {
      for (int k = i; k < i+bytes; ++k)
        accumulation[k] = accumulation[k-1] - 1;
    }
  }

  int margin = 0;
  int minpoint = num;
  for (int i = 1; i <= num; ++i) {
    if (accumulation[i] <= minpoint) {
      margin = i;
      minpoint = accumulation[i];
    }
  }

  return margin;
}

bool AcademicUtils::RemoveBegEndPunc(const string& text, string* output) {
  int beg = 0, end = text.size();

  for (beg = 0; beg < end; ++beg) {
    if (isalpha(text[beg]))
      break;
  }

  for (end = end - 1; end > beg; --end) {
    if (isalpha(text[end]))
      break;
  }

  if (end < beg)
    return false;

  *output = text.substr(beg, end - beg + 1);
  return true;
}

}
