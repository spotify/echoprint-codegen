//
//  Copyright 2011 The Echo Nest. All rights reserved.
//


#include "Spectrogram.h"
#include "AudioStreamInput.h"
#include "VectorUtility.h"

Spectrogram::Spectrogram(AudioStreamInput* pAudio, uint hopSize, uint fftSize, uint windowSize) : 
    _HopSize(hopSize), _FFTSize(fftSize), _WindowSize(windowSize) {
    _pSamples = pAudio->getSamples();
    _NumSamples = pAudio->getNumSamples();
    Init();
}

Spectrogram::Spectrogram(const float* pSamples, uint numSamples, uint hopSize, uint fftSize, uint windowSize) : 
    _pSamples(pSamples), _NumSamples(numSamples), _HopSize(hopSize), _FFTSize(fftSize), _WindowSize(windowSize) {
    Init();
}


Spectrogram::~Spectrogram() {
#if !(USE_ACCELERATE)
    free(_pBuffer);
    free(_fftr_state);
#else
    free(_pBuffer->realp);
    free(_pBuffer->imagp);
    free(_pBuffer);
    vDSP_destroy_fftsetup(_fftr_state);
#endif
}

void Spectrogram::Init() {
    _Log2FFTSize = VectorUtility::log2max(_FFTSize);
    _NumFrames = (uint)ceilf((float)_NumSamples / (float)_HopSize);
    assert(_NumFrames > 0);
    _Data = matrix_f(1+((_NumSamples-_FFTSize)/_HopSize), (_FFTSize/2) + 1);
    
#if !(USE_ACCELERATE)
    // Create KISS FFT descriptor
    _fftr_state = kiss_fftr_alloc(_FFTSize,0,0,0);
    // and alloc a frame
    _pBuffer = (fft_complex_type*) malloc(sizeof(fft_complex_type) * _FFTSize);
#else
    // Accelerate.framework
    _fftr_state = vDSP_create_fftsetup(log2(_FFTSize), kFFTRadix2);
    // and alloc a frame
    _pBuffer = (fft_complex_type*) malloc(sizeof(fft_complex_type));
    _pBuffer->realp = (float*)malloc((_FFTSize/2+1) * sizeof(float));
    _pBuffer->imagp = (float*)malloc((_FFTSize/2+1 ) * sizeof(float));
#endif

}

//  Computes the power spectrogram of the given chunk of audio and stores it in an internal matrix.
void Spectrogram::Compute() {
    SimpleSTFT();
}

// valgrind --leak-check=yes ./codegen.Darwin-i386 billie_jean.wav 10 60

// A simple FFT that outputs magnitude
void Spectrogram::SimpleFFT(float*& input) {
    input[_FFTSize] = 0;
    input[_FFTSize+1] = 0;

#if !(USE_ACCELERATE)
    kiss_fftr(_fftr_state,input,_pBuffer);
#else

    float scale = 0.5;

    vDSP_ctoz ((COMPLEX *)input, 2, _pBuffer, 1, _FFTSize/2 + 1);

    vDSP_fft_zrip(_fftr_state, _pBuffer, 1, _Log2FFTSize, FFT_FORWARD);
    vDSP_vsmul(_pBuffer->realp, 1, &scale, _pBuffer->realp, 1, _FFTSize/2 + 1 ); 
    vDSP_vsmul(_pBuffer->imagp, 1, &scale, _pBuffer->imagp, 1, _FFTSize/2 + 1); 
#endif

    // this is abs(specgram(sig, FFTSize)).^2
#if USE_ACCELERATE
    // Apple's vDSP FFT packs the DC aand Nyquist components into one real/imag pair at the start
    for (uint i=1;i < (_FFTSize/2);++i) 
        input[i] = hypot(_pBuffer->realp[i], _pBuffer->imagp[i]);

    input[0] = _pBuffer->realp[0];
    input[_FFTSize/2] = _pBuffer->imagp[0];
#else
    // kissFFT does it as you'd expect
    for (uint i=0;i < (_FFTSize/2 + 1);++i) 
        input[i] = hypot(_pBuffer[i].r, _pBuffer[i].i);
#endif

}

// A simple STFT that outputs magnitude
void Spectrogram::SimpleSTFT() {

    float *ham = new float[_FFTSize];

	for(uint i = 0 ; i != _FFTSize ; i++)
        ham[i] = .5 - .5*cos( (2.*M_PI/(_FFTSize-1))*i);
        
    float *u = new float[_FFTSize+2];

    int c = 0;

	for(uint b=0;b<(_NumSamples-_FFTSize);b=b+_HopSize) {
		uint i = b;
		for(i=b;i<b+_FFTSize;i++) {
            u[i-b] = _pSamples[i]*ham[i-b];
		}
        
        SimpleFFT(u);

		for(i=0;i<_FFTSize/2 + 1;i++) {
            _Data(c,i) = u[i];
		}
		c = c+1;
	}
    delete [] ham;
    delete [] u;
}

