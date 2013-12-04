//  aweEngine.h :: Core sound engine
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_ENGINE_H
#define AWE_ENGINE_H

#include "aweTrack.h"
#include "awePortAudio.h"

namespace awe {

class AEngine
{
protected:
    APortAudio  output_device;
    Atrack      master_output;

public:
    AEngine(size_t output_sample_rate = 48000, size_t output_frame_rate = 4096) :
        output_device(),
        master_output(output_sample_rate, output_frame_rate, "Output to Device")
        {
            if (output_device.init(output_sample_rate, output_frame_rate) == false)
                throw std::logic_error("libawe [exception] Could not initialize output device.");
        }

    ~AEngine() { output_device.shutdown(); }

    inline Atrack& getMasterTrack() { return master_output; }

    /** @todo Add some sort of multi-threading support. */
    inline bool update()
    {
        if (output_device.getFIFOBuffer().size() < master_output.getOutput().getSampleCount())
        {
            // Process stuff
            master_output.pull();
            master_output.flip();

            // Push to output device buffer
            output_device.getFIFOBuffer_mutex().lock();
            master_output.push(output_device.getFIFOBuffer());
            output_device.getFIFOBuffer_mutex().unlock();

            return true;
        } else {
            return false;
        }

    }
};

}

#endif
