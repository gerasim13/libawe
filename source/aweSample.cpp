#include "aweSample.h"

namespace awe
{

void Asample::render (AfBuffer &buffer, const ArenderConfig &config)
{
    if (mSource == nullptr) return;

    const double move_rate = static_cast<double>(mSampleRate) / static_cast<double>(config.targetSampleRate);

    if (config.quality == ArenderConfig::renderQuality::SKIP) {
        return;
    } else if (config.quality == ArenderConfig::renderQuality::MUTE) {
        mLoop += config.targetFrameCount * move_rate;
        return;
    }

    if (mSource->getChannelCount() > 1)
    {
        for(size_t i = config.targetFrameOffset; i < config.targetFrameOffset + config.targetFrameCount; i++)
        {
            const unsigned long z = mLoop.unow() * mSource->getChannelCount();
            buffer.data()[i*2  ] += mMixer.ifdoL(*(mSource->cdata()+z  )) * mSourcePeak;
            buffer.data()[i*2+1] += mMixer.ifdoR(*(mSource->cdata()+z+1)) * mSourcePeak;
            if (mLoop += move_rate) return;
        }
    } else if (mSource->getChannelCount() == 1) {
        for(size_t i = config.targetFrameOffset; i < config.targetFrameOffset + config.targetFrameCount; i++)
        {
            const unsigned long z = mLoop.unow();
            buffer.data()[i*2  ] += mMixer.ifdoL(*(mSource->cdata()+z)) * mSourcePeak;
            buffer.data()[i*2+1] += mMixer.ifdoR(*(mSource->cdata()+z)) * mSourcePeak;
            if (mLoop += move_rate) return;
        }
    }
}

size_t Asample::skip(const size_t &pos, const bool &skip_silence)
{
    size_t i = pos > mLoop.uend() ? mLoop.uend() : pos;
    size_t j = 0;

    if (skip_silence == true)
        for(; mSource->getSample(i) != 0 && i < mLoop.uend(); ++j)
            ++ i;

    mLoop.now = i / mSource->getChannelCount();

    return j;
}

}