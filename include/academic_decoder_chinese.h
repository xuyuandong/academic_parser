/*
 * academic_decoder_chinese.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_DECODER_CHINESE_H_
#define ACADEMIC_DECODER_CHINESE_H_

#include <vector>
#include <string>
#include "boost/boost/shared_ptr.hpp"
#include "academic_decoder.h"

namespace extlibs {
  class Conf;
}

namespace segmenter {
  class GswsSegmenter;
}

namespace academic {

class AcademicCrfpp;
class AcademicPaper;
class PosTagger;

class AcademicDecoderChinese : public AcademicDecoder {
public:
  explicit AcademicDecoderChinese(boost::shared_ptr<AcademicPaper> paper, extlibs::Conf* config);
  virtual ~AcademicDecoderChinese();

  virtual void DecodeHeader(const std::string& text);
  virtual void DecodeCitation(const std::vector<std::string>& items);

private:
  boost::shared_ptr<segmenter::GswsSegmenter> wordseg_;
  boost::shared_ptr<PosTagger> postag_;
};

}


#endif /* ACADEMIC_DECODER_CHINESE_H_ */
