#ifndef AWE_SOUND_H
#define AWE_SOUND_H

#include "aweSource.h"
#include "aweSample.h"
#include "aweAlgorithm.h"

namespace awe {

    class Asound : public Asource {
        protected:
            Asample*	source;

        public:
            Afloat  pitch;  //! @todo Implement granular pitch shifting.
            Afloat  speed;  //! negative values == play in reverse
            AStereoMixer mixer;
            
            enum ALoopType	loopType;   //! type of playing loop
            double	currPosition;       //! current frame position

        public:
            Asound (Asample* src, Afloat gain, Afloat pan, Afloat pitch=1.0f, Afloat speed=1.0f, ALoopType loop = ONESHOT)
                : Asource(),
                source(src),
                pitch(pitch),
                speed(speed),
                mixer(gain, pan),
                loopType(loop),
                currPosition(0)
        {}

            Asound (Asample* src, ALoopType loop = ONESHOT)
                : Asource(),
                source(src),
                pitch(src->defaultPitch),
                speed(src->defaultSpeed),
                mixer(src->defaultGain, src->defaultPan),
                loopType(loop),
                currPosition(0)
        {}

            inline Asample* getSource () { return source; }

            /**
             * Updates the current frame position based on the loop type set on this sound.
             * @param frames number of frames to move.
             * @return current frame position
             */
            double run (double frames);

            /**
             * skip to offset. simply returns 0 without doing anything if out of bounds.
             * @param offset  Frame offset to skip to.
             * @param silence Set to true to continue skipping until end of silence.
             *                Use this to skip through silent parts in the buffer.
             * @return Current playback position if supplied offset is out of bounds.
             *          Otherwise, returns the offset skipped to.
             */
            unsigned long skipTo (unsigned long offset=0, bool silence = false);

            /**
             * render(mix) into provided buffer and update sound.
             */
            virtual void render (const size_t &offset, AfBuffer* buffer, ArenderConfig* config);

            virtual void drop ();
    };

};
#endif
