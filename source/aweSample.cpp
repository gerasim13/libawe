#include "aweSample.h"

using namespace awe;

bool Asample::setup_source(AiBuffer* const _source, Afloat const &_peak, unsigned int const &_rate)
{
    if (source == nullptr) {
        source     = _source;
        sourcePeak = _peak;
        sampleRate = _rate;
        loop.end   = _source->getFrameCount();
        return true;
    } else {
        return false;
    }
}

size_t Asample::skip(size_t pos, bool skip_silence)
{
    if(skip_silence)
        while(source->getSample(pos) != 0 && pos < loop.uend())
            ++pos;

    loop.now = pos / 2;
}

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

    for(size_t i = config.targetFrameOffset; i < config.targetFrameOffset + config.targetFrameCount; i++)
    {
        const unsigned long long z = loop.sunow();
#ifdef _MSC_VER
        // Incomplete C++11 implementation in VS2012
        Asfloatf::data_type fdata = {
            to_Afloat(source->getSample(z * source->getChannelCount()    )),
            to_Afloat(source->getSample(z * source->getChannelCount() + 1))
        };

        Asfloatf frame(fdata);
#else
        Asfloatf frame ( Asfloatf::data_type {{
                to_Afloat(source->getSample(z * source->getChannelCount()    )),
                to_Afloat(source->getSample(z * source->getChannelCount() + 1))
                }} );
#endif
        mixer.mix(frame);

        buffer.at(i*2  ) += frame.data[0];
        buffer.at(i*2+1) += frame.data[1];

        if (loop += move_rate) return;
    }
}
