//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#include "Fingerprint.h"
#include "Params.h"


#ifdef __APPLE__
// iOS debugging
extern "C" {
	extern void NSLog(CFStringRef format, ...); 
}
#endif

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed ) {
    // MurmurHash2, by Austin Appleby http://sites.google.com/site/murmurhash/
    // m and r are constants set by austin
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	// Initialize the hash to a 'random' value
	unsigned int h = seed ^ len;
	// Mix 4 bytes at a time into the hash
	const unsigned char * data = (const unsigned char *)key;
	while(len >= 4)	{
		unsigned int k = *(unsigned int *)data;
		k *= m; 
		k ^= k >> r; 
		k *= m; 
		h *= m; 
		h ^= k;
		data += 4;
		len -= 4;
	}
	
	// Handle the last few bytes of the input array
	switch(len)	{
	    case 3: h ^= data[2] << 16;
	    case 2: h ^= data[1] << 8;
	    case 1: h ^= data[0];
	            h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;
	return h;
}

Fingerprint::Fingerprint(Spectrogram* p16Spectrogram, int offset) 
    : _p16Spectrogram(p16Spectrogram), _Offset(offset) { }


uint Fingerprint::adaptiveOnsets(int ttarg, matrix_u&out, uint*&onset_counter_for_band) {
    //  E is a sgram-like matrix of energies.
    const float *pE;
    int bands, frames, i, j, k;
    int deadtime = 32;
    double H[STFT_A_BANDS],taus[STFT_A_BANDS], N[STFT_A_BANDS];
    int contact[STFT_A_BANDS], lcontact[STFT_A_BANDS], tsince[STFT_A_BANDS];
    double overfact = 1.05;  /* threshold rel. to actual peak */
    uint onset_counter = 0;

    // Do the blocking
    matrix_f E = _p16Spectrogram->getMatrix();
    matrix_f Eb = matrix_f(E.size1()/4, E.size2());
    for(size_t x=0;x<Eb.size1();x++) {
        for(size_t y=0;y<Eb.size2();y++) {
            Eb(x,y) = 0;
            // compute the rms of each block
            for(k=0;k<4;k++)
                Eb(x,y) = Eb(x,y) + (E((x*4)+k, y) * E((x*4)+k, y));
            Eb(x,y) = sqrtf(Eb(x,y) / 1.0); // i used to divide here / 4 (b/c the M in RMS) but dpwe doesn't
        }
    }
    
    frames = Eb.size1(); // 20K
    bands = Eb.size2(); // 9
    pE = &Eb.data()[0];


    out = matrix_u(STFT_A_BANDS, frames); 

    onset_counter_for_band = new uint[STFT_A_BANDS];

    for (j = 0; j < bands; ++j) {
        onset_counter_for_band[j] = 0;
        N[j] = 0.0;
        taus[j] = 1.0;
        H[j] = pE[j];
        contact[j] = 0;
        lcontact[j] = 0;
        tsince[j] = 0;
    }

    for (i = 0; i < frames; ++i) {
        for (j = 0; j < STFT_A_BANDS; ++j) { 

    	    contact[j] = (pE[j] > H[j])? 1 : 0;

    	    if (contact[j] == 1 && lcontact[j] == 0) {
    	        /* attach - record the threshold level unless we have one */
    		    if(N[j] == 0) {
    			    N[j] = H[j];
    		    }
    		}
    		if (contact[j] == 1) {
    		    /* update with new threshold */
    		    H[j] = pE[j] * overfact;
    		} else {
    		    /* apply decays */
    		    H[j] = H[j] * exp(-1.0/(double)taus[j]);
    		}

    		if (contact[j] == 0 && lcontact[j] == 1) {
                /* detach */
                if (onset_counter_for_band[j] > 0   && (int)out(j, onset_counter_for_band[j]-1) > i - deadtime) {
                    // overwrite last-written time
                    --onset_counter_for_band[j];
                    --onset_counter;
                }
                out(j, onset_counter_for_band[j]++) = i;
                ++onset_counter;
                tsince[j] = 0;      
    	    }
    		++tsince[j];
    		if (tsince[j] > ttarg) {
    		    taus[j] = taus[j] - 1;
    		    if (taus[j] < 1) taus[j] = 1;
    		} else {
    		    taus[j] = taus[j] + 1;
    		}

    		if ( (contact[j] == 0) &&  (tsince[j] > deadtime)) {
    		    /* forget the threshold where we recently hit */
    		    N[j] = 0;
    		}
    		lcontact[j] = contact[j];
    	}
    	pE += bands;
    }

    return onset_counter;
}


// dan is going to beat me if i call this "decimated_time_for_frame" like i want to
uint Fingerprint::quantized_time_for_frame_delta(uint frame_delta) {
    double time_for_frame_delta = (double)frame_delta / ((double)Params::AudioStreamInput::SamplingRate / 32.0);
    return (int)floor((time_for_frame_delta * 1000.0) /  (float)QUANTIZE_DT_MS) * QUANTIZE_DT_MS;
}
uint Fingerprint::quantized_time_for_frame_absolute(uint frame) {
    double time_for_frame = _Offset + (double)frame / ((double)Params::AudioStreamInput::SamplingRate / 32.0);
    return (int)floor((time_for_frame * 1000.0) /  (float)QUANTIZE_A_MS) * QUANTIZE_A_MS;
}


void Fingerprint::Compute() {
    uint actual_codes = 0;
    unsigned char hash_material[5];
    for(uint i=0;i<5;i++) hash_material[i] = 0;

    uint * onset_counter_for_band;
    matrix_u out;
    uint onset_count = adaptiveOnsets(86, out, onset_counter_for_band);
    _Codes.resize(onset_count*6);

    for(unsigned char band=0;band<STFT_A_BANDS;band++) { 
        if(onset_counter_for_band[band]>0) {
            for(uint onset=0;onset<onset_counter_for_band[band]-5;onset++) {
                // What time was this onset at?
                uint time_for_onset_ms_quantized = quantized_time_for_frame_absolute(out(band,onset));

                // Build up 6 pairs of deltas from the successive onset times
                uint p[2][6];
                p[0][0] = (out(band,onset+1) - out(band,onset));
                p[1][0] = (out(band,onset+2) - out(band,onset+1));
                p[0][1] = (out(band,onset+1) - out(band,onset));
                p[1][1] = (out(band,onset+3) - out(band,onset+1));
                p[0][2] = (out(band,onset+1) - out(band,onset));
                p[1][2] = (out(band,onset+4) - out(band,onset+1));
                p[0][3] = (out(band,onset+2) - out(band,onset));
                p[1][3] = (out(band,onset+3) - out(band,onset+2));
                p[0][4] = (out(band,onset+2) - out(band,onset));
                p[1][4] = (out(band,onset+4) - out(band,onset+2));
                p[0][5] = (out(band,onset+3) - out(band,onset));
                p[1][5] = (out(band,onset+4) - out(band,onset+3));
            
                // For each pair emit a code
                for(uint k=0;k<6;k++) {
                    // Quantize the time deltas to 3ms
                    short time_delta0 = (short)quantized_time_for_frame_delta(p[0][k]);
                    short time_delta1 = (short)quantized_time_for_frame_delta(p[1][k]);
                    // Create a key from the time deltas and the band index
                    memcpy(hash_material+0, (const void*)&time_delta0, 2);
                    memcpy(hash_material+2, (const void*)&time_delta1, 2);
                    memcpy(hash_material+4, (const void*)&band, 1);
                    uint hashed_code = MurmurHash2(&hash_material, 5, HASH_SEED) & HASH_BITMASK;
                    // Set the code alongside the time of onset
                    _Codes[actual_codes++] = FPCode(time_for_onset_ms_quantized, hashed_code);
                    //fprintf(stderr, "whee %d,%d: [%d, %d] (%d, %d), %d = %u at %d\n", actual_codes, k, time_delta0, time_delta1, p[0][k], p[1][k], band, hashed_code, time_for_onset_ms_quantized);
                }
            }
        }
    }
    
    _Codes.resize(actual_codes);    
    delete [] onset_counter_for_band;
}


