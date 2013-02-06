/*
 * academic_postproc_chinese.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_POSTPROC_CHINESE_H_
#define ACADEMIC_POSTPROC_CHINESE_H_

#include <cstdio>
#include <string>
#include <vector>
#include "academic_postproc.h"

namespace academic {

class AcademicPaper;

class AcademicPostprocChinese : public AcademicPostproc {
public:
  explicit AcademicPostprocChinese(boost::shared_ptr<AcademicPaper> paper);
  virtual ~AcademicPostprocChinese();

  virtual void ProcessHeader();
  virtual void ProcessCitation();

protected:
  virtual void FormatHeaderToWordList();
  virtual void ReviseHeaderProperty();
  virtual void NormalizeHeaderToPaper();

  virtual void FormatCitationToWordList(int idx);
  virtual void ReviseCitationProperty(int idx);
  virtual void NormalizeCitationToPaper(int idx);

private:
  class WordProperty {
    public:
      void PrintDebug() {}
      int label_;
      bool name_;
      bool phrase_;
      std::string word_;
  };

  std::vector<WordProperty> wordlist_;
};

}


#endif /* ACADEMIC_POSTPROC_CHINESE_H_ */
