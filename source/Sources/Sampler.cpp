//  Sampler.cpp :: Single source sampler
//  Copyright 2014 Chu Chin Kuan <ksigen.shu@gmail.com>

#include "Sampler.hpp"

#include <cstdio>
#include "../soxr-0.1.1/src/soxr.h"

namespace awe {
namespace Source {

size_t soxr_input_fn(SoXR*, soxr_cbuf_t*, size_t);

struct SoXR
{
    soxr_t          soxr;       //!< SoXR object.
    soxr_error_t    soxr_error; //!< SoXR error string.

    std::shared_ptr<AiBuffer>
                iptr; //!< Input pointer
    size_t      chan; //!< Number of channels in sound sample.
    size_t      size; //!< Number frames in sound sample to play.
    size_t      read; //!< Number of frames read from input buffer.

    SoXR(const Sampler::SamplePtr& sample, unsigned long output_sample_rate)
        : soxr(0)
        , soxr_error(nullptr)
        , iptr(sample->getSource())
        , chan(sample->getChannelCount())
        , size(sample->getFrameCount())
        , read(0)
    {
        /* TODO Follow up bug report in soxr@sf.
         * This is a workaround for a bug in soxr-0.1.1 where I:O sampling
         * ratios very close(~10^-6) to 1:1 will crash the library.
         */
        if (sample->getSampleRate() == output_sample_rate) {
            fprintf(stderr, "libawe [debug] workaround bad I/O ratio crash on libSoXR.\n");
            return;
        }

        soxr_io_spec_t      const soxIOs = soxr_io_spec(SOXR_INT16_I, SOXR_FLOAT32_I);
        soxr_quality_spec_t const soxQs  = soxr_quality_spec(SOXR_MQ, 0);
        soxr_runtime_spec_t const soxRTs = soxr_runtime_spec(SoXR_threads);

        soxr = soxr_create(
                static_cast<double>  (sample->getSampleRate()),     // Input rate
                static_cast<double>  (output_sample_rate),          // Output rate
                static_cast<unsigned>(sample->getChannelCount()),   // Channel Count
                &soxr_error, &soxIOs, &soxQs, &soxRTs
                );
        if (soxr_error) { throw new std::runtime_error(soxr_error); }

        soxr_error = soxr_set_input_fn(
                soxr, (soxr_input_fn_t) soxr_input_fn,
                this, IO_BUFFER_SIZE
                );
        if (soxr_error) { throw new std::runtime_error(soxr_error); }
    }

    ~SoXR() {
        if (soxr != 0)
            soxr_delete(soxr);
    }
};

size_t soxr_input_fn(SoXR* ptr, soxr_cbuf_t* buf, size_t len)
{
    *buf = (ptr->iptr->data() + (ptr->read * ptr->chan));

    /****/ if (ptr->read >= ptr->size) {
        len = 0;
    } else if (ptr->read + len >= ptr->size) {
        len = ptr->size - ptr->read;
    } else if (ptr->read + len <  ptr->size) {
        /* do nothing */
    }

    ptr->read += len;
    return len;
}


Sampler::Sampler(const Sampler::SamplePtr &sample, unsigned long output_sample_rate, Asfloatf gain)
    : mSample           (sample)
    , mOutputSampleRate (output_sample_rate)
    , mChannelGain      (gain)
    , soxr              (std::make_shared<SoXR>(mSample, mOutputSampleRate))
{
    assert(mSample && "Invalid pointer to sample.");
}

Sampler::~Sampler () {
    soxr.reset();
}

void Sampler::drop() {
    soxr.reset();
}

void Sampler::make_active(void*) {
    soxr = std::make_shared<SoXR>(mSample, mOutputSampleRate);
}

bool Sampler::  is_active() const {
    if (soxr)
        return soxr->read < soxr->size;
    else
        return false;
}

void Sampler::render(AfBuffer& buffer, const ArenderConfig& config)
{
    // !workaround See TODO in SoXR::SoXR
    if (soxr->soxr == 0)
    {
        switch (config.quality)
        {
        case ArenderConfig::Quality::MUTE:
            soxr->read += config.frameCount;

        case ArenderConfig::Quality::SKIP:
            return;

        default:
            /****/ if (mSample->getChannelCount() == 2) {
                for (size_t f = 0; f < config.frameCount; f++) {
                    size_t i = soxr->read + f;
                    size_t j = config.frameOffset + f;
                    buffer.data() [j*2  ] += to_Afloat(soxr->iptr->at(i*2  )) * mChannelGain[0] * mSample->getPeak();
                    buffer.data() [j*2+1] += to_Afloat(soxr->iptr->at(i*2+1)) * mChannelGain[1] * mSample->getPeak();
                }
            } else if (mSample->getChannelCount() == 1) {
                for (size_t f = 0; f < config.frameCount; f++) {
                    size_t i = soxr->read + f;
                    size_t j = config.frameOffset + f;
                    buffer.data() [j*2  ] += to_Afloat(soxr->iptr->at(i  )) * mChannelGain[0] * mSample->getPeak();
                    buffer.data() [j*2+1] += to_Afloat(soxr->iptr->at(i  )) * mChannelGain[1] * mSample->getPeak();
                }
            }

            soxr->read += config.frameCount;
            return;
        }
    } else {
        AfBuffer oBuffer(buffer.size(), 0.f);

        switch (config.quality)
        {
        case ArenderConfig::Quality::MUTE:
            soxr_output(soxr->soxr, oBuffer.data(), config.frameCount);
            soxr->soxr_error = soxr_error(soxr->soxr);
            if (soxr->soxr_error) { throw std::runtime_error(soxr->soxr_error); }

        case ArenderConfig::Quality::SKIP:
            return;

        default:
            size_t oDone = soxr_output(soxr->soxr, oBuffer.data(), config.frameCount);
            soxr->soxr_error = soxr_error(soxr->soxr);
            if (soxr->soxr_error) { throw std::runtime_error(soxr->soxr_error); }

            /****/ if (mSample->getChannelCount() == 2) {
                for (size_t i = 0; i < oDone; i++) {
                    size_t j = config.frameOffset + i;
                    buffer.data() [j*2  ] += oBuffer[i*2  ] * mChannelGain[0] * mSample->getPeak();
                    buffer.data() [j*2+1] += oBuffer[i*2+1] * mChannelGain[1] * mSample->getPeak();
                }
            } else if (mSample->getChannelCount() == 1) {
                for (size_t i = 0; i < oDone; i++) {
                    size_t j = config.frameOffset + i;
                    buffer.data() [j*2  ] += oBuffer[i  ] * mChannelGain[0] * mSample->getPeak();
                    buffer.data() [j*2+1] += oBuffer[i  ] * mChannelGain[1] * mSample->getPeak();
                }
            }

            return;
        }
    }
}

}
}
