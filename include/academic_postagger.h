/*
 * academic_postagger.h
 *
 *  Created on: 2012-3-2
 *      Author: x
 */

#ifndef ACADEMIC_POSTAGGER_H_
#define ACADEMIC_POSTAGGER_H_

#include <vector>
#include <string>

namespace segmenter {
  class GswsSegmenter;
}

namespace nlp {
  typedef struct _pos_dict pos_dict_t;
}

namespace academic {

class PosTagger {
public:
  explicit PosTagger(const char* dict);
  ~PosTagger();

  void Tag(std::vector<std::string>* result, std::vector<segmenter::SegmentedToken>& tokens);

private:
  nlp::pos_dict_t* posdict_;
};

}


#endif /* ACADEMIC_POSTAGGER_H_ */
