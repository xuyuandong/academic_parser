/*
 * academic_trainer.h
 *
 *  Created on: 2012-1-17
 *      Author: x
 */

#ifndef ACADEMIC_TRAINER_H_
#define ACADEMIC_TRAINER_H_

#include <string>

namespace academic {

class AcademicTrainer {
public:
  virtual ~AcademicTrainer() {}

  virtual int TrainHeader(const std::string& text, FILE* fp) = 0;
  virtual int TrainCitation(const std::string& text, FILE* fp) = 0;
};


}


#endif /* ACADEMIC_TRAINER_H_ */
