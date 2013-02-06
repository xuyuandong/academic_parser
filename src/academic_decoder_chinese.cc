/*
 * academic_decoder_chinese.cc
 *
 *  Created on: 2012-2-29
 *      Author: x
 */


#include <cstdio>
#include <string>
#include <vector>
#include "base/logging.h"
#include "word_segmentor/public/gswssegmenter.h"
#include "academic_data.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_crfpp.h"
#include "academic_postagger.h"
#include "academic_header_chinese.h"
#include "academic_decoder_chinese.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;
using namespace segmenter;

namespace academic {

AcademicDecoderChinese::AcademicDecoderChinese(boost::shared_ptr<AcademicPaper> paper, Conf* config)
  : AcademicDecoder(paper) {
  char value[WORD_SIZE + 1];
  *value = '\0';

  // argument
  char* argv[3];
  argv[0] = const_cast<char*>("crf_test");
  argv[1] = const_cast<char*>("-m");
  argv[2] = value;

  // load header model
  int ret = config->ConfString(const_cast<char*>("ZhHeaderModelPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find ZhHeaderModelPath in config";

  crfheader_.reset(new(nothrow) AcademicCrfpp(3, argv));
  CHECK(crfheader_.get() != NULL) << "failed to create CRF header";

  // load citation model
  ret = config->ConfString(const_cast<char*>("ZhCitationModelPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find ZhCitationModelPath in config";

  crfcitation_.reset(new(nothrow) AcademicCrfpp(3, argv));
  CHECK(crfheader_.get() != NULL) << "failed to create CRF citation";

  // word segmenter
  ret = config->ConfString(const_cast<char*>("WordSegmentDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find WordSegmentDictPath in config";

  wordseg_.reset(new(nothrow) GswsSegmenter("gsws", value));
  CHECK(wordseg_.get() != NULL) << "failed to create word segmenter";
  CHECK(wordseg_->Init()) << "failed to initialize word segmenter";

  // pos tagger
  ret = config->ConfString(const_cast<char*>("PosTaggerDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find PosTaggerDictPath in config";

  postag_.reset(new(nothrow) PosTagger(value));
  CHECK(postag_.get() != NULL) << "failed to create pos tagger";
}

AcademicDecoderChinese::~AcademicDecoderChinese() {

}

void AcademicDecoderChinese::DecodeHeader(const string& text) {
  ChineseHeaderLine linefeat;
  linefeat.label_ = "";

  linefeat.seg_ = wordseg_;
  linefeat.tag_ = postag_;
  linefeat.SetText(text);

  string crfppstr;
  linefeat.Tokenize(&crfppstr);
  //fprintf(stdout, "%s\n\n\n", crfppstr.c_str());
  //fflush(stdout);

  string crfppout;
  crfheader_->Parse(crfppstr, &crfppout);
  //fprintf(stdout, "%s\n\n", crfppout.c_str());

  paper_->crf_str_ = crfppout;
}

void AcademicDecoderChinese::DecodeCitation(const vector<string>& items) {
  ChineseHeaderLine linefeat;
  linefeat.seg_ = wordseg_;
  linefeat.tag_ = postag_;
  
  for (int i = 0; i < items.size(); ++i) {
    linefeat.label_ = "";

    //TODO: judge english or chinese, choose feature
    linefeat.SetText(items[i]);

    string crfppstr;
    linefeat.Tokenize(&crfppstr);

    string crfppout;
    crfcitation_->Parse(crfppstr, &crfppout);
    //fprintf(stdout, "\n%s\n\n", crfppout.c_str());

    paper_->crf_vec_.push_back(crfppout);
  }
}

}

