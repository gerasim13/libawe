//  aweSample.cpp :: Sound sample class
//  Copyright 2012 - 2013 Keigen Shu

#include "aweSample.h"

namespace awe {

void Asample::render (AfBuffer &buffer, const ArenderConfig &config)
{
    if (source == nullptr) return;

    const double move_rate = static_cast<double>(sampleRate) / static_cast<double>(config.targetSampleRate);

    if (config.quality == ArenderConfig::renderQuality::SKIP) {
        return;
    } else if (config.quality == ArenderConfig::renderQuality::MUTE) {
        loop += config.targetFrameCount * move_rate;
        return;
    }

    if (source->getChannelCount() > 1)
    {
        for(size_t i = config.targetFrameOffset; i < config.targetFrameOffset + config.targetFrameCount; i++)
        {
            const unsigned long z = loop.unow() * source->getChannelCount();
            buffer.data()[i*2  ] += mixer.ifdoL(*(source->cdata()+z  ));
            buffer.data()[i*2+1] += mixer.ifdoR(*(source->cdata()+z+1));
            if (loop += move_rate) return;
        }
    } else if (source->getChannelCount() == 1) {
        for(size_t i = config.targetFrameOffset; i < config.targetFrameOffset + config.targetFrameCount; i++)
        {
            const unsigned long z = loop.unow();
            buffer.data()[i*2  ] += mixer.ifdoL(*(source->cdata()+z));
            buffer.data()[i*2+1] += mixer.ifdoR(*(source->cdata()+z));
            if (loop += move_rate) return;
        }
    }
}

size_t Asample::skip(const size_t &pos, const bool &skip_silence)
{
    size_t i = pos > loop.uend() ? loop.uend() : pos;
    size_t j = 0;

    if (skip_silence == true)
        for(; source->getSample(i) != 0 && i < loop.uend(); ++j)
            ++ i;

    loop.now = i / source->getChannelCount();

    return j;
}

}
