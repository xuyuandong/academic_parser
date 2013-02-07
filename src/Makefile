APP_PATH := $(PWD)

LIB_PATH := -L ./ -L /usr/lib64/ -L /usr/local/lib/ 

LIB := -lpthread
#-lprotobuf -ltcmalloc -lprofiler

INCLUDE := -I ./ -I /usr/include/ 

CXXFLAGS = -Wall -fPIC -O2 $(INCLUDE)

OBJS = academic_crfpp.o \
  academic_data.o \
  academic_decoder_chinese.o \
  academic_decoder_english.o \
  academic_dict.o \
  academic_factory.o \
  academic_feature.o \
  academic_header_chinese.o \
  academic_header_english.o \
  academic_parser_chinese.o \
  academic_parser_english.o \
  academic_postagger.o \
  academic_postproc_chinese.o \
  academic_postproc_english.o \
  academic_preproc.o \
  academic_preproc_chinese.o \
  academic_preproc_english.o \
  academic_reference_english.o \
  academic_regex.o \
  academic_trainer_chinese.o \
  academic_trainer_english.o \
  academic_utils.o \
  academic_wrapper.o 

LIBNAME=libacademic_parser.a

STLIB_MAKE_CMD=ar rcs $(LIBNAME)

$(LIBNAME): $(OBJS)
  $(STLIB_MAKE_CMD) $(OBJS)

clean:
  rm -rf $(LIBNAME) *.o
