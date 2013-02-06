/*
 * academic_header_chinese.h
 *
 *  Created on: 2012-3-1
 *      Author: x
 */

#ifndef ACADEMIC_HEADER_CHINESE_H_
#define ACADEMIC_HEADER_CHINESE_H_

#include "third_party/boost/boost/shared_ptr.hpp"

namespace segmenter {
  class GswsSegmenter;
}

namespace academic {

class PosTagger;

class ChineseToken {
public:
  ChineseToken();
  void ToString(std::string* output);

  bool bName_;
  bool bPlace_;
  bool bOrgan_;

  bool subName_;
  bool subPlace_;
  bool subOrgan_;

  bool bCJK_;
  bool bSTOP_;
  bool bPUNCT_;
  bool bPHRASE_;

  int eng_;
  int digit_;
  bool bCap_;

  std::string pos_;
  std::string word_;
  std::vector<std::string> subwords_;
};

class ChineseHeaderLine {
public:
  void SetText(const std::string& text);
  void Tokenize(std::string* features);
  void PrintDebug();

  std::string label_;
  std::vector<ChineseToken> words_;

  boost::shared_ptr<segmenter::GswsSegmenter> seg_;
  boost::shared_ptr<PosTagger> tag_;
};

}


#endif /* ACADEMIC_HEADER_CHINESE_H_ */
