#include "../source/aweEngine.h"
#include "../source/aweSample.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread>

using namespace awe;

int main (int argc, char**argv)
{
    /* Number of frames to update per loop.
     * Anything above 2048 should be fine on most systems.
     */
#ifdef _DEBUG
    unsigned frameCount = 32768;
#else
    unsigned frameCount = 2048;
#endif

    /* Playing speed, done using linear resampling */
    float playSpeed = 1.0f;

    /* Process arguments */
    switch (argc) {
        case 4: argc--;
                playSpeed  = atof(argv[3]);
        case 3: argc--;
                frameCount = atoi(argv[2]);
        case 2: argc--;
                //  char* file = argv[1];
                break;
        default:
                printf("usage: file_test FILE_PATH [FRAME_RATE [PLAY_PITCH]]\n");
                printf("FRAME_RATE \t frame rate of output device\n");
                printf("PLAY_PITCH \t output render pitch (Note: Experimental)\n");
                return 0;
                break;
    }

    frameCount = (frameCount < 128) ? 128 : frameCount; /* Minimum of 128 frames per update */

    /*- Start engine -*/
    AEngine* engine = new AEngine (48000, frameCount);

    /*- Open file -*/
    Asample* smp = new Asample(argv[1]);

    if (smp->getSource() == nullptr) {
        printf("Failed to read file. Exiting... \n");
        return 0;
    }

    smp->skip(0, true); // skip through silence at the beginning
    engine->getMasterTrack().attach_source(smp);

    /*- Main loop -*/
    while (engine->getMasterTrack().count_active_sources() != 0)
    {
        if (engine->update() == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            /*- Console Visualization -*/
#ifndef _MSC_VER
            Asfloatf wvSum {{ .0f, .0f }}; // Waveform
            Asfloatf wvAvg {{ .0f, .0f }};
            Asfloatf wvMax {{ .0f, .0f }};
#else
            /* MSVC2012 does not have initializer lists, so you have to do it this way. */
            Asfloatf wvSum;
            Asfloatf wvAvg;
            Asfloatf wvMax;
            wvSum[0] = wvSum[1] = .0f;
            wvAvg[0] = wvAvg[1] = .0f;
            wvMax[0] = wvMax[0] = .0f;
#endif
            const AfBuffer& output = engine->getMasterTrack().getOutput();

            for (unsigned t=0; t<frameCount; t++)
            {
                Asfloatf wvVol = output.getFrame(t);
                wvVol.abs();
                wvAvg += wvVol;
                wvMax[0] = std::max(wvVol[0], wvMax[0]);
                wvMax[1] = std::max(wvVol[1], wvMax[1]);
            }

            wvAvg /= frameCount;
            wvAvg *= 8.0f * 3.14159265f;
            wvMax *= 8.0f * 3.14159265f;

            char wvM[17]; wvM[16] = 0; // Waveform, left
            char wvN[17]; wvN[16] = 0; // Waveform, right

            for (int i=0; i<16; i++)
                wvM[15-i] =
                    (wvMax[0]-i > 0) ? ((wvAvg[0]-i > 0) ? '<' : '=') : ' ';

            for (int i=0; i<16; i++)
                wvN[i] =
                    (wvMax[1]-i > 0) ? ((wvAvg[1]-i > 0) ? '>' : '=') : ' ';

            printf ("%s+%s\n", wvM, wvN);
        }
    } // elihw

    printf ("Done playing. Cleaning-up... \n");

    delete engine;

    printf ("Exit \n");

    return 0;
}
