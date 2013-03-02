#include "awePortAudio.h"

#include <cmath>
#include <cstdio>

namespace awe {

    static int PaCallback (
            const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData
            )
    {
        APortAudio::PaCallbackPacket* data = (APortAudio::PaCallbackPacket*)userData;
        float *out = (float*)outputBuffer;

        /* Prevent unused argument warnings. */
        (void) inputBuffer;
        (void) timeInfo;

        if (statusFlags == paOutputUnderflow)
            data->underflows++;

        /* Library failed to update sooner. */
        if (data->output->size() < framesPerBuffer * 2)
        {
#ifdef AWE_CATCH_OUTPUT_UNDERRUNS
            for (unsigned int i=0; i<framesPerBuffer; i++) {
                *out++ = 0;
                *out++ = 0;
            }
#endif
        } else {
            for (unsigned int i=0; i<framesPerBuffer; i++) {
                *out++ = data->output->front(); data->output->pop();
                *out++ = data->output->front(); data->output->pop();
            }
        }

        data->calls++;
        return 0;
    }


    bool APortAudio::init (unsigned int output_sample_rate, unsigned int output_buffer_frame_count)
    {
        sample_rate = output_sample_rate;
        frame_rate  = output_buffer_frame_count;

        pa_error = Pa_Initialize();
        if (test_error()) return false;

        pa_outstream_params.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
        if (pa_outstream_params.device == paNoDevice) {
            fprintf(stderr, "PortAudio [error] No default output device. \n");
            Pa_Terminate();
            return false;
        }

        pa_packet = { &output_queue, 0, 0 };

        pa_outstream_params.channelCount = 2;  /* Stereo output. */
        pa_outstream_params.sampleFormat = paFloat32;
        pa_outstream_params.suggestedLatency = Pa_GetDeviceInfo(pa_outstream_params.device)->defaultHighOutputLatency;
        pa_outstream_params.hostApiSpecificStreamInfo = NULL;
        pa_error = Pa_OpenStream (
                &pa_outstream, NULL,        /* One output stream, No input. */
                &pa_outstream_params,       /* Output parameters.*/
                sample_rate, frame_rate,    /* Sample rate, Frames per buffer. */
                paPrimeOutputBuffersUsingStreamCallback, PaCallback,
                &pa_packet
                );
        if (test_error()) return false;

        pa_error = Pa_StartStream(pa_outstream);
        if (test_error()) return false;

        return true;
    }

    bool APortAudio::test_error() const
    {
        if (pa_error != paNoError) {
            Pa_Terminate();
            fprintf(stderr, "PortAudio [error] %d : %s \n", pa_error, Pa_GetErrorText(pa_error));
            return true;
        }

        return false;
    }

    unsigned short int APortAudio::fplay (const AfBuffer& buffer)
    {
#ifdef AWE_CATCH_OUTPUT_OVERCLIPS
        unsigned int overclips = 0;

        for (unsigned int i=0; i<frame_rate*2; i++)
        {
            Afloat smp = buffer.getSample(i);

            if (smp != smp) { smp = 0.0; }
            else if (smp >  1.0) { smp =  1.0; overclips++; }
            else if (smp < -1.0) { smp = -1.0; overclips++; }

            output_queue.push(smp);
        }

        if (overclips > 0)
            fprintf (stdout, "PortAudio [warn] %u overclip(s) on last update.\n", overclips);
#else
        std::copy(
                output_buffer.begin(),
                output_buffer.end(),
                std::back_inserter(queue)
                );
#endif

#ifdef AWE_CATCH_OUTPUT_UNDERFLOW
        if (pa_packet.underflows != 0)
            fprintf (stdout, "PortAudio [warn] %u device underflows(s) on last update.\n", pa_packet.underflows);
        if (pa_packet.calls > 1)
            fprintf (stdout, "PortAudio [warn] %u libawe underflows(s) on last update.\n", pa_packet.calls - 1 );

        pa_packet.calls = 0;
        pa_packet.underflows = 0;
#endif

        return 0;
    }

    void APortAudio::shutdown ()
    {
        pa_error = Pa_StopStream (pa_outstream);
        pa_error = Pa_CloseStream(pa_outstream);

        Pa_Terminate();

        return;
    }

}
