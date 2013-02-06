/*
 * academic_decoder_english.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_DECODER_ENGLISH_H_
#define ACADEMIC_DECODER_ENGLISH_H_


#include <vector>
#include <string>
#include "academic_decoder.h"

namespace extlibs {
  class Conf;
}

namespace academic {

class AcademicCrfpp;
class AcademicPaper;

class AcademicDecoderEnglish : public AcademicDecoder {
public:
  explicit AcademicDecoderEnglish(boost::shared_ptr<AcademicPaper> paper, extlibs::Conf* config);
  virtual ~AcademicDecoderEnglish();

  virtual void DecodeHeader(const std::string& text);
  virtual void DecodeCitation(const std::vector<std::string>& items);
};

}

#endif /* ACADEMIC_DECODER_ENGLISH_H_ */
