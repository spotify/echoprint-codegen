# Codegen for Echoprint

**Please note: this is not the final version and will likely not emit codes that match release Echoprint codes.**

Echoprint is an open source music fingerprint and resolving framework powered by the [The Echo Nest](http://the.echonest.com/ "The Echo Nest"). The [code generator](http://github.com/echonest/echoprint-codegen "echoprint-codegen") (library to convert PCM samples from a microphone or file into Echoprint codes) is MIT licensed and free for any use. The [server component](http://github.com/echonest/echoprint-server "echoprint-server") that stores and resolves queries is Apache licensed and free for any use. The [data for resolving to millions of songs](http://echoprint.me/data "Echoprint Data") is free for any use provided any changes or additions are merged back to the community. 

There are two modes of operation of the Echoprint codegen:

1. the codegen library (libcodegen) is meant to be linked into code that passes it a buffer of PCM data and will output a code string.
 
2. the codegen binary runs standalone, accepts filenames as inputs and runs in a multithreaded worker mode.

## Requirements for the codegen library

* Boost >= 1.35

## Additional requirements for the codegen binary

* Taglib (we provide compiled libraries for Linux and OS X)
* ffmpeg - this is called via shell and is not linked into codegen

## Notes

You only need to query for 20 seconds of audio (or less sometimes) to get a result.

## Notes about libcodegen:

Code generation takes a buffer of floating point PCM data sampled at 11025 Hz and mono. 

    Codegen * pCodegen = new Codegen(const float* pcm, uint numSamples, int start_offset, bool full, bool lowrank);

    pcm: a buffer of floats, mono, 11025 Hz
    numSamples: the number of samples
    start_offset: creates a hint to the server on where the sample is taken from in the original file if known
    full: creates "full" Echoprint codes (roughly 100Hz, matches in the air)
    lowrank: creates "lowrank" Echoprint codes (roughly 5Hz, matches files)

    string code = pCodegen->getFullCodeString(); 

The code string is just a base64 encoding of a zlib compression of the original code string, which is a series of ASCII numbers.)

The makefile builds an example code generator that uses libcodegen, called "codegen." This code generator has more features -- it will output ID3 tag information and uses ffmpeg to decode any type of file. If you don't need to compile libcodegen into your app you can rely on this. Note that you need to have ffmpeg installed and accessible on your path for this to work.

    ./codegen.Linux-x86_64 billie_jean.mp3 10 10

Will take 10 seconds of audio from 10 seconds into the file and output JSON suitable for querying:

    {"metadata":{"artist":"Michael jackson", "release":"800 chansons des annes 80", "title":"Billie jean", "genre":"", "bitrate":192, "sample_rate":44100, "seconds":294, "filename":"billie_jean.mp3", "samples_decoded":220598, "given_duration":10, "start_offset":10, "version":3.14}, "code_count":76, "codes":"JxVlIuNwzAMQ1fxCDL133+xo1rnGqNAEcWy/ERa2aKeZmW...

You can POST this JSON directly to the Echo Nest's [song/identify](http://developer.echonest.com/docs/v4/song.html#identify "song/identify") (who has an Echoprint server booted), for example:

    curl -F "query=@post_string" http://developer.echonest.com/api/v4/song/identify?api_key=YOUR_KEY
    {"fp_lookup_time_ms": 21, "results": [{"songID": "SOAFVGQ1280ED4E371", "match_type": "fp", "title": "Billie Jean", "artist": "Michael Jackson", "artistID": "ARXPPEY1187FB51DF4", "score": 63, "release": "Thriller"}]
    (you can also use GET, see the API description)

Or you can host your own [Echoprint server](http://github.com/echonest/echoprint-server "echoprint-server") and ingest or query to that.

## FAQ

Q: I get "Couldn't decode any samples with: ffmpeg" when running the example code generator.

A: When running the example code generator (codegen.$(PLATFORM)) make sure ffmpeg is accessible to your path. Try running ffmpeg filename.mp3 on the file you are testing the code generator with. If it doesn't work, codegen won't work.

