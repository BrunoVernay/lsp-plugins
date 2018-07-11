/*
 * Analyzer.cpp
 *
 *  Created on: 14 авг. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/dsp.h>
#include <core/Analyzer.h>

#include <math.h>

namespace lsp
{
    
    Analyzer::Analyzer()
    {
        nChannels       = 0;
        nMaxRank        = 0;
        nRank           = 0;
        nSampleRate     = 0;
        nBufSize        = 0;
        nFftPeriod      = 0;
        fReactivity     = 0.0f;
        fTau            = 1.0f;
        fRate           = 1.0f;
        fShift          = 1.0f;
        nReconfigure    = 0;
        nEnvelope       = envelope::PINK_NOISE;
        nWindow         = windows::HANN;
        bActive         = true;

        vChannels       = NULL;
        vSigRe          = NULL;
        vSigIm          = NULL;
        vFftRe          = NULL;
        vFftIm          = NULL;
        vWindow         = NULL;
        vEnvelope       = NULL;
    }
    
    Analyzer::~Analyzer()
    {
        destroy();
    }

    bool Analyzer::init(size_t channels, size_t max_rank)
    {
        destroy();

        size_t fft_size     = 1 << max_rank;
        size_t allocate     = (6 + 2 * channels) * fft_size;

        // Allocate data
        float *abuf         = new float[allocate];
        if (abuf == NULL)
            return false;

        // Allocate channels
        channel_t *clist    = new channel_t[channels];
        if (clist == NULL)
        {
            delete [] abuf;
            return false;
        }

        nChannels           = channels;
        nMaxRank            = max_rank;
        nRank               = max_rank;

        // Clear buffers
        dsp::fill_zero(abuf, allocate);

        // Initialize buffers
        vSigRe              = abuf;
        abuf               += fft_size;
        vSigIm              = abuf;
        abuf               += fft_size;
        vFftRe              = abuf;
        abuf               += fft_size;
        vFftIm              = abuf;
        abuf               += fft_size;
        vWindow             = abuf;
        abuf               += fft_size;
        vEnvelope           = abuf;
        abuf               += fft_size;

        // Initialize channels
        vChannels           = clist;
        for (size_t i=0; i<channels; ++i)
        {
            channel_t *c        = &vChannels[i];

            // FFT buffers
            c->vBuffer          = abuf;
            abuf               += fft_size;
            c->vAmp             = abuf;
            abuf               += fft_size;

            // Counters
            c->nCounter         = 0;
            c->bFreeze          = false;
            c->bActive          = true;
        }

        // Set reconfiguration flags
        nReconfigure        = R_ALL;

        return true;
    }

    void Analyzer::destroy()
    {
        if (vChannels != NULL)
        {
            delete [] vChannels;
            vChannels   = NULL;
        }

        if (vSigRe != NULL)
        {
            delete [] vSigRe;
            vSigRe      = NULL;
        }
    }

    void Analyzer::reconfigure()
    {
        if (!nReconfigure)
            return;

        size_t fft_size     = 1 << nRank;
//        size_t fft_csize    = (fft_size >> 1) + 1;
        nFftPeriod          = float(nSampleRate) / fRate;

        // Update envelope
        if (nReconfigure & R_ENVELOPE)
        {
            envelope::reverse_noise(vEnvelope, fft_size, envelope::envelope_t(nEnvelope));
            dsp::scale(vEnvelope, vEnvelope, fShift / fft_size, fft_size);
        }
        // Clear analysis
        if (nReconfigure & R_ANALYSIS)
        {
            for (size_t i=0; i<nChannels; ++i)
                dsp::fill_zero(vChannels[i].vAmp, fft_size);
        }
        // Update window
        if (nReconfigure & R_WINDOW)
            windows::window(vWindow, fft_size, windows::window_t(nEnvelope));
        // Update reactivity
        if (nReconfigure & R_TAU)
            fTau    = 1.0f - expf(logf(1.0f - M_SQRT1_2) / seconds_to_samples(float(nSampleRate) / float(nFftPeriod), fReactivity));
        // Update counters
        if (nReconfigure & R_COUNTERS)
        {
            // Get step aligned to 4-sample boundary
            size_t step     = fft_size / nChannels;
            step            = step - (step & 0x3);

            for (size_t i=0; i<nChannels; ++i)
                vChannels[i].nCounter   = i * step;
        }

        // Clear reconfiguration flag
        nReconfigure    = 0;
    }

    void Analyzer::process(size_t channel, const float *in, size_t samples)
    {
        if ((vChannels == NULL) || (channel >= nChannels))
            return;

        if (nReconfigure)
            reconfigure();

        // Process single channel
        // Get channel pointer
        size_t fft_size     = 1 << nRank;
        ssize_t fft_csize   = (fft_size >> 1) + 1;
        channel_t *c        = &vChannels[channel];

        // Process signal by channel
        while (samples > 0)
        {
            // Calculate amount of samples that can be appended to buffer
            ssize_t to_process  = nFftPeriod - c->nCounter;
            if (to_process <= 0)
            {
                // Perform FFT only for active channels
                if (!c->bFreeze)
                {
                    if ((bActive) && (c->bActive))
                    {
                        // Apply window to the temporary buffer
                        dsp::multiply(vSigRe, c->vBuffer, vWindow, fft_size);
                        // Do FFT
                        dsp::direct_fft(vFftRe, vFftIm, vSigRe, vSigIm, nRank);
                        // Leave only positive frequencies
                        dsp::combine_fft(vFftRe, vFftIm, vFftRe, vFftIm, nRank);
                        // Get complex argument
                        dsp::complex_mod(vFftRe, vFftRe, vFftIm, fft_csize);
                        // Mix with the previous value
                        dsp::mix(c->vAmp, c->vAmp, vFftRe, 1.0f - fTau, fTau, fft_csize);
                    }
                    else
                        dsp::fill_zero(c->vAmp, fft_size);
                }

                // Update counter
                c->nCounter        -= nFftPeriod;
            }
            else
            {
                // Limit number of samples to be processed
                if (to_process > ssize_t(samples))
                    to_process      = samples;

                // Move data in the buffer
                dsp::move(c->vBuffer, &c->vBuffer[to_process], fft_size - to_process);
                dsp::copy(&c->vBuffer[fft_size - to_process], in, to_process);

                // Update counters
                c->nCounter        += to_process;
                in                 += to_process;
                samples            -= to_process;
            }
        }
    }

    bool Analyzer::get_spectrum(size_t channel, float *out, const uint32_t *idx, size_t count)
    {
        if ((vChannels == NULL) || (channel >= nChannels))
            return false;

        channel_t *c        = &vChannels[channel];
        for (size_t i=0; i<count; ++i)
        {
            size_t j            = idx[i];
            out[i]              = c->vAmp[j] * vEnvelope[j];
        }

        return true;
    }

    void Analyzer::get_frequencies(float *frq, uint32_t *idx, float start, float stop, size_t count)
    {
        size_t fft_size     = 1 << nRank;
        size_t fft_csize    = (fft_size >> 1) + 1;
        float scale         = float(fft_size) / float(nSampleRate);

        // Initialize list of frequencies
        float norm          = logf(stop/start) / (count - 1);

        for (size_t i=0; i<count; ++i)
        {
            float f         = start * expf(i * norm);
            size_t ix       = scale * f;
            if (ix > fft_csize)
                ix                  = fft_csize;

            frq[i]          = f;
            idx[i]          = ix;
        }
    }

} /* namespace lsp */