#ifndef AWE_PORTAUDIO_H
#define AWE_PORTAUDIO_H

#include "aweBuffer.h"
#include <portaudio.h>
#include <queue>
#include <mutex>

namespace awe {

/** PortAudio class to handle communication with audio device.
 */
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
    PaError             mPAerror;
    PaStream          * mPAostream;
    PaStreamParameters  mPAostream_params;
    PaCallbackPacket    mPApacket;

    AfFIFOBuffer        mOutputQueue;
    std::mutex          mOutputMutex;

    unsigned int    mSampleRate;
    unsigned int    mFrameRate;

    //! Checks if PortAudio has an error
    bool test_error() const;

public:
    inline unsigned char pa_calls           () const { return mPApacket.calls; }
    inline double        pa_stream_cpu_load () const { return Pa_GetStreamCpuLoad(mPAostream); }
    inline double        pa_stream_time     () const { return Pa_GetStreamTime   (mPAostream); }
    inline AfFIFOBuffer& getFIFOBuffer      ()       { return mOutputQueue; }
    inline std::mutex  & getFIFOBuffer_mutex()       { return mOutputMutex; }

    //! Plays provided buffer. @returns underruns since last play.
    unsigned short int fplay(const AfBuffer& buffer);

    bool init(const unsigned int output_sample_rate, const unsigned int output_buffer_frame_count);
    void shutdown();
};
}
#endif
