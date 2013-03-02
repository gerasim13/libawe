//  aweSample.h :: Sound sample
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_SAMPLE_H
#define AWE_SAMPLE_H

#include "aweLoop.h"
#include "aweSource.h"
#include "aweAlgorithm.h"

namespace awe {

    class Asample : public Asource
    {
        private:
            AiBuffer*   source;
            Afloat      sourcePeak;
            unsigned    sampleRate;
            std::string sampleName;

        protected: 
            AschMixer   mixer;
            Aloop       loop;
        public:
            /**
             * Default constructor
             */
            Asample (AiBuffer* const &_source, const Afloat &_peak, const unsigned &_rate, const std::string &_name = "Unnamed sample", const Aloop::Mode &_loop = Aloop::Mode::__DEFAULT) :
                Asource     (),
                source      (_source),
                sourcePeak  (_peak),
                sampleRate  (_rate),
                sampleName  (_name),
                loop(0, 0, _source == nullptr ? 0 : _source->getFrameCount(), _loop)
                {
                }

            /**
             * Load from file constructor.
             * This function blocks execution and leaves source as nullptr if it fails to load the sample from file.
             */
            Asample (const std::string &file, const Aloop::Mode &_loop = Aloop::Mode::__DEFAULT);

            /**
             * Load from memory constructor.
             * This function blocks execution and leaves source as nullptr if it fails to load the sample from memory.
             */
            Asample (char* mptr, const size_t &size, const std::string &_name = "Unnamed sample", const Aloop::Mode &_loop = Aloop::Mode::__DEFAULT);

            inline bool setSource (AiBuffer* const _source, const Afloat &_peak, const unsigned &_rate)
            {
                if (source == nullptr) {
                    source     = _source;
                    sourcePeak = _peak;
                    sampleRate = _rate;
                    loop.end   = _source->getFrameCount();
                    return true;
                } else {
                    return false;
                }
            }
            inline AiBuffer * getSource () { return source; }
            inline AschMixer* getMixer  () { return &mixer; }
            inline const double& getPos () const { return loop.now; }
            inline void setPos (const double &_pos) { loop.now = _pos; } 

            // Asource pure function definition.
            virtual bool is_active () const;
            virtual void render (AfBuffer &buffer, const ArenderConfig &config);
            virtual void drop () { delete source; source = nullptr; }
    };

}

#endif
