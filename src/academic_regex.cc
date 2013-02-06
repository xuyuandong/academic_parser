////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 by Goso  ALL RIGHTS RESERVED.
// Consult your license regarding permissions and restrictions.
//
// cpp file for academic regex.
// Author: xuyuandong@jike.com
////////////////////////////////////////////////////////////////////////

#include <cstdio>
#include "base/logging.h"
#include "base/hash_tables.h"
#include "third_party/boost/boost/regex.hpp"
#include "academic_regex.h"

using namespace std;

namespace academic {

// for count
base::hash_map<CiteMarker, boost::regex, HashMarker> AcademicRegex::cite_marker_map_;
// for search
base::hash_map<CiteMarker, boost::regex, HashMarker> AcademicRegex::cite_marker_ext_;

// for count
base::hash_map<CiteMarker, boost::regex, HashMarker> AcademicRegex::zh_cite_marker_map_;
// for search
base::hash_map<CiteMarker, boost::regex, HashMarker> AcademicRegex::zh_cite_marker_ext_;

void AcademicRegex::Init() {
  // parentheses
  cite_marker_map_[PAREN] = boost::regex("\n\\s*(\\([^\n]+?\\)([^\n]){10})");
  // square brackets
  cite_marker_map_[SQUARE] = boost::regex("\n\\s*(\\[[^\n]+?\\]([^\n]){10})");
  // braces
  cite_marker_map_[BRACES] = boost::regex("\n\\s*(\\{[^\n]+?\\}([^\n]){10})");
  // naked number
  cite_marker_map_[NAKENUM] = boost::regex("\n\\s*(\\d+ [^\n]{10})");
  // number dot
  cite_marker_map_[NUMDOT] = boost::regex("\n\\s*(\\d+\\.([^\n]){10})");

  // parentheses
  cite_marker_ext_[PAREN] = boost::regex("\n\\s*(\\([^\n]+?\\)([^\n]){3})");
  // square brackets
  cite_marker_ext_[SQUARE] = boost::regex("\n\\s*(\\[[^\n]+?\\]([^\n]){3})");
  // braces
  cite_marker_ext_[BRACES] = boost::regex("\n\\s*(\\{[^\n]+?\\}([^\n]){3})");
  // naked number
  cite_marker_ext_[NAKENUM] = boost::regex("\n\\s*(\\d+ [^\n]{3})");
  // number dot
  cite_marker_ext_[NUMDOT] = boost::regex("\n\\s*(\\d+\\.([^\n]){3})");

  // Chinese
  zh_cite_marker_map_[SQUARE] = boost::regex("\n\\s*(\\[[^\na-zA-Z]+?\\]([^\n]){10})");
  zh_cite_marker_ext_[SQUARE] = boost::regex("\n\\s*(\\[[^\na-zA-Z]+?\\]([^\n]){3})");
}

void AcademicRegex::Release() {

}

bool AcademicRegex::IsBigStart(const string& str) {
  static const boost::regex big("^\\s*[A-Z]");

  boost::smatch result;
  return boost::regex_search(str, result, big);
}

bool AcademicRegex::IsYear(const string& str) {
  static const boost::regex year1("^(19|20)[0-9][0-9]");
  static const boost::regex year2("^(19|20)[0-9][0-9]\\D");

  if (boost::regex_match(str, year1))
    return true;

  boost::smatch result;
  return boost::regex_search(str, result, year2);
}

bool AcademicRegex::IsYearExt(const string& str, pair<int, int>* pos) {
  static const boost::regex year3("[\\W]*(19|20)[0-9][0-9][\\D]");

  boost::smatch result;
  if (boost::regex_search(str, result, year3)) {
    if (pos != NULL) {
      pos->first = static_cast<int>(result[0].first - str.begin()) + 1;
      pos->second = static_cast<int>(result[0].second - str.begin()) - 1;
    }
    return true;
  }
  return false;
}

bool AcademicRegex::GetYearExt(const string& str, string* date) {
  static const boost::regex year4("(19|20)[0-9][0-9]");

  boost::smatch result;
  if (boost::regex_search(str, result, year4)) {
    date->assign(result[0].first, result[0].second);
    return true;
  }
  return false;
}

bool AcademicRegex::IsPage(const string& str) {
  static const boost::regex page_("[0-9]\\-[0-9]");

  boost::smatch result;
  return boost::regex_search(str, result, page_);
}

bool AcademicRegex::IsVolume(const string& str) {
  static const boost::regex volume_("[0-9]\\([0-9]+\\)");

  boost::smatch result;
  return boost::regex_search(str, result, volume_);
}

bool AcademicRegex::IsOrdinal(const string& str) {
  static const boost::regex ordinal_("^[0-9]+(th|st|nd|rd)");

  return boost::regex_match(str, ordinal_);
}

bool AcademicRegex::IsEditors(const string& str) {
  static const boost::regex editor_("[^A-Za-z](ed\\.|editor|editors|eds\\.)");

  boost::smatch result;
  return boost::regex_search(str, result, editor_);
}

int AcademicRegex::GetPunctType(const string& str) {
  static const boost::regex begQuote_("^[\"'`]");
  static const boost::regex endQuote_("[\"'`][^s]?$");
  static const boost::regex hyphen_("\\-.*\\-");
  static const boost::regex contPunct_("[\\-,:;]$");
  static const boost::regex stopPunct_("[!\\?.\"']$");
  static const boost::regex braces_("^[(\\[{<].+[)\\]}>].?$");

  boost::smatch result;
  if (boost::regex_search(str, result, begQuote_))
    return 1;
  else if (boost::regex_search(str, result, endQuote_))
    return 2;
  else if (boost::regex_search(str, result, hyphen_))
    return 3;
  else if (boost::regex_search(str, result, contPunct_))
    return 4;
  else if (boost::regex_search(str, result, stopPunct_))
    return 5;
  else if (boost::regex_search(str, result, braces_))
    return 6;

  return 0;
}

int AcademicRegex::Count(const CiteMarker marker, const string& str) {
  int count = 0;
  if (!cite_marker_map_.count(marker))
    return count;

  const boost::regex& expression = cite_marker_map_[marker];

  boost::smatch result;
  string::const_iterator beg = str.begin();
  while (boost::regex_search(beg, str.end(), result, expression)) {
    count ++;
    beg = result[0].second;
    //string msg(result[1].first, result[1].second);
    //fprintf(stdout, "Rmatch<%d>: %s\n", (int)(result[0].first - str.begin()), msg.c_str());
  }

  return count;
}

pair<int, int> AcademicRegex::Search(const CiteMarker marker, const string& str, int pos) {
  pair<int, int> positions(-1, -1);
  if (!cite_marker_ext_.count(marker))
    return positions;

  const boost::regex& expression = cite_marker_ext_[marker];

  boost::smatch result;
  if (boost::regex_search(str.begin() + pos, str.end(), result, expression)) {
    positions.first = static_cast<int>(result[0].first - str.begin());
    positions.second = static_cast<int>(result[0].second - str.begin());
  }

  return positions;
}

bool AcademicRegex::IsDate(const string& str) {
  static const boost::regex date("[\\d]+(年|月)");

  string out;
  if (GetYearExt(str, &out))
    return true;

  boost::smatch result;
  return boost::regex_search(str, result, date);
}

int AcademicRegex::CountZh(const enum CiteMarker marker, const std::string& str) {
  int count = 0;
  if (!zh_cite_marker_map_.count(marker))
    return count;

  const boost::regex& expression = zh_cite_marker_map_[marker];

  boost::smatch result;
  string::const_iterator beg = str.begin();
  while (boost::regex_search(beg, str.end(), result, expression)) {
    count ++;
    beg = result[0].second;
    //string msg(result[1].first, result[1].second);
    //fprintf(stdout, "Rmatch<%d>: %s\n", (int)(result[0].first - str.begin()), msg.c_str());
  }

  return count;
}

pair<int, int> AcademicRegex::SearchZh(const CiteMarker marker, const string& str, int pos) {
  pair<int, int> positions(-1, -1);
  if (!zh_cite_marker_ext_.count(marker))
    return positions;

  const boost::regex& expression = zh_cite_marker_ext_[marker];

  boost::smatch result;
  if (boost::regex_search(str.begin() + pos, str.end(), result, expression)) {
    positions.first = static_cast<int>(result[0].first - str.begin());
    positions.second = static_cast<int>(result[0].second - str.begin());
  }

  return positions;
}

}  // end namespace

