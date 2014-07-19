#include "../source/Engine.hpp"
#include "../source/Filters/Metering.hpp"
#include "../source/Sources/Sampler.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <memory>

using namespace awe;

int main (int argc, char** argv)
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
    auto engine = std::make_shared<AEngine>(48000, frameCount);

    /*- Open file -*/
    auto sample = std::make_shared<Asample>(argv[1]);
    if (!sample->getSource()) {
        fprintf(stderr, "Failed to read file. Exiting... \n");
        return 0;
    }

    auto sampler = std::make_shared<Source::Sampler>(sample, 48000 / playSpeed);
    auto meter   = std::make_shared<Filter::AscMetering>(48000, 0.0);

    // smp->skip(0, true); // skip through silence at the beginning
    engine->getMasterTrack().attach_source(sampler);
    engine->getMasterTrack().getRack().attach_filter(meter);

    /*- Main loop -*/
    while (engine->getMasterTrack().count_active_sources() != 0)
    {
        if (engine->update() == false) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } else {
            /*- Console Visualization -*/
            auto p = meter->getPeak();
            auto r = meter->getAvgRMS();
            p *= 16.0f;
            r *= 16.0f;

            char wvM[21]; wvM[20] = 0; // Waveform, left
            char wvN[21]; wvN[20] = 0; // Waveform, right

            for (int i=0; i<20; i++)
                wvM[19-i] =
                    (p[0]-i > 0) ? ((r[0]-i > 0) ? '#' : '=') : ' ';

            for (int i=0; i<20; i++)
                wvN[i] =
                    (p[1]-i > 0) ? ((r[1]-i > 0) ? '#' : '=') : ' ';

            printf ("%s+%s\n", wvM, wvN);
        }
    } // elihw

    printf ("Done playing. Cleaning-up... \n");

    // This part is optional because we're using shared_ptr.
    // meter   = nullptr;
    // sampler = nullptr;
    // sample  = nullptr;
    // engine  = nullptr;

    printf ("Exit \n");

    return 0;
}
