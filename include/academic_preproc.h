/*
 * academic_prepare.h
 *
 *  Created on: 2012-1-11
 *      Author: x
 */

#ifndef ACADEMIC_PREPROC_H_
#define ACADEMIC_PREPROC_H_

#include <vector>
#include <string>
#include "academic_regex.h"

namespace extlibs {
  typedef struct _mm_dict mm_dict_t;
}

namespace academic {

class AcademicPaper;

class AcademicPreproc {
public:
  explicit AcademicPreproc(boost::shared_ptr<AcademicPaper> paper) : paper_(paper) {}
  virtual ~AcademicPreproc(){}

  virtual int GetHeaderPage(const std::vector<std::string>& pages) = 0;
  virtual int FindHeader(const std::string& text, std::string* header) = 0;

  virtual int RecallSummary(const std::vector<std::string>& textvec, int headpage) = 0;

  virtual int FindCitation(const std::vector<std::string>& textvec, std::string* citation) = 0;
  virtual int SplitCitation(const std::string& citation, std::vector<std::string>* items) = 0;

protected:
  virtual int SearchDictionary(extlibs::mm_dict_t* dict, const std::string& text);

  virtual enum CiteMarker GuessMarkerType(const std::string& text) = 0;
  virtual void SplitUnmarkedCitation(const std::string& citation, std::vector<std::string>* items) = 0;
  virtual void SplitCitationByMarker(const std::string& citation, enum CiteMarker, std::vector<std::string>* items) = 0;

  class KeyPos {
  public:
    KeyPos(int prop, int poff, int len) : prop_(prop), poff_(poff), keylen_(len) {}
    static bool Compare(const KeyPos& a, const KeyPos& b) {
      return a.poff_ < b.poff_;
    }
    int prop_;
    int poff_;
    int keylen_;
  };

  std::vector<KeyPos> keyposList_;
  boost::shared_ptr<AcademicPaper> paper_;
};

}


#endif /* ACADEMIC_PREPROC_H_ */
