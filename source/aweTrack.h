//  aweTrack.h :: Sound mixing track
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_TRACK_H
#define AWE_TRACK_H

#include <mutex>
#define __MUTEX__ std::lock_guard<std::mutex> lock(mutex);

#include <string>
#include <algorithm>
#include "aweDefine.h"
#include "aweBuffer.h"
#include "aweSource.h"
#include "Filters/Mixer.h"

namespace awe {

class Atrack : public Asource
{
    typedef Filter::AscMixer AscMixer;

private:
    mutable std::mutex mutex;
    std::string     name;
    ArenderConfig   config;

    AsourceSet      pool_sources;
    AfBuffer        pool_buffer;
    AfBuffer        output_buffer;

    AscMixer        mixer; //##TODO: Create FilterRack and use it here.
    bool            disabled;

public:
    Atrack (const size_t &sample_rate, const size_t &frames, const std::string &name = "Unnamed Track" ) :
        config          (sample_rate, frames),
        pool_buffer     (2, frames),
        output_buffer   (2, frames),
        mixer           (1.0f, 0.0f),
        disabled        (true)
    {}

    inline const ArenderConfig& cgetConfig() const { return config; }
    inline       ArenderConfig   getConfig()       { return config; }

    // TODO: Protect me
    inline const AsourceSet& cgetSources() const { return pool_sources; }
    inline       AsourceSet   getSources()       { return pool_sources; }

    // TODO: Protect me
    inline const AfBuffer& cgetOutput() const { return output_buffer; }
    inline       AfBuffer&  getOutput()       { return output_buffer; }

    inline const AscMixer& cgetMixer() const { return mixer; }
    inline       AscMixer&  getMixer()       { return mixer; }

    inline size_t count_active_sources() const
    {
        __MUTEX__
            size_t count = 0;
        for(const Asource* source : pool_sources)
        {
            if (source->is_active() == true)
                count++;
        }
        return count;
    }

    inline size_t count_sources() const {
        __MUTEX__
            return pool_sources.size();
    }

    inline void attach_source(Asource* const src)
    {
        __MUTEX__
            if (pool_sources.count(src) == 0) pool_sources.insert(src);
        disabled = false;
    }

    inline bool detach_source(Asource* const src)
    {
        __MUTEX__
            bool a = pool_sources.erase(src) != 0;
        disabled = pool_sources.empty();
        return a;
    }

    inline void pull()
    {
        __MUTEX__
            for(Asource* snd: pool_sources)
                if (snd->is_active() == true)
                    snd->render(pool_buffer, config);
    }

    inline void flip()
    {
        __MUTEX__
            output_buffer.reset(true);
        output_buffer.swap(pool_buffer);

        mixer.doI(output_buffer.begin(), output_buffer.end(), 2);
    }

    inline void push(AfFIFOBuffer &queue) const
    {
        __MUTEX__
            for(auto s : output_buffer)
                queue.push(s);
    }

    virtual void make_active(void* userData) {
        __MUTEX__
            disabled = count_sources() == 0;
    }

    virtual bool is_active() const { return !disabled; }
    virtual void render(AfBuffer &buffer, const ArenderConfig &config);
    virtual void drop() {}
};


}

#endif
