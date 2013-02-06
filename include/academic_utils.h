/*
 * academic_utils.h
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#ifndef ACADEMIC_UTILS_H_
#define ACADEMIC_UTILS_H_

#include <cstdio>
#include <string>
#include <vector>

namespace academic {

const int MAX_LEMMA_NUM = 128;
const int MAX_LEMMA_SIZE = 32;

const int MAX_HEADER_PROP = 100;
const int MIN_HEADER_SIZE = 40;

const int MAX_ABSTRACT_SIZE = 1000;
const int MAX_KEYWORDS_SIZE = 120;

const int MAX_ZH_ABSTRACT_WORD = 300;
const int MAX_ZH_KEYWORDS_WORD = 40;

class AcademicUtils {
public:
  static int mm_str2int(char* pstr) {
    int prop = 0;
    while(isdigit(*pstr)) {
      prop = 10 * prop + (*pstr - '0');
      pstr ++;
    }
    return prop;
  }

  static void mm_int2str(int prop, char* pstr) {
    sprintf(pstr, "%d", prop);
  }

  static int GetCharBytes(const char c);

  static void SplitText(const std::string& text, const std::string& tag, std::vector<std::string>* output);

  static void SplitText2(const std::string& text, const std::string& tag, std::vector<std::string>* output);

  static void NormalizeText(const std::string& text, std::string* output);

  static void NormalizeTextZh(const std::string& text, std::string* output);

  static int NormalMatch(const std::string& text, const std::string& str);

  static void MergeLinesByComma(std::vector<std::string>* plines);

  static void MergeLinesByDash(std::string* ptext);

  static void MergeWordsByDash(std::string* ptext);

  static void MergeRedundantSpace(std::string* ptext);

  static int FuzzyEditDistance(const std::string& a, const std::string& b);

  static int GetChinaEuropeMargin(const std::string& text);

  static bool RemoveBegEndPunc(const std::string& text, std::string* output);
};

}


#endif /* ACADEMIC_UTILS_H_ */
