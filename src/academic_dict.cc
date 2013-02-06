/*
 * academic_dict.cc
 *
 *  Created on: 2012-2-3
 *      Author: x
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "base/logging.h"
#include "base/string_util.h"
#include "academic_dict.h"
#include "academic_utils.h"
#include "ext_libs/base/public/conf.h"
#include "ext_libs/search/public/mmatch.h"

using namespace std;
using namespace extlibs;

namespace academic {

#define MAX_LINE_SIZE 1024

mm_dict_t* AcademicDict::en_header_dict_ = NULL;
mm_dict_t* AcademicDict::zh_header_dict_ = NULL;

mm_dict_t* AcademicDict::en_citation_dict_ = NULL;
mm_dict_t* AcademicDict::zh_citation_dict_ = NULL;

mm_dict_t* AcademicDict::en_author_div_char_ = NULL;

base::hash_set<string> AcademicDict::name_dict_;
base::hash_set<string> AcademicDict::addr_dict_;
base::hash_set<string> AcademicDict::date_dict_;
base::hash_set<string> AcademicDict::publisher_dict_;

base::hash_map<string, int> AcademicDict::head_tag_dict_;
base::hash_map<string, int> AcademicDict::cite_tag_dict_;

base::hash_map<string, string> AcademicDict::char_dict_;
base::hash_set<string> AcademicDict::cond_punct_dict_;
base::hash_set<string> AcademicDict::stop_punct_dict_;

void AcademicDict::Init(Conf* config) {
  char value[WORD_SIZE + 1];  *value = '\0';

  int ret = config->ConfString(const_cast<char*>("TagsDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find TagsDictPath in config";
  LoadTags(value);
  
  ret = config->ConfString(const_cast<char*>("NameDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find NameDictPath in config";
  LoadName(value);

  ret = config->ConfString(const_cast<char*>("AddressDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find AddressDictPath in config";
  LoadAddress(value);

  ret = config->ConfString(const_cast<char*>("DateDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find DateDictPath in config";
  LoadDate(value);

  ret = config->ConfString(const_cast<char*>("PublisherDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find PublisherDictPath in config";
  LoadPublisher(value);

  ret = config->ConfString(const_cast<char*>("EnAuthorDivCharPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find EnAuthorDivCharPath in config";
  LoadAuthorDivChar(value);

  ret = config->ConfString(const_cast<char*>("CharMapDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find CharMapDictPath in config";
  LoadCharMap(value);

  ret = config->ConfString(const_cast<char*>("PunctSetDictPath"),
      value, sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find PunctSetDictPath in config";
  LoadPunctSet(value);

  // keywords
  LoadHeaderAndCitation(config);
}

void AcademicDict::Release() {
  mm_dict_del(en_header_dict_);
  mm_dict_del(zh_header_dict_);

  mm_dict_del(en_citation_dict_);
  mm_dict_del(zh_citation_dict_);

  mm_dict_del(en_author_div_char_);
}

void AcademicDict::LoadTags(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  int idx = 0;
  char buf[MAX_LINE_SIZE];

  // header
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    ++idx;
    if (strstr(buf, "#citation tags"))
      break;
    if (buf[0] == '#')
      continue;

    string line(buf);
    vector<string> keyval;
    AcademicUtils::SplitText2(line, "\t\n", &keyval);
    CHECK(keyval.size() == 2) << "bad format for line " << idx << " in " << fname;

    head_tag_dict_[keyval[0]] = StringToInt(keyval[1]);
  }

  // citation
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    ++idx;
    if (buf[0] == '#')
      continue;

    string line(buf);
    vector<string> keyval;
    AcademicUtils::SplitText2(line, "\t\n", &keyval);
    CHECK(keyval.size() == 2) << "bad format for line " << idx << " in " << fname;

    cite_tag_dict_[keyval[0]] = StringToInt(keyval[1]);
  }

  fclose(fin);
}

void AcademicDict::LoadCharMap(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  int idx = 0;
  char buf[MAX_LINE_SIZE];
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    idx = idx + 1;
    if (buf[0] == '#')
      continue;

    string line(buf);
    vector<string> keyval;
    AcademicUtils::SplitText2(line, "\t\n", &keyval);
    CHECK(keyval.size() == 2) << "bad format for line " << idx << " in " << fname;

    char_dict_[keyval[0]] = keyval[1];
  }

  fclose(fin);
}

void AcademicDict::LoadPunctSet(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  char buf[MAX_LINE_SIZE];

  // conduct punctuation
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    if (strstr(buf, "#cond") != NULL)
      continue;
    if (strstr(buf, "#stop") != NULL)
      break;

    int bytes = AcademicUtils::GetCharBytes(buf[0]);
    string line(buf, 0, bytes);
    cond_punct_dict_.insert(line);
  }

  // stop punctuation
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    int bytes = AcademicUtils::GetCharBytes(buf[0]);
    string line(buf, 0, bytes);
    stop_punct_dict_.insert(line);
  }

  fclose(fin);
}

void AcademicDict::LoadHeaderAndCitation(Conf* config) {
  char value[WORD_SIZE + 1];  *value = '\0';
  mm_prop_str2int_p = AcademicUtils::mm_str2int;
  mm_prop_int2str_p = AcademicUtils::mm_int2str;

  // header
  int ret = config->ConfString(const_cast<char*>("EnHeaderDictPath"), value,
      sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find EnHeaderDictPath in config";
  en_header_dict_ = mm_dict_load(value, MAX_LEMMA_NUM);

  ret = config->ConfString(const_cast<char*>("ZhHeaderDictPath"), value,
      sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find ZhHeaderDictPath in config";
  zh_header_dict_ = mm_dict_load(value, MAX_LEMMA_NUM);

  // citation
  ret = config->ConfString(const_cast<char*>("EnCitationDictPath"), value,
      sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find EnCitationDictPath in config";
  en_citation_dict_ = mm_dict_load(value, MAX_LEMMA_NUM);

  ret = config->ConfString(const_cast<char*>("ZhCitationDictPath"), value,
      sizeof(value), ConfItemAction::GET);
  CHECK(ret > 0) << "failed to find ZhCitationDictPath in config";
  zh_citation_dict_ = mm_dict_load(value, MAX_LEMMA_NUM);
}

void AcademicDict::LoadName(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  char buf[MAX_LINE_SIZE];
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    if (buf[0] == '#')
      continue;

    char* p = buf;
    while(*p != '\0') {
      if (*p == ' ' || *p == '\n'){
        name_dict_.insert(string(buf, p - buf));
        break;
      }

      if (isupper(*p))
        *p = *p - 'A' + 'a';
      ++p;
    }
  }

  fclose(fin);
}

void AcademicDict::LoadDate(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  char buf[MAX_LINE_SIZE];
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    if (buf[0] == '#')
      continue;

    char* p = buf;
    while(*p != '\0') {
      if (*p == ' ' || *p == '\n'){
        date_dict_.insert(string(buf, p - buf));
        break;
      }
      ++p;
    }
  }

  fclose(fin);
}

void AcademicDict::LoadAddress(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  char buf[MAX_LINE_SIZE];
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    if (buf[0] == '#')
      continue;

    char* p = buf;
    while(*p != '\0') {
      if (*p == ' ' || *p == '\n'){
        addr_dict_.insert(string(buf, p - buf));
        break;
      }
      ++p;
    }
  }

  fclose(fin);
}

void AcademicDict::LoadPublisher(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open " << fname;

  char buf[MAX_LINE_SIZE];
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    if (buf[0] == '#')
      continue;

    char* p = buf;
    while(*p != '\0') {
      if (*p == ' ' || *p == '\n'){
        publisher_dict_.insert(string(buf, p - buf));
        break;
      }
      ++p;
    }
  }

  fclose(fin);
}

void AcademicDict::LoadAuthorDivChar(const char* fname) {
  FILE* fin = fopen(fname, "r");
  CHECK (fin != NULL) << "failed to open en_author_div_char";

  mm_lemma_t lemma;
  en_author_div_char_ = mm_dict_create(MAX_LEMMA_NUM);

  char buf[MAX_LINE_SIZE];
  memset(buf, 0, sizeof(buf));
  while(fgets(buf, MAX_LINE_SIZE, fin)) {
    if (buf[0] == '#')
      continue;

    lemma.pstr = buf;
    lemma.len = strlen(buf) - 1;
    mm_add_lemma(en_author_div_char_, &lemma);
  }

  fclose(fin);
}


}
