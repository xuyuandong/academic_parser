/*
 * academic_decoder.h
 *
 *  Created on: 2012-1-11
 *      Author: x
 */

#ifndef ACADEMIC_DECODER_H_
#define ACADEMIC_DECODER_H_

#include <vector>
#include <string>
#include "third_party/boost/boost/shared_ptr.hpp"

namespace academic {

class AcademicCrfpp;
class AcademicPaper;

class AcademicDecoder {
public:
  explicit AcademicDecoder(boost::shared_ptr<AcademicPaper> paper) : paper_(paper) {};
  virtual ~AcademicDecoder(){};

  virtual void DecodeHeader(const std::string& text) = 0;
  virtual void DecodeCitation(const std::vector<std::string>& items) = 0;

protected:
  boost::shared_ptr<AcademicCrfpp> crfheader_;
  boost::shared_ptr<AcademicCrfpp> crfcitation_;

  boost::shared_ptr<AcademicPaper> paper_;
};

}

#endif /* ACADEMIC_DECODER_H_ */
