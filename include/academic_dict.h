/*
 * AcademicDict.h
 *
 *  Created on: 2012-2-3
 *      Author: x
 */

#ifndef ACADEMIC_DICT_H_
#define ACADEMIC_DICT_H_

#include <string>
#include "base/hash_tables.h"

namespace extlibs {
  class Conf;
  typedef struct _mm_dict mm_dict_t;
}

namespace academic {

class AcademicDict {
public:
  static void Init(extlibs::Conf* config);
  static void Release();

  static int GetHeaderTag(const std::string& tag) {
    if (!head_tag_dict_.count(tag))
      return -1;
    return head_tag_dict_[tag];
  }

  static int GetCitationTag(const std::string& tag) {
    if (!cite_tag_dict_.count(tag))
      return -1;
    return cite_tag_dict_[tag];
  }

  static bool MatchName(const std::string& str) {
    return (name_dict_.count(str) != 0);
  }
  static bool MatchAddr(const std::string& str) {
    return (addr_dict_.count(str) != 0);
  }

  static bool MatchDate(const std::string& str) {
    return (date_dict_.count(str) != 0);
  }

  static bool MatchPublisher(const std::string& str) {
    return (publisher_dict_.count(str) != 0);
  }

  static extlibs::mm_dict_t* EnHeaderDict() {
    return en_header_dict_;
  }

  static extlibs::mm_dict_t* ZhHeaderDict() {
    return zh_header_dict_;
  }

  static extlibs::mm_dict_t* EnCitationDict() {
    return en_citation_dict_;
  }

  static extlibs::mm_dict_t* ZhCitationDict() {
    return zh_citation_dict_;
  }

  static extlibs::mm_dict_t* EnAuthorDivCharDict() {
    return en_author_div_char_;
  }

  static bool ConvertChar(const std::string& str, std::string* out) {
    if (!char_dict_.count(str))
      return false;
    *out = char_dict_[str];
    return true;
  }

  static bool CondPunctChar(const std::string& str) {
    return (cond_punct_dict_.count(str) != 0);
  }
  static bool StopPunctChar(const std::string& str) {
    return (stop_punct_dict_.count(str) != 0);
  }

private:
  static void LoadTags(const char* fname);

  static void LoadDate(const char* fname);
  static void LoadName(const char* fname);
  static void LoadAddress(const char* fname);
  static void LoadPublisher(const char* fname);

  static void LoadHeaderAndCitation(extlibs::Conf* config);

  static void LoadCharMap(const char* fname);
  static void LoadPunctSet(const char* fname);
  static void LoadAuthorDivChar(const char* fname);

  static extlibs::mm_dict_t* en_header_dict_;
  static extlibs::mm_dict_t* zh_header_dict_;

  static extlibs::mm_dict_t* en_citation_dict_;
  static extlibs::mm_dict_t* zh_citation_dict_;

  static extlibs::mm_dict_t* en_author_div_char_;

  static base::hash_set<std::string> name_dict_;
  static base::hash_set<std::string> addr_dict_;
  static base::hash_set<std::string> date_dict_;
  static base::hash_set<std::string> publisher_dict_;

  static base::hash_map<std::string, int> head_tag_dict_;
  static base::hash_map<std::string, int> cite_tag_dict_;

  static base::hash_map<std::string, std::string> char_dict_;

  static base::hash_set<std::string> cond_punct_dict_;
  static base::hash_set<std::string> stop_punct_dict_;
};

}

#endif /* ACADEMIC_DICT_H_ */
