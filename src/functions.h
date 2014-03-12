#include <stdio.h>
#include <string.h>
#include <memory>
#ifndef _WIN32
    #include <libgen.h>
    #include <dirent.h>
#endif
#include <stdlib.h>
#include <stdexcept>

#include "AudioStreamInput.h"
#include "Metadata.h"
#include "Codegen.h"
#include <string>


typedef struct {
    char *error;
    char *filename;
    int start_offset;
    int duration;
    int tag;
    double t1;
    double t2;
    int numSamples;
    Codegen* codegen;
} codegen_response_t;

codegen_response_t *codegen_file(char*, int, int, int);
std::string escape(const string& value);
char *make_json_string(codegen_response_t* response);