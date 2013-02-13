#include "aweBuffer.h"
#include "aweTools.h"

namespace awe
{

    AfBuffer* generateMonoSinefBuffer(const double &waveFreq, const unsigned &buffFreq, const double &length, const double &gain)
    {
        const double size = buffFreq * length;

        AfBuffer* buff = new AfBuffer(1, static_cast<size_t>(size), false);

        for (unsigned long t=0; t < static_cast<size_t>(size); t++)
            buff->push_back(
                    gain * std::sin(
                        2.0 * M_PI * waveFreq * (static_cast<double>(t) / size)
                        )
                    );

        return buff;
    }

    AiBuffer* generateMonoSineiBuffer(const double &waveFreq, const unsigned &buffFreq, const double &length, const double &gain)
    {
        const double size = static_cast<double>(buffFreq) * length;

        AiBuffer* bufi = new AiBuffer(1, static_cast<size_t>(size), false);
        for (double t=0; t < static_cast<size_t>(size); t += 1.0)
            bufi->push_back(
                    to_Aint(
                        gain * std::sin(2.0 * M_PI * waveFreq * (t / size))
                        )
                    );

        return bufi;
    }

};
