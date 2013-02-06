/*
 * academic_trainer_chinese.h
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#ifndef ACADEMIC_TRAINER_CHINESE_H_
#define ACADEMIC_TRAINER_CHINESE_H_

#include <string>
#include "third_party/boost/boost/shared_ptr.hpp"
#include "academic_trainer.h"

namespace extlibs {
  class Conf;
}

namespace segmenter {
  class GswsSegmenter;
}

namespace academic {

class PosTagger;

class AcademicTrainerChinese : public AcademicTrainer {
public:
  explicit AcademicTrainerChinese(extlibs::Conf* config);
  virtual ~AcademicTrainerChinese() {}

  virtual int TrainHeader(const std::string& text, FILE* fp);
  virtual int TrainCitation(const std::string& text, FILE* fp);

private:
  boost::shared_ptr<segmenter::GswsSegmenter> wordseg_;
  boost::shared_ptr<PosTagger> postag_;
};


}


#endif /* ACADEMIC_TRAINER_chinese_H_ */
