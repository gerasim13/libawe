#include "aweEngine.h"
#include "awesndfile.h"
#include "aweSound.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

using namespace awe;

int main (int argc, char **argv)
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
	AEngine *engine = new AEngine (44100, frameCount);
    
	/*- Open file -*/
	Asample* smp = engine->readFromFile(argv[1], true);
    
	if (smp == nullptr) {
		printf("Failed to read file. Exiting... \n");
		return 0;
	}

	/*- Play source directly into Master Output Track -*/
	Asound* src = new Asound(smp, 1.0f, 0.0f, 1.0f, playSpeed);
	engine->getMasterTrack()->attachSource(src);
    
    src->skipTo(0, true);   // skip through silence at the beginning
    src->setState(RUNNING); // play

    /*- Main loop -*/
	while (engine->getMasterTrack()->countPlaying() != 0)
    {
		if (engine->update() == false) {
			engine->getOutputDevice()->sleep(1);
		} else {
			/*- Console Visualization -*/
			Afloat2 wvSum = { .0f, .0f }; // Waveform
            
            AfBuffer* output = engine->getOutput();
            
			for (unsigned t=0; t<frameCount; t++)
			{
				Afloat2 wvVol ( fabs(output->getSample(t)), fabs(output->getSample(t+1)) );
				wvVol *= 10.0f;
				wvSum += wvVol;
			}
			
			wvSum /= frameCount;
			wvSum *= 3.14159265f;

			char wvM[17]; wvM[16] = 0; // Waveform, left
			char wvN[17]; wvN[16] = 0; // Waveform, right

			for (int i=0; i<16; i++)
				wvM[15-i] =
					(wvSum.l-i)? ((wvSum.l-i<0)? ' ' : '-') : '*'; 
			for (int i=0; i<16; i++)
				wvN[i] =
					(wvSum.r-i)? ((wvSum.r-i<0)? ' ' : '-') : '*'; 


			printf ("%s+%s\n", wvM, wvN);
		} // esle
	} // elihw

	printf ("Done playing. Cleaning-up... \n");
	
	delete engine;
	
	printf ("Exit \n");
	
	return 0;
}
