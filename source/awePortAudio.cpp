#include "awePortAudio.h"

#include <cmath>
#include <cstdio>



static int PaCallback (
        const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData
        )
{
    PaCallbackPacket* data = (PaCallbackPacket*)userData;
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
    if (error()) return false;

    outstream_params.device = Pa_GetDefaultOutputDevice(); /* Default output device. */
    if (outstream_params.device == paNoDevice) {
        fprintf(stderr, "PortAudio [error] No default output device. \n");
        Pa_Terminate();
        return false;
    }

    pa_packet = { &output_queue, 0, 0 };

        outstream_params.channelCount = 2;  /* Stereo output. */
        outstream_params.sampleFormat = paFloat32;
        outstream_params.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultHighOutputLatency;
        outstream_params.hostApiSpecificStreamInfo = NULL;
        pa_error = Pa_OpenStream (
                &outstream, NULL,               /* One output stream, No input. */
                &outstream_params,              /* Output parameters.*/
                sample_rate, update_frame_rate, /* Sample rate, Frames per buffer. */
                paPrimeOutputBuffersUsingStreamCallback, PaCallback,
                &pa_packet
                );
        if (test_error()) return false;

        pa_error = Pa_StartStream(outStream);
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

    unsigned APortAudio::fplay (const AfBuffer& buffer)
    {
#ifdef AWE_CATCH_OUTPUT_OVERCLIPS
        unsigned int overclips = 0;
        
        for (unsigned int i=0; i<uFrameRate*2; i++)
        {
            Afloat smp = buffer->getSample(i);

            if (smp != smp) { smp = 0.0; }
            else if (smp >  1.0) { smp =  1.0; overclips++; }
            else if (smp < -1.0) { smp = -1.0; overclips++; }

            output_buffer.push(smp);
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
        pa_error = Pa_StopStream (outStream);
        pa_error = Pa_CloseStream(outStream);

        Pa_Terminate();

        return;
    }
