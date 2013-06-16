//  aweSample.h :: Sound sample class
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_SAMPLE_H
#define AWE_SAMPLE_H

#include "aweLoop.h"
#include "aweSource.h"
#include "Filters/Mixer.h"

namespace awe {

    class Asample : public Asource
    {
        typedef Filter::AscMixer AscMixer;

        private:
            AiBuffer*   source;
            Afloat      sourcePeak;
            unsigned    sampleRate;
            std::string sampleName;

        protected: 
            AscMixer    mixer;
            Aloop       loop;

        public:
            /**
             * Default constructor
             */
            Asample(AiBuffer*   const &_source,
                    Afloat      const &_peak,
                    unsigned    const &_rate,
                    std::string const &_name = "Unnamed sample",
                    Aloop::Mode const &_loop = Aloop::Mode::__DEFAULT
            ) : Asource     (),
                source      (_source),
                sourcePeak  (_peak),
                sampleRate  (_rate),
                sampleName  (_name),
                loop(0, 0, _source == nullptr ? 0 : _source->getFrameCount(), _loop)
            {}

            Asample (Asample* _source) :
                Asource     (),
                source      (_source->getSource()), 
                sourcePeak  (_source->getPeak()),
                sampleRate  (_source->getSampleRate()),
                sampleName  (_source->getName()),
                mixer       (_source->cgetMixer()),
                loop        (_source->cgetLoop())
            {}

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

            virtual ~Asample() {}

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

            inline const AiBuffer * cgetSource() const { return source; }
            inline const AscMixer & cgetMixer () const { return mixer; }
            inline const Aloop    & cgetLoop  () const { return loop; }

            inline       AiBuffer * getSource()       { return source; }
            inline const Afloat   & getPeak  () const { return sourcePeak; }
            inline       AscMixer * getMixer ()       { return &mixer; }
            inline       Aloop    * getLoop  ()       { return &loop; }
            inline const double   & getPos   () const { return loop.now; }
            inline       void       setPos   (double pos) { loop.now = pos; }

            inline const unsigned    & getSampleRate() const { return sampleRate; }
            inline const std::string & getName      () const { return sampleName; }
            inline void pause() { loop.paused = true; }
            inline void stop () { loop = Aloop(loop.begin, loop.end, loop.mode, true); }
            inline void play () { loop.paused = false; }
            inline void play (Afloat vol, Afloat pan, bool looping = false)
            {
                mixer = AscMixer(vol, pan);
                loop.paused = false;
                loop.now    = loop.begin;
                loop.mode   = looping ? Aloop::Mode::REPEAT : Aloop::Mode::__DEFAULT;
            }

            /**
             * Skips the playing position of the sample to the supplied value.
             * If skip_silence is true, it will skip past the supplied value
             * until the current sample is not silent.
             *
             * @return number of silent samples skipped
             */
            size_t skip(const size_t &pos, const bool &skip_silence = false);

            // Asource pure function definition.
            virtual void make_active(void* userData) {
                stop();
                play();
            }

            virtual bool is_active() const { return !loop.paused; }
            virtual void render(AfBuffer &buffer, const ArenderConfig &config);
            virtual void drop() { delete source; source = nullptr; }
    };

}

#endif
