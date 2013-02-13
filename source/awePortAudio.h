#ifndef AWE_PORTAUDIO_H
#define AWE_PORTAUDIO_H

#include <queue>
#include <portaudio.h>
#include "aweBuffer.h"

namespace awe {

    struct AcallbackStatus {
        bool                called; /** Has PA executed the callback? */
        std::queue<Afloat>* output; /** Output buffer queue */
        unsigned    softUnderruns;  /** libawe reported undderun. */
        unsigned    hardUnderruns;  /** System reported underrun. */
    };

    /** PortAudio object to communicate to the audio device. */
    class APortAudio {
        private:
            PaStreamParameters  outParams;  /** PortAudio output stream parameters */
            PaStream*           outStream;  /** PortAudio output stream object */
            PaError             devError;   /** PortAudio system state */

            AcallbackStatus     devReturn;  /** Callback status */


            unsigned long   sampleRate;     /** output sampling rate */
            unsigned long   uFrameRate;     /** update frame rate */

        public:

            std::queue<Afloat>  outBuffer;  /** library output buffer queue */

            /** Blocks execution for a number of milliseconds */
            inline void sleep (long msec) { Pa_Sleep(msec); }
            inline double getCPULoad    () { return Pa_GetStreamCpuLoad(&outStream); }  /** @return PortAudio stream CPU usage */
            inline double getStreamTime () { return Pa_GetStreamTime   (&outStream); }  /** @return PortAudio stream time */

            inline bool     paCalled () const { return devReturn.called; }
            inline PaError  getError () const { return devError; }   /** @return PortAudio's error status */

            /**
             * Queues the data in the selected buffer for playing.
             * @return system reported underruns since last play.
             */
            unsigned fplay (AfBuffer* buffer);
            unsigned fplay (AfBuffer* buffer, size_t samples);

            /**
             * Terminates PortAudio and prints its error message to stderr if a PortAudio
             * function returns an error status code.
             * @return true if PortAudio has an error.
             */
            bool CheckError ();

            bool init (size_t rate, size_t frame_rate);
            void shutdown ();

            void test ();

    };

};
#endif
