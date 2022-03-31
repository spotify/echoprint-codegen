//
//  echoprint-codegen
//  Copyright 2011 The Echo Nest Corporation. All rights reserved.
//

#ifndef AUDIOBUFFERINPUT_H
#define AUDIOBUFFERINPUT_H

#include "Common.h"
#include <iostream>
#include <string>
#include "AudioStreamInput.h"


class AudioBufferInput  : public AudioStreamInput {
 public:
  AudioBufferInput();
  std::string GetName() {return "direct buffer";}
  void SaveBuffer(const char*filename);
  void SetBuffer(const float* pBuffer, uint numSamples);
 protected:
  std::string GetCommandLine(const char*)
  {return "";}
};

#endif



