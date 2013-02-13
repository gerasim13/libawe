#ifndef AWE_TRACK_H
#define AWE_TRACK_H

#include "aweBuffer.h"

namespace awe {

    /**
     * Standard sound track
     */
    class ATrack {
        protected:	
            ArenderConfig   renderConfig;	//! output render configuration
            const float     bufferLength;	//! buffering delay in milliseconds

            AfBuffer    bufferOutput;	//! output buffer
            AfBuffer    bufferPool;	//! mixing buffer pool

        public:
            /**
             * Creates a track stream, which you can use to mix sounds into.
             */
            ATrack (unsigned long sample_rate, unsigned long frame_count)
                : renderConfig(sample_rate, frame_count),
                bufferLength(1000.f * (static_cast<float>(frame_count * 2.) / static_cast<float>(sample_rate))),
                bufferOutput(2, frame_count, true),
                bufferPool	(2, frame_count, true)
        { }
            ATrack (unsigned long sample_rate, unsigned long frame_count, const ArenderConfig &config)
                : renderConfig(config),
                bufferLength(1000.f * (static_cast<float>(frame_count * 2.) / static_cast<float>(sample_rate))),
                bufferOutput(2, frame_count, true),
                bufferPool	(2, frame_count, true)
        { }

            inline AfBuffer* const getOutput ()
            {
                return &bufferOutput;
            }

            inline unsigned long getFrameCount () const
            {
                return bufferOutput.getFrameCount( );
            }

            inline unsigned long getSampleRate () const
            {
                return renderConfig.targetSampleRate;
            }

            inline float getBufferLength () const
            {
                return bufferLength;
            }

            /*
            // virtual void pull () const = 0; //! Call to pull data into outputPool.
            // virtual bool flip () const = 0; //! Pre-flipBuffer() function to do some stuff. Return false to prevent flipBuffer from swapping the buffers.

            void skip () { //! Same as ATrack::pull() but sets ArenderConfig to SKIP so that it does nothing to the buffer.
            ARenderQ temp = renderConfig.renderQuality;
            renderConfig.renderQuality = SKIP;

            this->pull();

            renderConfig.renderQuality = temp;
            }
            */

            /**
             * Swaps the bufferPool into bufferOutput and resets bufferPool.
             * @return pointer to bufferOutput
             */
            AfBuffer* flipBuffer ();

            /**
             * Clean up and delete itself
             */
            void drop ();
    };

};
#endif
