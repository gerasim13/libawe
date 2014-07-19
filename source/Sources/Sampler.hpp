//  Sampler.hpp :: Single mono/stereo source sampler
//  Copyright 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef SOURCE_SAMPLER_H
#define SOURCE_SAMPLER_H

#include <memory>

#include "../Frame.hpp"
#include "../Sample.hpp"
#include "../Source.hpp"

namespace awe {
namespace Source {

/** Number of threads to initialize for soxr.
 *
 *  Set this to 1 for real-time rendering `OR` 0(auto) if you are
 *  rendering very large buffers (>8kB). Multi-threaded resampling is
 *  not recommended for small output buffers as they generate quite a
 *  lot of thread contention, which degrades system performance.
 */
static unsigned char SoXR_threads = 1;
struct SoXR;

class Sampler : public awe::Asource
{
public:
    using SamplePtr = std::shared_ptr<Asample>;

    SamplePtr       mSample;                //!< Sample to render.
    unsigned long   mOutputSampleRate;      //!< Output sampling rate.
    Asfloatf        mChannelGain;           //!< Channel volumes.

private:
    std::shared_ptr<SoXR> soxr;

public:
    Sampler(const SamplePtr &sample, unsigned long output_sample_rate, Asfloatf gain = Asfloatf({ 1.0f, 1.0f }));
    virtual ~Sampler();
    virtual void drop();
    virtual void make_active(void*);
    virtual bool is_active() const;
    virtual void render(AfBuffer& buffer, const ArenderConfig& config);
};

}
}

#endif
