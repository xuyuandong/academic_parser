/*
 * academic_feature.h
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#ifndef ACADEMIC_REFERENCE_H_
#define ACADEMIC_REFERENCE_H_

#include <string>
#include <vector>

namespace academic {

class ReferenceLine {
public:
  void SetText(const std::string& text);
  void Tokenize(std::string* features);
  void PrintDebug();

  std::string label_;
  std::vector<std::string> words_;
};

class ReferenceToken {
public:
  void Clear();
  void Extract(ReferenceLine* line, int idx);
  void ToString(const std::string& token, std::string* output);

private:
  void GetProto(const std::string& token, std::string* proto);
  void RemoveAllPunct(const std::string& token, std::string* proto);
  void RemoveSidePunct(const std::string& token, std::string* proto);
  void GetPunctuation(const std::string& token, std::string* prepunc, std::string* postpunc);
};


}


#endif /* ACADEMIC_FEATURE_H_ */
