/*
 * academic_postproc.h
 *
 *  Created on: 2012-1-11
 *      Author: x
 */

#ifndef ACADEMIC_POSTPROC_H_
#define ACADEMIC_POSTPROC_H_

#include <cstdio>
#include <string>
#include <vector>

namespace academic {

class AcademicPaper;

class AcademicPostproc {
public:
  explicit AcademicPostproc(boost::shared_ptr<AcademicPaper> paper) : paper_(paper) {};
  virtual ~AcademicPostproc(){};

  virtual void ProcessHeader() = 0;
  virtual void ProcessCitation() = 0;

protected:
  virtual void FormatHeaderToWordList() = 0;
  virtual void ReviseHeaderProperty() = 0;
  virtual void NormalizeHeaderToPaper() = 0;

  virtual void FormatCitationToWordList(int idx) = 0;
  virtual void ReviseCitationProperty(int idx) = 0;
  virtual void NormalizeCitationToPaper(int idx) = 0;

  boost::shared_ptr<AcademicPaper> paper_;
};

}

#endif /* ACADEMIC_POSTPROC_H_ */
