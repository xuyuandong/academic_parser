/*
 * academic_crfpp.cc
 *
 *  Created on: 2012-1-16
 *      Author: x
 */

#include "base/logging.h"
#include "third_party/crfpp/crfpp.h"
#include "academic_crfpp.h"
#include "ext_libs/base/public/conf.h"

using namespace std;
using namespace extlibs;

namespace academic {

AcademicCrfpp::AcademicCrfpp(int argc, char* argv[]) {
  tagger_ = CRFPP::createTagger(argc, argv);
  CHECK(tagger_ != NULL) << "failed to create CRF tagger";
}

AcademicCrfpp::~AcademicCrfpp() {
  delete tagger_;
  tagger_ = NULL;
}

void AcademicCrfpp::Parse(const string& input, string* output) {
  const char* result = tagger_->parse(input.c_str(), input.size());
  *output = result;
}

}


