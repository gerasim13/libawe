#ifndef AWE_ENGINE_H
#define AWE_ENGINE_H

#include <cassert>
#include <system_error>

#include "aweSample.h"
#include "aweMixTrack.h"
#include "awePortAudio.h"

namespace awe {

    /**
     * libawe's sound output engine.
     */
    class AEngine {
        protected:
            size_t outFrameCount; /** Device output buffer frame count */
            size_t dspFrameCount; /** libawe engine update frame count */

            APortAudio*	outputDevice;
            AMixTrack *	masterOutput;

        public:
            /**
             * Initialize the master output track and starts up the output device.
             * This function will throw a system_error exception if the output device fails
             * to initialize. The APortAudio object should still be usable.
             * 
             * @param sample_rate       Output sampling rate
             * @param output_frame_rate Output buffer update frame rate
             * @param update_frame_rate Engine update frame rate
             * 
             * @exception std::system_error with PortAudio's error code and explanation
             *            string if output device fails to initialize.
             */
            AEngine ( size_t sample_rate = 48000, size_t output_frame_rate = 4096, size_t update_frame_rate = 512 )
                : outFrameCount (output_frame_rate),
                dspFrameCount (update_frame_rate),
                outputDevice (new APortAudio()),
                masterOutput (new AMixTrack(sample_rate, output_frame_rate, update_frame_rate))
        {
            assert(output_frame_rate > update_frame_rate);

            if (outputDevice->init(sample_rate, output_frame_rate) == false)
            {
                fprintf(stderr, "libawe [ERROR] Failed to initialize PortAudio.");
                throw std::system_error(
                        std::error_code(outputDevice->getError(), std::system_category()),
                        Pa_GetErrorText(outputDevice->getError())
                        );
            }
        }

            virtual ~AEngine ()
            {
                masterOutput->drop();
                outputDevice->shutdown();
            }

            /** @return master output mixer track */
            inline AMixTrack* getMasterTrack () const { return masterOutput; }

            /** @return master output mixer track buffer */
            inline AfBuffer* getOutput () const { return masterOutput->getOutput(); }

            /** @return master output buffer frame count */
            inline unsigned long getOutputFrameCount () const { return outFrameCount; }

            /** @return master output mixer track frame count */
            inline unsigned long getUpdateFrameCount () const { return dspFrameCount; }

            /** @return period of updates in the engine */
            inline unsigned long getPeriod () const { return outFrameCount / dspFrameCount; }

            /** @return master output mixer track sample rate */
            inline unsigned long getSampleRate () const { return masterOutput->getSampleRate(); }

            /** @return output PortAudio device */
            inline APortAudio* getOutputDevice () const { return outputDevice; }

            /**
             * Opens a sound file and decodes it using libsndfile.
             * 
             * @param file    file path to sound file to read. It's filename will be used to
             *                name the sample.
             * @param readNow setting this to false (default) will make libawe spawn a new 
             *                thread to load and decode the file. The pointer to the sound
             *                buffer in the sample will be NULL until the file has finished
             *                loading.
             * 
             * @return pointer to the sample containing the data read from the file; or
             *         nullptr on failure.
             */
            Asample* readFromFile ( const char* file, bool readNow = false );

            /**
             * Same as readFromFile(), but reads from a memory address instead.
             * 
             * @param name    name to put on sample
             * @param mptr    memory address pointer
             * @param size    number of bytes to read
             * @param readNow see readFromFile().
             * 
             * @return pointer to the sample containing the data read from the file; or
             *         nullptr on failure.
             */
            Asample* readFromMemory ( const char* name, char* mptr, size_t size, bool readNow = false );

            /**
             * Pushes data to output buffer if system requests for them.
             */
            bool update ();

            /**
             * Pushes data to output buffer.
             */
            void push ();
    };

};
#endif
