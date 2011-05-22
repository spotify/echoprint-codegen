//
//  Copyright 2011 The Echo Nest. All rights reserved.
//

#include "FingerprintLowRank.h"
#include "Params.h"
#include "Fingerprint.h"

#ifdef __APPLE__
// iOS debugging
extern "C" {
	extern void NSLog(CFStringRef format, ...); 
}
#endif


FingerprintLowRank::FingerprintLowRank(Spectrogram* p16Spectrogram, Spectrogram* p512Spectrogram, int offset) 
    : _p16Spectrogram(p16Spectrogram), _p512Spectrogram(p512Spectrogram), _Offset(offset) { }

uint FingerprintLowRank::adaptiveOnsets(int ttarg, matrix_f&out, unsigned char*&band_for_onset, uint*&frame_for_onset) {
    //  E is a sgram-like matrix of energies.
    const float *pE;
    float *pO;
    int bands, frames, i, j, k;
    int deadtime = 32;
    double H[STFT_A_BANDS],taus[STFT_A_BANDS], N[STFT_A_BANDS];
    int contact[STFT_A_BANDS], lcontact[STFT_A_BANDS], tsince[STFT_A_BANDS];
    double overfact = 1.05;  /* threshold rel. to actual peak */
    uint onset_counter = 0;
    uint concordant_frame=0;

    // Do the blocking
    matrix_f E = _p16Spectrogram->getMatrix();
    matrix_f Eb = matrix_f(E.size1()/4, E.size2());
    
    for(i=0;i<(int)Eb.size1();i++) {
        for(j=0;j<(int)Eb.size2();j++) {
            Eb(i,j) = 0;
            // compute the rms of each block
            for(k=0;k<4;k++)
                Eb(i,j) = Eb(i,j) + (E((i*4)+k, j) * E((i*4)+k, j));
            Eb(i,j) = sqrtf(Eb(i,j) / 1.0); // i used to divide here / 4 (b/c the M in RMS) but dpwe doesn't
        }
    }
    
    frames = Eb.size1(); // 20K
    bands = Eb.size2(); // 9
    pE = &Eb.data()[0];

    // upper limit on # of onset-vectors is (frames/ttarg) * bands
	// on iOS this is not true?? so I did a *2 -- TODO, ask dpwe why this happens
    out = matrix_f(((frames/ttarg)*bands) * 2, 32);
    matrix_f O = matrix_f(frames, bands);
    pO = &O.data()[0];

    frame_for_onset = new uint[out.size1()];
    band_for_onset = new unsigned char[out.size1()];

    for (j = 0; j < bands; ++j) {
        N[j] = 0.0;
        taus[j] = 1.0;
        H[j] = pE[j];
        contact[j] = 0;
        lcontact[j] = 0;
        tsince[j] = 0;
    }
    // The p512 spec is here to have his data copied from him
    matrix_f R = _p512Spectrogram->getMatrix();
    
    uint R_frames = R.size1();

    // Let's make it all dB
    for(i=0;i<(int)(R.size1()*R.size2());i++) {
        R.data()[i] =(R.data()[i] * R.data()[i]);
        R.data()[i] = (10.0*log10(R.data()[i])+300.0)-300.0;
    }

    for (i = 0; i < frames; ++i) {
        concordant_frame = i/4;
        if(concordant_frame >= R_frames) concordant_frame = R_frames-1; 
        for (j = 0; j < 5; ++j) { // only look at the bottom 4 bands

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
    		    pO[j] = 1;

    		    // the concordant band is j*32 - 16 to j*32 + 16 -- there are 9 bands in E and 257 bands in R
    		    // the concordant frame is i/4 - it would be be 1/32 but remember the blocking=4, (makes it 1/8) and then R's hopsize is 2x E (1/4)
                if ( j == 0) {
                    // deal with the case of -16 to 16
                    int c =0;
                    for(int l=15;l>=0;l--, c++)
                        out(onset_counter, c) = R(concordant_frame, l);
                    for(int l= 0;l<16;l++, c++)
                        out(onset_counter, c) = R(concordant_frame, l);
                } else {

                    uint concordant_band_start = j*32 - 16;
                    // copy 32 floats from R at the right frame and the bands j*32 - 16 to j*32 + 16
                    memcpy(&out(onset_counter, 0), (const void*) &R(concordant_frame, concordant_band_start), 32*4);
                }

                // these are used for lookup later
                band_for_onset[onset_counter] = (unsigned char)j;
                frame_for_onset[onset_counter] = i;
                onset_counter++;

    		    /* apply deadtime */
    		    for(k = 1; k < ((i > deadtime)?deadtime:i); ++k) {
    			    pO[j - k*bands] = 0;
    		    }
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
    	pO += bands;
    }

    return onset_counter;
}


// dan is going to beat me if i call this "decimated_time_for_frame" like i want to
uint FingerprintLowRank::quantized_time_for_frame(uint frame) {
    double time_for_onset = _Offset + (double)frame / ((double)Params::AudioStreamInput::SamplingRate / 32.0);
    return (int)floor((time_for_onset * 1000.0) /  (float)QUANTIZE_MS) * QUANTIZE_MS;
}

void FingerprintLowRank::Compute() {
    uint onset_count;
    matrix_f out;
    uint *frame_for_onset;
    unsigned char *band_for_onset;
    unsigned short hash=0;
    uint actual_landmarks = 0;

    unsigned char landmark[7];
    for(uint i=0;i<7;i++) landmark[i] = 0;

    uint last_time[STFT_A_BANDS];
    for(uint i=0;i<STFT_A_BANDS;i++) last_time[i] = quantized_time_for_frame(0);

    unsigned short last_hash[STFT_A_BANDS];
    for(uint i=0;i<STFT_A_BANDS;i++) last_hash[i] = 0;    

	
    //  - Each landmark results in a 32-element subband spectrum
    onset_count = adaptiveOnsets(86, out, band_for_onset, frame_for_onset);
    _Codes.resize(onset_count);

    for(uint i=0;i<onset_count;i++) {
        // Only look at bands 0,1,2,3 apparently
        unsigned char band = band_for_onset[i];
        // - I cut off all values more than 5 dB below the max
        float mv = VectorUtility::maxv(&out(i,0), 32);
        for(uint j=0;j<32;j++)  {
            out(i,j) = out(i,j) - mv;
            if(out(i,j)+5 > 0) 
                out(i,j) = out(i,j) + 5.0;
            else 
                out(i,j) = 0.0;        
        }
        // - keep only the local maxima (x[n-1] < x[n] & x[n] >= x[n+1])
        // - Any nonzero values become 1, to give a single, 32-bit signature
        uint signature = 0;
        for(uint j=0;j<32;j++)  {
            if(j==0) 
                if (out(i,j)>=out(i,j+1)) signature |= 1 << j; 
            if(j<31 && j>0)
                if (out(i,j)>out(i,j-1) && out(i,j)>=out(i,j+1)) signature |= 1 << j;
            if(j==31)
                if (out(i,j)>out(i,j-1)) signature |= 1 << j;
        }
        
        // mix this into a single 16 bit integer with a hash function
        hash = (unsigned short) ( MurmurHash2( &signature, 4, HASH_SEED) & 0x0000ffff );
    
        // Final landmark is a combination of the two 16
        // bit landmarks, the band index (1..9), and the time difference between
        // them (quantized to newfp_time_res = 26ms units).  
        uint time_for_onset_ms_quantized = quantized_time_for_frame(frame_for_onset[i]);
        uint time_delta = time_for_onset_ms_quantized - last_time[band];
        if(time_delta > 65535) {
            fprintf(stderr, "wow. that is a long time between onsets %d\n", time_delta);
            time_delta = 0;
        }
        short time_delta_short = (short)time_delta;

        // skip pairs that have <QUANTIZE_MS distance between them.
        if(time_delta_short > 0) {
            memcpy(landmark+0, (const void*)&hash, 2);
            memcpy(landmark+2, (const void*)&last_hash[band], 2);
            memcpy(landmark+4, (const void*)&band, 1);
            memcpy(landmark+5, (const void*)&time_delta_short, 2);

            // Then hash it down to LANDMARK_BITMASK bits for my index table.
            uint hashed_landmark = MurmurHash2(&landmark, 7, HASH_SEED) & HASH_BITMASK;
            _Codes[actual_landmarks++] = FPCode(time_for_onset_ms_quantized, hashed_landmark);
            //fprintf(stderr, "landmark at frame %d (quantized %d) is %d from hash: %d + prevhash: %d + band: %d + time_delta: %d\n", 
            //    frame_for_onset[i], time_for_onset_ms_quantized, hashed_landmark, hash, last_hash, band, time_delta);
        } 
        last_hash[band] = hash;  
        last_time[band]= time_for_onset_ms_quantized;
    }
    
    _Codes.resize(actual_landmarks);    
    delete [] frame_for_onset;
    delete [] band_for_onset;
}


