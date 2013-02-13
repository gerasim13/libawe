#ifndef AWE_MIXTRACK_H
#define AWE_MIXTRACK_H

#include <set>
#include <stdexcept>

#include "aweSource.h"
#include "aweTrack.h"

#include "aweLimiter.h"
#include "aweAlgorithm.h"

namespace awe {

    /** @todo Move stuff like trackState and flip() to ATrack base class,
     *  All ATrack derived classes should follow the same processing system shown below.
     */

    /**
     * Asource* mixing track
     * 
     * How it works:
     * 
     * On the outside:
     * - Call work() to process some data
     * - Call dine() to grab final mix
     * 
     * On the inside:
     * 1. Call pull() to copy sounds(toss ingredients) into the bufferPool(bowl). Repeat until it is full.
     * 2. Call flip() to swap bufferPool(bowl) with bufferOutput(cooking pot).
     * 3. Call cook() to perform post-render on bufferOutput (cook food). (Technically speaking, this function is bundled with flip())
     * 4. Call dine() to empty the pot of food.
     * 5. Repeat cycle. You may work() and dine() at the same time.
     * 
     * This object contains two functions that operate with the two buffers;
     * bufferPool and bufferOutput. The first is update() which mixes and processes
     * sound and the second is feast(), which as the name implies, tries to eat the
     * final mix.
     * 
     * state INITIAL : bufferPool is fully filled, wants to flip and bufferOutput had been eaten.
     *                 work() Action: flip buffers and post-render bufferOutput. Switch to state PAUSED.
     *                 dine() Action: flip buffers and post-render bufferOutput. Switch to state RUNNING and return bufferOutput.
     * 
     * state RUNNING : bufferPool is busy pulling in data while bufferOutput had been eaten.
     *                 work() Action: continue pulling in data and switch to INITIAL when full.
     *                 dine() Action: FAIL: food is undercooked.
     * 
     * state PAUSED  : bufferPool is busy pulling in data while bufferOutput had not been eaten.
     *                 work() Action: continue pulling in data and switch to STOPPED when full.
     *                 dine() Action: switch to state RUNNING and return bufferOutput.
     * 
     * state STOPPED : bufferPool is fully filled, wants to flip but bufferOutput has not yet been eaten
     *                 work() Action: do nothing.
     *                 dine() Action: switch to state INITIAL and return bufferOutput.
     */
    class AMixTrack : public ATrack {
        protected:
            unsigned long   pullPos;    /** Current pulling position. */

        public:
            AState  trackState;

            AStereoMixer mixer; /** Stereo sound mixer. */

            AsrcSet sourceSet;  /** List of sounds connected to this MixTrack to pull from. */

            void *userData; /** Current user data state. */

            /**
             * This function pointer is called on every renderSingle() call except if set to NULL.
             * @param currRenderPos Current frame number being rendered by renderSingle().
             * @param thisBuffer    Pointer to outputPool, which is the buffer being processed by the renderer.
             * @param userData      Pointer to whatever you need on the function.
             */
            int (*sendThrough) (unsigned long currRenderPos, AfBuffer* buffer, void* userData);

            /**
             * This function pointer is called on every flip() call except if set to NULL.
             * @param thisBuffer    Pointer to outputBuffer, which is the buffer that has just been processed by the renderer.
             * @param userData      Pointer to whatever you need on the function.
             */
            int (*flipThrough) (AfBuffer* buffer, void* userData);

        public:
            AMixTrack (unsigned long sample_rate, unsigned long output_frame_rate, unsigned long update_frame_rate, float gain = 1.0f, float pan = 0.0f)
                : ATrack      (sample_rate, output_frame_rate, ArenderConfig(sample_rate, update_frame_rate)),
                pullPos     (0),
                trackState  (RUNNING),
                mixer       (gain, pan),
                userData    (new aLimiterData),
                sendThrough (nullptr),
                flipThrough (nullptr)
        {
            if (output_frame_rate < update_frame_rate)
                throw std::invalid_argument("libawe [exception@AMixTrack] updateFrameRate must be lower than outputFrameRate.");

            /** remove this requirement */
            if (output_frame_rate % update_frame_rate != 0)
                throw std::invalid_argument("libawe [exception@AMixTrack] outputFrameRate must be perfectly divisible to updateFrameRate with no remainders.");
        }

            /**
             * Inserts sound source into sourceSet.
             * @return false if element already exists in sourceSet
             */
            bool attachSource (Asource* src);

            /**
             * Removes sound source from sourceSet.
             * @return false if element not found
             */
            bool detachSource (Asource* src);

            /** Resets the state of all sound sources in sourceSet. */
            void setAllSourcesTo (awe::AState state);

            /** @return number of sound sources currently playing in this track */
            size_t countPlaying () const;

            /** this-> (╯°□°)╯︵ ┻━┻ <-buffers */
            AfBuffer* dine();

            /**
             * Does some work. 
             * 
             * @return true if pull is called
             */
            bool work(); 

            /** 
             * Renders a part of all sound sources into the bufferPool.
             */
            void pull ();

            /** 
             * Same as pull(), but with renderQuality set to SKIP.
             */
            void skip ();

            /**
             * Swaps contents in bufferPool into bufferOutput. bufferPool becomes empty.
             * @return pointer to bufferOutput; or
             *         0 if track is still rendering (trackState != AState::INITIAL)
             *         and cannot be flipped.
             *          
             */
            void flip ();

    };

};
#endif
