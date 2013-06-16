#include "aweTrack.h"

namespace awe {

void Atrack::render(AfBuffer &buffer, const ArenderConfig &config)
{
    __MUTEX__

    pull();
    flip();

    for(size_t i = config.targetFrameOffset, j = 0; i < config.targetFrameOffset + config.targetFrameCount; i++, j++)
    {
        Afloat &l = output_buffer.data()[j*2  ];
        Afloat &r = output_buffer.data()[j*2+1];

        mixer.ffdoL(l); buffer.data()[i*2  ] += l;
        mixer.ffdoR(r); buffer.data()[i*2+1] += r;
    }
}

}
