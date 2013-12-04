#ifndef AWE_PORTAUDIO_H
#define AWE_PORTAUDIO_H

#include "aweBuffer.h"
#include <portaudio.h>
#include <queue>
#include <mutex>

namespace awe {

/* PortAudio object to communicate to the audio device. */
class APortAudio
{
public:
    struct PaCallbackPacket {
        std::mutex  *   mutex;      // Output FIFO buffer mutex
        AfFIFOBuffer*   output;     // Output FIFO buffer
        unsigned char   calls;      // Number of times PA ran this callback since last update.
        unsigned char   underflows; // Number of times PA reported underflow problems since last update.
    };
private:
    PaError             pa_error;

    PaStream          * pa_outstream;
    PaStreamParameters  pa_outstream_params;

    PaCallbackPacket    pa_packet;

    AfFIFOBuffer    output_queue;
    std::mutex      output_mutex;

    unsigned int    sample_rate;
    unsigned int    frame_rate;

    //! Checks if PortAudio has an error
    bool test_error() const;

public:
    inline unsigned char pa_calls           () const { return pa_packet.calls; }
    inline double        pa_stream_cpu_load () const { return Pa_GetStreamCpuLoad(pa_outstream); }
    inline double        pa_stream_time     () const { return Pa_GetStreamTime   (pa_outstream); }
    inline AfFIFOBuffer& getFIFOBuffer      ()       { return output_queue; }
    inline std::mutex  & getFIFOBuffer_mutex()       { return output_mutex; }

    //! Plays provided buffer. @returns underruns since last play.
    unsigned short int fplay(const AfBuffer& buffer);

    bool init(const unsigned int output_sample_rate, const unsigned int output_buffer_frame_count);
    void shutdown();
};
}
#endif
