/*
 * academic_postagger.cc
 *
 *  Created on: 2012-3-2
 *      Author: x
 */

#include "base/logging.h"
#include "nlp_goso/postagger/public/postagger.h"
#include "word_segmentor/public/gswssegmenter.h"
#include "academic_postagger.h"

using namespace std;
using namespace segmenter;

namespace academic {

PosTagger::PosTagger(const char* dict) {
  posdict_ = nlp::pos_dict_load(dict);
  CHECK(posdict_ != NULL) << "failed to load pos dict";
}

PosTagger::~PosTagger() {
  nlp::pos_dict_del(posdict_);
  posdict_ = NULL;
}

void PosTagger::Tag(vector<string>* result, vector<SegmentedToken>& tokens) {
  nlp::pos_pack_t* ppack = nlp::pos_pack_create(tokens.size() + 1);

  nlp::pos_tag(posdict_, ppack, tokens.data(), tokens.size());

  result->clear();
  for (int i = 0; i < tokens.size(); ++i) {
    result->push_back(ppack->result[i]);
  }

  nlp::pos_pack_del(ppack);
}

}

