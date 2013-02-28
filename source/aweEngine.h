//  aweEngine.h :: Core sound engine
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_ENGINE_H
#define AWE_ENGINE_H

#include "aweTrack.h"
#include "awePortAudio.h"

namespace awe {

    class AEngine
    {
        private:
            // Stuff that are thread sensitive.
            APortAudio  output_device;
        protected:
            // Stuff that are thread sensitive, but have their own critical section
            Atrack      master_output;
            
        public:
            // Stuff for the main process to fumble with.

            AEngine(size_t output_sample_rate = 48000, size_t output_frame_rate = 4096) :
                output_device(),
                master_output(output_sample_rate, output_frame_rate)
        {
            assert(output_device.init(output_sample_rate, output_frame_rate));
        }

            inline Atrack& getMasterTrack() { return master_output; }
            inline void update() { master_output.flip(); master_output.push(output_device.getFIFOBuffer()); }
    };

}

#endif
