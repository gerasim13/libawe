#ifndef AWE_SAMPLE_H
#define AWE_SAMPLE_H

#include <string>
#include "aweBuffer.h"

namespace awe {

    class Asample {
        protected:
            AiBuffer*       source;
            Afloat          sourcePeak;
            unsigned long   sampleRate;
            std::string     sampleName;

        public:
            float   defaultPitch;
            float   defaultSpeed;
            float   defaultGain;
            float   defaultPan;

        public:
            Asample ( const char* sample_name, AiBuffer* buffer, unsigned long sample_rate )
                : source (buffer),
                sampleRate (sample_rate),
                sampleName (sample_name),
                defaultPitch (1.0f), defaultSpeed (1.0f),
                defaultGain (1.0f), defaultPan (0.0f)
        {}

            inline bool setBuffer ( AiBuffer* _source, Afloat _peak )
            {
                if (source == nullptr) {
                    source = _source;
                    sourcePeak = _peak;
                    return true;
                } else {
                    return false;
                }
            }

            inline AiBuffer* getBuffer () const { return source; }
            inline       void  setName (const char* name) { sampleName = name; }
            inline const char* getName () const { return sampleName.c_str(); }
            inline Afloat        getSourcePeak () const { return sourcePeak; }
            inline unsigned long getSampleRate () const { return sampleRate; }
            inline double        getPlayLength () const { return (double)source->getFrameCount() / (double)sampleRate; }

            inline Afloat2 getFrame ( unsigned long offset )
            {
                return Afloat2 (
                        to_Afloat(source->getSample(offset * source->getChannelCount()))     * sourcePeak,
                        to_Afloat(source->getSample(offset * source->getChannelCount() + 1)) * sourcePeak
                        );
            }

            inline void drop () { delete source; delete this; }
    };

};
#endif
