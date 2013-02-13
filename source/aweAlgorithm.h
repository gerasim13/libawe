//  aweAlgorithm.h :: <algorithm> tools for libawe
//  Copyright 2013 Keigen Shu

#ifndef AWE_ALGORITHM_H
#define AWE_ALGORITHM_H

#include <iterator>
#include <limits>
#include "aweDefine.h"

namespace awe
{
    struct AStereoMixer
    {
        Afloat      vol, pan;
        Afloat2     chgain;

        AStereoMixer(const Afloat& vol = 1.0f, const Afloat& pan = 0.0f) :
            vol(vol),
            pan(pan),
            chgain(1.0f, 1.0f)
        {
            chgain.l = (pan + 1.0f) / 2.0f; 
            chgain.r = 1.0f - chgain.l;

            // chgain.l = std::pow(Lchgain, 0.5f); // This code is stolen from Psycle.
            // chgain.r = std::pow(Rchgain, 0.5f); 
            chgain.l = std::log10((chgain.l*9.0f)+1.0f) * vol; // Faster approximation
            chgain.r = std::log10((chgain.r*9.0f)+1.0f) * vol;
        }

        inline void setVol(const Afloat &_vol) { AStereoMixer(_vol, pan); }
        inline void setPan(const Afloat &_pan) { AStereoMixer(vol, _pan); }

        inline void operator()(Aint2& f) const
        {
            f.l = to_Aint(to_Afloat(f.l) * chgain.l);
            f.r = to_Aint(to_Afloat(f.r) * chgain.r);
        }

        inline void operator()(Afloat2& f) const
        {
            f *= chgain;
        }

    };

}
#endif
