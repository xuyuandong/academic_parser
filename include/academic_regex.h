////////////////////////////////////////////////////////////////////////
// Copyright (c) 2011 by Goso  ALL RIGHTS RESERVED.
// Consult your license regarding permissions and restrictions.
//
// header file for academic regex.
// Author: xuyuandong@jike.com
////////////////////////////////////////////////////////////////////////

#ifndef PAGE_ANALYSIS_ACADEMIC_PARSER_INTERNAL_ACADEMIC_REGEX_H_
#define PAGE_ANALYSIS_ACADEMIC_PARSER_INTERNAL_ACADEMIC_REGEX_H_

#include <map>
#include <string>
#include "base/hash_tables.h"
#include "third_party/boost/boost/regex.hpp"

namespace academic {

enum CiteMarker {
  UNKNOWN = 0,
  PAREN = 1,
  SQUARE = 2,
  BRACES = 3,
  NUMDOT = 4,
  NAKENUM = 5
};

struct HashMarker {
  size_t operator()(const CiteMarker& x) const {
    return static_cast<int>(x);
  }
};

class AcademicRegex {
 public:
  static void Init();
  static void Release();

  static bool IsBigStart(const std::string& str);

  static bool IsYear(const std::string& str);
  static bool IsYearExt(const std::string& str, std::pair<int, int>* pos = NULL);
  static bool GetYearExt(const std::string& str, std::string* date);

  static bool IsPage(const std::string& str);
  static bool IsVolume(const std::string& str);
  static bool IsOrdinal(const std::string& str);
  static bool IsEditors(const std::string& str);

  static int GetPunctType(const std::string& str);

  static int Count(const enum CiteMarker marker, const std::string& str);
  static std::pair<int, int> Search(const enum CiteMarker marker, const std::string& str, int pos = 0);

  static bool IsDate(const std::string& str);
  static int CountZh(const enum CiteMarker marker, const std::string& str);
  static std::pair<int, int> SearchZh(const enum CiteMarker marker, const std::string& str, int pos = 0);

 private:
  static base::hash_map<enum CiteMarker, boost::regex, HashMarker> cite_marker_map_;
  static base::hash_map<enum CiteMarker, boost::regex, HashMarker> cite_marker_ext_;

  static base::hash_map<enum CiteMarker, boost::regex, HashMarker> zh_cite_marker_map_;
  static base::hash_map<enum CiteMarker, boost::regex, HashMarker> zh_cite_marker_ext_;
};

}  // end namespace

#endif  // PAGE_ANALYSIS_ACADEMIC_PARSER_INTERNAL_ACADEMIC_REGEX_H_
