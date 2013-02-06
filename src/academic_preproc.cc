/*
 * academic_preproc.cc
 *
 *  Created on: 2012-1-12
 *      Author: x
 */

#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
#include "base/logging.h"
#include "ext_libs/search/public/mmatch.h"
#include "academic_utils.h"
#include "academic_preproc.h"

using namespace std;
using namespace extlibs;  // NOLINT

namespace academic {

int AcademicPreproc::SearchDictionary(mm_dict_t* dict, const string& text) {
  mm_pack_t* ppack = mm_pack_create(MAX_LEMMA_NUM);
  int ret = mm_search(dict, ppack, text.c_str(), text.size());
  if (ret < 0 || ppack->ppseg_cnt == 0) {
    LOG(INFO) << "No keys in dict found";
    mm_pack_del(ppack);
    return -1;
  }

  keyposList_.clear();
  for (int i = 0; i < ppack->ppseg_cnt; ++i) {
    if (ppack->ppseg[i]->prop < MAX_HEADER_PROP) {
      keyposList_.push_back(KeyPos(ppack->ppseg[i]->prop, ppack->poff[i], ppack->ppseg[i]->len));
      LOG(INFO) << "find " << ppack->ppseg[i]->prop << " " << ppack->ppseg[i]->pstr << " " <<ppack->poff[i];
    }
  }

  std::sort(keyposList_.begin(), keyposList_.end(), KeyPos::Compare);
  for (int i = 0; i < keyposList_.size(); ++i) {
    LOG(INFO) << "key pos " << keyposList_[i].prop_ << " " << keyposList_[i].poff_;
  }

  mm_pack_del(ppack);
  return 0;
}


}

