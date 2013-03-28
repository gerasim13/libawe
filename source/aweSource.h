//  aweSource.h :: Sound source base class
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_SOURCE_H
#define AWE_SOURCE_H

#include <set>
#include "aweBuffer.h"

namespace awe {

    class Asource {
        protected:
             
        public:
            virtual bool isActive () const = 0;

            /**
             * Render function.
             */
            virtual void render (AfBuffer &targetBuffer, const ArenderConfig &targetConfig) = 0;

            /**
             * Called when the source is being released by a manager.
             */
            virtual void drop () = 0;
    };
    
    typedef std::set<Asource*> AsourceSet;
}

#endif
