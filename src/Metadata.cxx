//
//  echoprint-codegen
//  Copyright 2011 The Echo Nest Corporation. All rights reserved.
//


#include "Metadata.h"
#include <fileref.h>
#include <tag.h>
#include <iostream>

Metadata::Metadata(const string& file) : _Filename(file), _Artist(""), _Album(""), _Title(""), _Genre(""), _Bitrate(0), _SampleRate(0), _Seconds(0) {
    if (file != "stdin") {
        // TODO: Consider removing the path from the filename -- not sure if we can do this in a platform-independent way.
        TagLib::FileRef f(_Filename.c_str());

        TagLib::Tag* tag = f.isNull() ? NULL : f.tag();
        if (tag != NULL) {
            _Artist = tag->artist().to8Bit(true);
            _Album = tag->album().to8Bit(true);
            _Title = tag->title().to8Bit(true);
            _Genre = tag->genre().to8Bit(true);
        }

        TagLib::AudioProperties* properties = f.isNull() ? NULL : f.audioProperties();
        if (properties != NULL) {
            _Bitrate = properties->bitrate();
            _SampleRate = properties->sampleRate();
            _Seconds = properties->length();
        }
    }
}

