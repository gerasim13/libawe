#include "aweCompileConfig.h"
#include "awePortAudio.h"
#include "aweBuffer.h"
#include "aweTools.h"

#include <portaudio.h>
#include <cmath>
#include <cstdio>

// #include <thread>

namespace awe {

    static int paCallback (
            const void *inputBuffer, void *outputBuffer,
            unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags,
            void *userData
            ) {
        AcallbackStatus *data = (AcallbackStatus*)userData;
        float *out = (float*)outputBuffer;

        /* Prevent unused argument warnings. */
        (void) inputBuffer;
        (void) timeInfo;

        if (statusFlags == paOutputUnderflow)
            data->hardUnderruns++;

        /* Engine failed to update sooner. */
        if (data->called)
        {
            for (unsigned int i=0; i<framesPerBuffer; i++) {
                *out++ = 0;
                *out++ = 0;
            }

            data->softUnderruns++;
        } else {
            for (unsigned int i=0; i<framesPerBuffer; i++) {
                *out++ = data->output->front();
                data->output->pop();
                *out++ = data->output->front();
                data->output->pop();
            }

            data->called = true;
        }

        return 0;
    }


    bool APortAudio::init (size_t rate, size_t frame_rate)
    {
        sampleRate = rate;
        uFrameRate = frame_rate;

        devError = Pa_Initialize();
        if (CheckError()) return false;

        PaDeviceIndex device = Pa_GetDefaultOutputDevice(); /* Obtain default output device */
        if (device == paNoDevice) {
            fprintf(stderr,"PortAudio [error] No default output device. \n");
            Pa_Terminate();
            return false;
        }

        devReturn = { true, &outBuffer, 0, 0 };
        outParams = {
            device, 2, paFloat32, Pa_GetDeviceInfo(device)->defaultLowOutputLatency, NULL
        }; /* Default device, stereo, 32-bit floating-point sample type, default low output latency */

        devError = Pa_OpenStream (
                &outStream, NULL,               /* One output stream, No input. */
                &outParams, rate, uFrameRate,   /* Output parameters, Sample rate, Frames per buffer. */
                paPrimeOutputBuffersUsingStreamCallback, paCallback,
                &devReturn
                );
        if (CheckError()) return false;

        devError = Pa_StartStream(outStream);
        if (CheckError()) return false;

        return true;
    }

    bool APortAudio::CheckError ()
    {
        if (devError != paNoError) {
            Pa_Terminate();
            fprintf (stderr, "PortAudio [error] %d : %s \n", devError, Pa_GetErrorText(devError));
            return true;
        } else {
            return false;
        }
    }

    unsigned APortAudio::fplay (AfBuffer* buffer)
    {
        return this->fplay( buffer, buffer->getSampleCount() );
    }
    unsigned APortAudio::fplay (AfBuffer* buffer, size_t samples)
    {
        for (unsigned i=0; i<samples; i++)
            outBuffer.push(buffer->getSample(i));

        const size_t underruns = devReturn.softUnderruns + devReturn.hardUnderruns;

        devReturn.softUnderruns = 0;
        devReturn.hardUnderruns = 0;

        devReturn.called = false;

        return underruns;
    }
    // system sanity test
    void APortAudio::test ()
    {
        /*
        this->fplay( mixToStereo(
                    generateMonoSinefBuffer(200, sampleRate, 1),
                    generateMonoSinefBuffer(400, sampleRate, 1),
                    0
                    ) );
                    */
    }

    void APortAudio::shutdown ()
    {
        devError = Pa_StopStream (outStream);
        if (CheckError()) return;

        devError = Pa_CloseStream(outStream);
        if (CheckError()) return;

        Pa_Terminate();
        return;
    }

};
