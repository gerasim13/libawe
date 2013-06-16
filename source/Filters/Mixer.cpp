#include "Mixer.h"

namespace awe
{
namespace Filter
{

const Asfloatf AscMixer::xLinear(Afloat const &vol, Afloat const &pan)
{
    const Afloat l = (1.0f + pan) / 2.0f;
    return {{ l, 1.0f - l }};
}

const Asfloatf AscMixer::xSinCos(Afloat const &vol, Afloat const &pan)
{
    const Afloat p = M_PI * (1.0f + pan) / 4.0f;
    return {{ (float)cos(p) * vol, (float)sin(p) * vol }};
}

}
}
