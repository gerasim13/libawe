//  aweTrack.h :: Sound mixing track
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_MIXER_H
#define AWE_MIXER_H

#include <algorithm>
#include "aweDefine.h"
#include "aweBuffer.h"
#include "aweSource.h"
#include "aweAlgorithm.h"

namespace awe {

    class Atrack {
        private:
            ArenderConfig   config;

            AsourceSet      pool_sources;
            AfBuffer        pool_buffer;
            AfBuffer        output_buffer;

            AschMixer       mixer;
        public:
            Atrack (const size_t &sample_rate, const size_t &frames) :
                config          (sample_rate, frames),
                pool_buffer     (2, frames),
                output_buffer   (2, frames),
                mixer           (1.0f, 0.0f)
                {
                }
            
            inline const AfBuffer& cgetOutput() const { return output_buffer; }
            inline       AfBuffer&  getOutput()       { return output_buffer; }

            inline size_t count_active_sources() const
            {
                size_t count = 0;
                for(Asource* source : pool_sources)
                {
                    if (source->is_active() == true)
                        count++;
                }

                return count;
            }

            inline void attach_source(Asource* const src)
            {
                if (pool_sources.count(src) == 0) pool_sources.insert(src);
            }

            inline bool detach_source(Asource* const src)
            {
                return pool_sources.erase(src) != 0;
            }

            inline void pull()
            {
                for(Asource* snd: pool_sources)
                    snd->render(pool_buffer, config);
            }

            inline void flip()
            {
                output_buffer.reset(true);
                output_buffer.swap(pool_buffer);

                mixer(output_buffer.begin(), output_buffer.end(), 2);
            }

            inline void push(AfFIFOBuffer &queue) const
            {
                for(auto s : output_buffer)
                    queue.push(s);
            }
    };


}

#endif
