/*
 * academic_postproc_english.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_POSTPROC_ENGLISH_H_
#define ACADEMIC_POSTPROC_ENGLISH_H_

#include <cstdio>
#include <string>
#include <vector>
#include "academic_postproc.h"

namespace academic {

class AcademicPaper;

class AcademicPostprocEnglish : public AcademicPostproc {
public:
  explicit AcademicPostprocEnglish(boost::shared_ptr<AcademicPaper> paper);
  virtual ~AcademicPostprocEnglish();

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
  enum ReviseEvent {
    RDEFAULT = -1,
    REMAIL = 0,
    RHTTP = 1
  };
  void HeaderInnerLineRevise(enum ReviseEvent event, int emailpos, int beg, int end);

  enum LinePos {
    LBEGIN = 0,
    LMIDDLE = 1,
    LENDLE = 2,
    LALONE = 3
  };

  class WordProperty {
  public:
    void PrintDebug(){
      fprintf(stdout, "%s<%d>: %d %d %d %d\n", word_.c_str(),
          label_, linepos_, bemail_, bhttp_, byear_);
    }
    int label_;
    int bhttp_;
    int bemail_;
    int byear_;
    LinePos linepos_;
    std::string word_;
  };

  std::vector<WordProperty> wordlist_;
};

}


#endif /* ACADEMIC_POSTPROC_ENGLISH_H_ */
