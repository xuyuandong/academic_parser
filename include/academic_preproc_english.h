/*
 * academic_preproc_english.h
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#ifndef ACADEMIC_PREPROC_ENGLISH_H_
#define ACADEMIC_PREPROC_ENGLISH_H_

#include <vector>
#include <string>
#include "academic_regex.h"
#include "academic_preproc.h"

namespace extlibs {
  typedef struct _mm_dict mm_dict_t;
}

namespace academic {

class AcademicPaper;

class AcademicPreprocEnglish : public AcademicPreproc {
public:
  explicit AcademicPreprocEnglish(boost::shared_ptr<AcademicPaper> paper);
  virtual ~AcademicPreprocEnglish();

  // English header
  virtual int GetHeaderPage(const std::vector<std::string>& pages);
  virtual int FindHeader(const std::string& text, std::string* header);

  virtual int RecallSummary(const std::vector<std::string>& textvec, int headpage);

  // English citation
  virtual int FindCitation(const std::vector<std::string>& textvec, std::string* citation);
  virtual int SplitCitation(const std::string& citation, std::vector<std::string>* items);

protected:
  virtual enum CiteMarker GuessMarkerType(const std::string& text);
  virtual void SplitUnmarkedCitation(const std::string& citation, std::vector<std::string>* items);
  virtual void SplitCitationByMarker(const std::string& citation, enum CiteMarker, std::vector<std::string>* items);

};

}

#endif /* ACADEMIC_PREPROC_ENGLISH_H_ */
