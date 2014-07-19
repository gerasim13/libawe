//  Filters/Rack.hpp :: Mixer filter rack
//  Copyright 2013 - 2014 Chu Chin Kuan <keigen.shu@gmail.com>

#ifndef AWE_FILTER_RACK_H
#define AWE_FILTER_RACK_H

#include <cassert>
#include <cstdint>
#include <memory>
#include "../Filter.hpp"

namespace awe {
namespace Filter {

template< Achan Channels >
    class Rack : public Afilter< Channels >
{
public:
    using  filter_type = Afilter< Channels >;
    using pointer_type = std::shared_ptr< filter_type >;

private:
    std::vector< pointer_type > filters;

public:
    Rack() {}

    inline void reset_state() override {
        for(pointer_type filter : filters)
            filter->reset_state();
    }

    inline void filter_buffer(AfBuffer &buffer) override {
        for(pointer_type filter : filters)
            filter->filter_buffer(buffer);
    }

    inline void attach_filter( filter_type* filter) { filters.push_back(std::shared_ptr<filter_type>(filter)); }
    inline void attach_filter(pointer_type  filter) { filters.push_back(filter); }
    inline void detach_filter(size_t        filter) {
        auto it = filters.begin();
        while(it != filters.end())
        {
            if (filter == 0) {
                filters.erase(it);
            } else {
                --filter;
                ++it;
            }
        }
    }

    inline       pointer_type   getFilter(size_t filter)       { return filters[filter]; }
    inline const  filter_type* cgetFilter(size_t filter) const { return filters[filter].get(); }
};

}
}
#endif
