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
#include "functions.h"

using namespace std;


// deal with quotes etc in json
std::string escape(const string& value) {
    std::string s(value);
    std::string out = "";
    out.reserve(s.size());
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        if ((unsigned char)c < 31)
            continue;

        switch (c) {
            case '"' : out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b" ; break;
            case '\f': out += "\\f" ; break;
            case '\n': out += "\\n" ; break;
            case '\r': out += "\\r" ; break;
            case '\t': out += "\\t" ; break;
            // case '/' : out += "\\/" ; break; // Unnecessary?
            default:
                out += c;
                // TODO: do something with unicode?
        }
    }

    return out;
}


codegen_response_t *codegen_file(char* filename, int start_offset, int duration, int tag) {
    // Given a filename, perform a codegen on it and get the response
    // This is called by a thread
    double t1 = now();
    codegen_response_t *response = (codegen_response_t *)malloc(sizeof(codegen_response_t));
    response->error = NULL;
    response->codegen = NULL;

    auto_ptr<FfmpegStreamInput> pAudio(new FfmpegStreamInput());
    pAudio->ProcessFile(filename, start_offset, duration);

    if (pAudio.get() == NULL) { // Unable to decode!
        char* output = (char*) malloc(16384);
        sprintf(output,"{\"error\":\"could not create decoder\", \"tag\":%d, \"metadata\":{\"filename\":\"%s\"}}",
            tag,
            escape(filename).c_str());
        response->error = output;
        return response;
    }

    int numSamples = pAudio->getNumSamples();

    if (numSamples < 1) {
        char* output = (char*) malloc(16384);
        sprintf(output,"{\"error\":\"could not decode\", \"tag\":%d, \"metadata\":{\"filename\":\"%s\"}}",
            tag,
            escape(filename).c_str());
        response->error = output;
        return response;
    }
    t1 = now() - t1;

    double t2 = now();
    Codegen *pCodegen = new Codegen(pAudio->getSamples(), numSamples, start_offset);
    t2 = now() - t2;
    
    response->t1 = t1;
    response->t2 = t2;
    response->numSamples = numSamples;
    response->codegen = pCodegen;
    response->start_offset = start_offset;
    response->duration = duration;
    response->tag = tag;
    response->filename = filename;
    
    return response;
}

char *make_json_string(codegen_response_t* response) {
    
    if (response->error != NULL) {
        return response->error;
    }
    
    // Get the ID3 tag information.
    auto_ptr<Metadata> pMetadata(new Metadata(response->filename));

    // preamble + codelen
    char* output = (char*) malloc(sizeof(char)*(16384 + strlen(response->codegen->getCodeString().c_str()) ));

    sprintf(output,"{\"metadata\":{\"artist\":\"%s\", \"release\":\"%s\", \"title\":\"%s\", \"genre\":\"%s\", \"bitrate\":%d,"
                    "\"sample_rate\":%d, \"duration\":%d, \"filename\":\"%s\", \"samples_decoded\":%d, \"given_duration\":%d,"
                    " \"start_offset\":%d, \"version\":%2.2f, \"codegen_time\":%2.6f, \"decode_time\":%2.6f}, \"code_count\":%d,"
                    " \"code\":\"%s\", \"tag\":%d}",
        escape(pMetadata->Artist()).c_str(),
        escape(pMetadata->Album()).c_str(),
        escape(pMetadata->Title()).c_str(),
        escape(pMetadata->Genre()).c_str(),
        pMetadata->Bitrate(),
        pMetadata->SampleRate(),
        pMetadata->Seconds(),
        escape(response->filename).c_str(),
        response->numSamples,
        response->duration,
        response->start_offset,
        response->codegen->getVersion(),
        response->t2,
        response->t1,
        response->codegen->getNumCodes(),
        response->codegen->getCodeString().c_str(),
        response->tag
    );
    return output;
}