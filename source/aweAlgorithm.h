//  aweAlgorithm.h :: <algorithm> tools for libawe
//  Copyright 2013 Keigen Shu

#ifndef AWE_ALGORITHM_H
#define AWE_ALGORITHM_H

#include <cstdint>
#include <cassert>
#include <iterator>
#include <limits>
#include <type_traits>

#include "aweDefine.h"

namespace awe
{
    /**
     * Stereo channel mixer, for mixing gain and panning.
     * 
     */
    struct AschMixer
    {
        Afloat      vol, pan;
        Asfloatf    chgain;

        AschMixer(const Afloat& vol = 1.0f, const Afloat& pan = 0.0f) :
            vol(vol),
            pan(pan)
        {
            chgain[0] = (pan + 1.0f) / 2.0f; 
            chgain[1] = 1.0f - chgain[0];

            chgain[0] = std::log10((chgain[0]*9.0f)+1.0f) * vol;
            chgain[1] = std::log10((chgain[1]*9.0f)+1.0f) * vol;
        }

        inline void setVol(const Afloat &_vol) { AschMixer(_vol, pan); }
        inline void setPan(const Afloat &_pan) { AschMixer(vol, _pan); }

        inline void operator()(Asintf& f) const
        {
            f.data[0] = to_Aint(to_Afloat(f.data[0]) * chgain[0]);
            f.data[1] = to_Aint(to_Afloat(f.data[1]) * chgain[1]);
        }

        inline void operator()(Asfloatf& f) const
        {
            f *= chgain;
        }

        // C++ iterator template magic.
        // Apply on all stereo samples from begin to end.
        template <typename Iterator>
            inline void operator()(
                    Iterator begin,
                    Iterator end,
                    const std::uint8_t channels
                    ) const
            {
                assert(channels >= 2 && "AschMixer accepting iterator input requires at least two channels per frame.");
                if (std::numeric_limits< typename std::iterator_traits<Iterator>::value_type >::is_integer)
                {
                    for (auto iter = begin; iter != end; iter += channels)
                    {
                        *(iter)   = to_Aint(to_Afloat(*(iter  )) * chgain[0]);
                        *(iter+1) = to_Aint(to_Afloat(*(iter+1)) * chgain[1]);
                    }
                } else {
                    for (auto iter = begin; iter != end; iter += channels)
                    {
                        *(iter)   *= chgain[0];
                        *(iter+1) *= chgain[1];
                    }
                } 
            }

        // Overdub-mixing. Only works on iterators.
        template <typename InputIt, typename OutputIt>
            inline void operator()(
                    InputIt sbegin,
                    InputIt send,
                    OutputIt dbegin,
                    const std::uint8_t schannels,
                    const std::uint8_t dchannels
                    ) const
            {
                assert(schannels >= 2 && "AschMixer accepting iterator input requires at least two channels per frame.");
                assert(dchannels >= 2 && "Iterator accepting AschMixer output requires at least two channels per frame.");


                auto siter = sbegin;
                auto diter = dbegin;
                if (
                        std::is_integral< typename std::iterator_traits<InputIt>::value_type >::value &&
                        std::is_integral< typename std::iterator_traits<OutputIt>::value_type >::value
                   )
                {
                    for (; siter != send; siter += schannels, diter += dchannels)
                    {
                        *(diter)   = to_Aint(to_Afloat(*(siter  )) * chgain[0]);
                        *(diter+1) = to_Aint(to_Afloat(*(siter+1)) * chgain[1]);
                    }
                } else if (
                        std::is_integral      < typename std::iterator_traits<InputIt>::value_type >::value &&
                        std::is_floating_point< typename std::iterator_traits<OutputIt>::value_type >::value
                        )
                {
                    for (; siter != send; siter += schannels, diter += dchannels)
                    {
                        *(diter)   = to_Afloat(*(siter  )) * chgain[0];
                        *(diter+1) = to_Afloat(*(siter+1)) * chgain[1];
                    }
                } else if (
                        std::is_floating_point< typename std::iterator_traits<InputIt>::value_type >::value &&
                        std::is_integral      < typename std::iterator_traits<OutputIt>::value_type >::value
                        )
                {
                    for (; siter != send; siter += schannels, diter += dchannels)
                    {
                        *(diter)   = to_Aint(*(siter  ) * chgain[0]);
                        *(diter+1) = to_Aint(*(siter+1) * chgain[1]);
                    }
                } else {
                    for (; siter != send; siter += schannels, diter += dchannels)
                    {
                        *(diter)   = *(siter)   * chgain[0];
                        *(diter+1) = *(siter+1) * chgain[1];
                    }
                } 
            }

    };
}
#endif
