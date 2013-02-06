/*
 * academic_trainer_english.h
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#ifndef ACADEMIC_TRAINER_ENGLISH_H_
#define ACADEMIC_TRAINER_ENGLISH_H_

#include <string>
#include "academic_trainer.h"

namespace extlibs {
  class Conf;
}

namespace academic {

class AcademicTrainerEnglish : public AcademicTrainer {
  public:
  explicit AcademicTrainerEnglish(extlibs::Conf* config);
  virtual ~AcademicTrainerEnglish() {}

  virtual int TrainHeader(const std::string& text, FILE* fp);
  virtual int TrainCitation(const std::string& text, FILE* fp);
};


}


#endif /* ACADEMIC_TRAINER_ENGLISH_H_ */
