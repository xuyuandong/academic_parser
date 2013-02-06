/*
 * academic_decoder_english.cc
 *
 *  Created on: 2012-2-29
 *      Author: x
 */

#include <cstdio>
#include <string>
#include <vector>
#include "base/logging.h"
#include "academic_data.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "academic_crfpp.h"
#include "academic_decoder_english.h"
#include "academic_header_english.h"
#include "academic_reference_english.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;

namespace academic {

AcademicDecoderEnglish::AcademicDecoderEnglish(boost::shared_ptr<AcademicPaper> paper, Conf* config)
  : AcademicDecoder(paper) {
  char value[WORD_SIZE + 1];
  *value = '\0';

  // argument
  char* argv[3];
  argv[0] = const_cast<char*>("crf_test");
  argv[1] = const_cast<char*>("-m");
  argv[2] = value;

  // load header model
  int ret = config->ConfString(const_cast<char*>("EnHeaderModelPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find EnHeaderModelPath in config";

  crfheader_.reset(new(nothrow) AcademicCrfpp(3, argv));
  CHECK(crfheader_.get() != NULL) << "failed to create CRF header";

  // load citation model
  ret = config->ConfString(const_cast<char*>("EnCitationModelPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find EnCitationModelPath in config";

  crfcitation_.reset(new(nothrow) AcademicCrfpp(3, argv));
  CHECK(crfheader_.get() != NULL) << "failed to create CRF citation";
}

AcademicDecoderEnglish::~AcademicDecoderEnglish() {

}

void AcademicDecoderEnglish::DecodeHeader(const string& text) {
  vector<string> lines;
  AcademicUtils::SplitText(text, "\n", &lines);

  vector<HeaderLine> linefeats;
  linefeats.reserve(lines.size());

  for (int i = 0; i < lines.size(); ++i) {
    linefeats.push_back(HeaderLine());
    HeaderLine& linefeat = linefeats.back();
    linefeat.label_ = "";
    linefeat.linebeg_ = true;
    linefeat.lineend_ = true;
    linefeat.SetText(lines[i]);
  }

  string crfppstr;
  for (int i = 0; i < linefeats.size(); ++i)
    linefeats[i].Tokenize(&crfppstr);

  // fprintf(stdout, "%s\n", crfppstr.c_str());
  // fflush(stdout);

  string crfppout;
  crfheader_->Parse(crfppstr, &crfppout);
  //fprintf(stdout, "%s\n", crfppout.c_str());

  paper_->crf_str_ = crfppout;
}

void AcademicDecoderEnglish::DecodeCitation(const vector<string>& items) {
  for (int i = 0; i < items.size(); ++i) {
    ReferenceLine rline;
    rline.label_ = "";
    rline.SetText(items[i]);

    string crfppstr;
    rline.Tokenize(&crfppstr);

    //fprintf(stdout, "%s\n", crfppstr.c_str());
    //fflush(stdout);

    string crfppout;
    crfcitation_->Parse(crfppstr, &crfppout);
    //fprintf(stdout, "\n%s\n", crfppout.c_str());

    paper_->crf_vec_.push_back(crfppout);
  }
}


}


