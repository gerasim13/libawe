#include <stdexcept>

#include "aweSound.h"
#include "aweTools.h"

namespace awe {

    //# TODO: Something doesn't seem right here.
    double Asound::run (double frames)
    {
        switch (loopType) {
            case ONESHOT:
                if (currPosition < source->getBuffer()->getFrameCount())
                    currPosition += frames;
                else
                    state = STOPPED;
                break;

            case FORWARD:
                if (currPosition <  source->getBuffer()->getFrameCount())
                    currPosition += frames;
                else
                    currPosition -= source->getBuffer()->getFrameCount();
                break;

            case BACKWARD:
                if (currPosition > 0)
                    currPosition -= frames;
                else
                    currPosition += source->getBuffer()->getFrameCount();
                break;

            case PINGPONG:
                if (currPosition <  source->getBuffer()->getFrameCount())
                    currPosition += frames;
                else {
                    currPosition -= source->getBuffer()->getFrameCount();
                    loopType = PONGPING;
                }
                break;

            case PONGPING:
                if (currPosition > 0)
                    currPosition -= frames;
                else {
                    currPosition += source->getBuffer()->getFrameCount();
                    loopType = PONGPING;
                } 
                break;

        }

        return currPosition;

    }


    unsigned long Asound::skipTo (unsigned long offset, bool silence)
    {
        if (offset > source->getBuffer()->getFrameCount())
            return currPosition;

        while (silence) {
            if (source->getBuffer()->getSample(offset * source->getBuffer()->getChannelCount())) // != 0
                silence = false;
            else
                offset++;
        }

        currPosition = offset;
        return offset;
    }

    /**
     * #TODO: - Proper resampling and interpolation.
     *        - Pitch shifting support.
     */
    void Asound::render (const size_t &offset, AfBuffer* buffer, ArenderConfig* config)
    {
        if (source->getBuffer() == nullptr)
            return; // pcmData is not ready.

        // Calculate difference between source sample rate and target sample rate...
        double targetRate = (double)source->getSampleRate() / (double)config->targetSampleRate;

        // Multiply with speed
        targetRate *= speed;

        unsigned long renderFrames = config->targetFrameCount;
        double        actualFrames = renderFrames * targetRate;

        // Skip through this render.
        if (config->renderQuality == SKIP) {
            return;
        } else if (config->renderQuality == MUTE) {
            currPosition += actualFrames;
            return;
        }

        for (unsigned long i=offset; i<renderFrames+offset; i++)
        {
            unsigned long z = run( targetRate );

            if (z >= source->getBuffer()->getFrameCount())
                z  = source->getBuffer()->getFrameCount() - 1; // overrun protection.

            try {
                Afloat2 frame = source->getFrame( z );

                mixer(frame);

                buffer->at(i*2)   += frame.l;
                buffer->at(i*2+1) += frame.r;

            } catch (std::out_of_range oor) {

                printf("libawe [!] OH SHI\n\n");
                printf("==== + libawe + ====\n");

                printf("libawe crashed while trying to read buffer from a sound source into an output buffer.\n");
                printf("Renderer requested %lu frames at %lu Hz.\n", config->targetFrameCount, config->targetSampleRate);
                printf("This sound is set to play at %.4f speed rate (Speed: %.4f, Pitch: %.4f, Resample: %.4f)\n", targetRate, speed, pitch, ((double)config->targetSampleRate) / ((double)source->getSampleRate()));
                printf("Reading %lu frames (%lu samples)  INTO  buffer of size %lu frames (%lu samples)\n",
                        source->getBuffer()->getFrameCount(), source->getBuffer()->getSampleCount(),
                        buffer->getFrameCount(), buffer->getSampleCount()
                      );
                printf("Currently rendering frame %lu out of %lu, reading frame %lu to speed through %lf frames on this call.\n", i-offset, renderFrames, z, actualFrames);
                printf("==== + libawe + ====\n\n");

                // Delete exception?

                state = CRASHED;
            }
            //! TODO: Some how support fade-speed. Maybe a Attack, Hold, Sustain, Release (AHSR) envelope factory?
            // fadeParameters(FADE_CONSTANT,FADE_CONSTANT,FADE_CONSTANT,FADE_CONSTANT);
        }
    }

    void Asound::drop () {}

};
