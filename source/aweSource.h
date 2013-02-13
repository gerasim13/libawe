#ifndef AWE_SOURCE_H
#define AWE_SOURCE_H

#include <set>
#include "aweBuffer.h"

namespace awe {

    class Asource {
        protected:
            enum AState		state;			/** State of the sound sourcce. */
            unsigned long	lastUpdateTime;	/** Last update moment in milliseconds. To be used as timer sync in the future. */

        public:
            Asource () : state(INITIAL), lastUpdateTime(0) { }

            inline AState getState () const
            {
                return state;
            }

            virtual void setState (AState new_state)
            {
                state = new_state;
                return;
            }

            inline unsigned long getLastUpdateTime () const
            {
                return lastUpdateTime;
            }

            virtual void updateTime (unsigned long new_time)
            {
                lastUpdateTime = new_time;
                return;
            }

            /**
             * Render/mix into provided buffer and update sound.
             */
            virtual void render (const size_t &offset, AfBuffer* buffer, ArenderConfig* config) = 0;

            /**
             * This function is called whenever the mixer thinks it has no use of the source
             * anymore. You should manage it's own deletion.
             */
            virtual void drop () = 0;
    };

    typedef std::set<Asource*> AsrcSet;

};
#endif
