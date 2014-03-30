#include <stdio.h>
#include <string.h>
#include <memory>
#include <jni.h>
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
#include<string.h>
#include "functions.h"

using namespace std;

char *make_short_json_string(codegen_response_t* response) {
    
    if (response->error != NULL) {
        return response->error;
    }
    
    // preamble + codelen
    char* output = (char*) malloc(sizeof(char)*(16384 + strlen(response->codegen->getCodeString().c_str()) ));

    sprintf(output,"{\"metadata\":{\"filename\":\"%s\", \"samples_decoded\":%d, \"given_duration\":%d,"
                    " \"start_offset\":%d, \"version\":%2.2f, \"codegen_time\":%2.6f, \"decode_time\":%2.6f}, \"code_count\":%d,"
                    " \"code\":\"%s\", \"tag\":%d}",
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

extern "C"
JNIEXPORT jstring JNICALL 
Java_com_playax_fingerprint_Echoprint_code(JNIEnv *env, jobject obj, jstring fileName) {
    const char *nativeString = env->GetStringUTFChars(fileName, 0);
    char* cFilename = const_cast<char *>(nativeString); 
 
 	codegen_response_t* response = codegen_file(cFilename, 0, 0, 0);
	
	char *output = make_short_json_string(response);
	 
    env->ReleaseStringUTFChars(fileName, nativeString);

	jstring result = env->NewStringUTF(output);

	free(output); 
    free(response);

	return result; // result does not need output
}

