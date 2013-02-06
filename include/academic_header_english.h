/*
 * academic_feature.h
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#ifndef ACADEMIC_FEATURE_H_
#define ACADEMIC_FEATURE_H_

#include <string>
#include <vector>

namespace academic {

class HeaderLine {
public:
  void SetText(const std::string& text);
  void Tokenize(std::string* features);
  void PrintDebug();

  std::string label_;
  bool linebeg_;
  bool lineend_;
  std::vector<std::string> words_;
};

class HeaderToken {
public:
  void Clear();
  void Extract(HeaderLine* line, int idx);
  void ToString(const std::string& token, std::string* output);

private:
  void GetProto(const std::string& token, std::string* proto);
  void GetRawProto(std::string* proto);
  void GetPunctuation(const std::string& token, std::string* prepunc,
      std::string* postpunc);

  bool BegCap_;
  bool HasCap_;
  bool AllCap_;

  bool BegNum_;
  bool EndNum_;
  bool HasNum_;
  bool AllNum_;

  bool CapDot_;
  bool HasDot_;
  bool HasAt_;

  bool BegPunc_;
  bool EndPunc_;
  bool HasPunc_;
  bool AllPunc_;

  bool AllWord_;

  bool HasBracket_;

  bool SingleNumber_;
  bool SingleLetter_;

  bool LineBeg_;
  bool LineMid_;
  bool LineEnd_;

  bool HasHttp_;
  bool HasName_;
  bool HasAddr_;

};


}


#endif /* ACADEMIC_FEATURE_H_ */
